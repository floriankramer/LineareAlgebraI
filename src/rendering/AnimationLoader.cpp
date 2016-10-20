/*
 * Parser.cpp
 *
 *  Created on: Mar 26, 2015
 *      Author: dwarf
 */

#include <iostream>
#include <string>
#include <fstream>
#include "rapidxml.hpp"
#include <vector>
#include <map>
#include "AnimationLoader.h"
#include "Character.h"
#include "TextureLoader.h"
#include "DisplayHandler.h"
#include "Warp.h"
#include "GraphicsOptions.h"
#include <cmath>
#include "os.h"

namespace warp {
	namespace animation{


		std::vector<Character *> characters;

		float toRadians(float f){
			return f / 360 * 2 * 3.1415;
		}

		enum LOOP_TYPE{
			LOOPING,
			NO_LOOPING
		};

		enum INTERPOLATION{
			INSTANT,
			LINEAR,
			QUADRATIC,
			CUBIC
		};

		enum OBJECT_TYPE{
			SPRITE,BONE,BOX,POINT,SOUND,ENTITY,VARIABLE
		};

		struct File{
			std::string name;
			float pivotX=0;
			float pivotY=1;
			std::string fileReference;
			int width, height;
			// a reference to the image store in this file
		};

		struct Folder{
			std::string name;
			std::map<int, File> files; // <file> tags
		};

		struct MapInstruction{
			int folder;
			int file;
			int tarFolder=-1;
			int tarFile=-1;
		};

		struct Ref{
			int parent=-1; // -1==no parent - uses ScmlObject spatialInfo as parentInfo
			int timeline;
			int key;
			int id;
			int type;
		};

		struct Bone{
			std::string name;
			float width;
			float height;
		};


		struct MainlineKey{
			int time=0;
			std::vector<Ref> boneRefs; // <bone_ref> tags
			std::vector<Ref> objectRefs; // <object_ref> tags
		};


		struct TimelineKey{
			int time=0;
			int curveType=LINEAR; // enum : INSTANT,LINEAR,QUADRATIC,CUBIC
			float c1;
			float c2;
			int id;

			//    TimelineKey interpolate(TimelineKey nextKey,int nextKeyTime,float currentTime)
			//    float getTWithNextKey(TimelineKey nextKey,int nextKeyTime,float currentTime)
			//    TimelineKey linear(TimelineKey keyB,t)
		};

		struct SpatialInfo{
			float x=0;
			float y=0;
			float angle=0;
			float scaleX=1;
			float scaleY=1;
			float a=1;
			int spin=1;

			void unmapFromParent(SpatialInfo parentInfo)
			{
				angle += parentInfo.angle;
				scaleX *= parentInfo.scaleX;
				scaleY *= parentInfo.scaleY;
				a *= parentInfo.a;

				if(x != 0 || y != 0){
					float preMultX = x * parentInfo.scaleX;
					float preMultY = y * parentInfo.scaleY;
					float s = sin(toRadians(parentInfo.angle));
					float c = cos(toRadians(parentInfo.angle));
					x = (preMultX * c) - (preMultY * s);
					y = (preMultX * s) + (preMultY * c);
					x += parentInfo.x;
					y += parentInfo.y;
				}
				else
				{
					// Mandatory optimization for future features
					x = parentInfo.x;
					y = parentInfo.y;
				}

			}

		};

		struct SpatialTimelineKey : public TimelineKey{
			SpatialInfo info;
			void paint();
		};

		struct SpriteTimelineKey : SpatialTimelineKey{
			int folder; // index of the folder within the ScmlObject
			int file;
			bool useDefaultPivot; // true if missing pivot_x and pivot_y in object tag
			float pivot_x=0;
			float pivot_y=1;

			//	void paint()
			//	TimelineKey linear(TimelineKey keyB,float t)
		};

		struct BoneTimelineKey : SpatialTimelineKey{
			// unimplemented in Spriter
			int length=200;
			int width=10;

			// override paint if you want debug visuals for bones
			//	void paint()
			//	TimelineKey linear(TimelineKey keyB,float t)
		};

		struct Timeline{
			std::string name;
			OBJECT_TYPE o;
			std::vector<BoneTimelineKey> boneKeys;
			std::vector<SpriteTimelineKey> spriteKeys;
			int id;
		};

		//struct Timeline{
		//
		//	Timeline() : o(SPRITE){
		//
		//	}
		//
		//	OBJECT_TYPE o;
		//	std::vector<TimelineKey> keys;
		//
		////	std::string name;
		////	int objectType; // enum : SPRITE,BONE,BOX,POINT,SOUND,ENTITY,VARIABLE
		////	std::vector<TimelineKey> keys; // <key> tags within <timeline> tags
		//};





		struct Animation{
			std::string name;
			int length;
			int loopType=LOOPING; // enum : NO_LOOPING,LOOPING
			std::vector<MainlineKey> mainlineKeys; // <key> tags within a single <mainline> tag
			std::vector<Timeline> timelines; // <timeline> tags

			//	void setCurrentTime(float newTime)
			//	void updateCharacter(MainlineKey mainKey,int newTime)
			//	MainlineKey mainlineKeyFromTime(int time)
			//	TimelineKey keyFromRef(Ref ref,int newTime)
		};


		struct CharacterMap{
			std::string name;
			std::vector<MapInstruction> maps; // <map> tags
		};

		struct Entity{
			std::string name;
			std::vector<CharacterMap> characterMaps; // <character_map> tags
			std::vector<Animation> animations; // <animation> tags
			std::vector<Bone> bones;

			Bone* getBoneById(int id, std::map<std::string, int> boneIdMap){
				for(unsigned int i = 0; i < bones.size(); i++){
					if(boneIdMap[bones[i].name] == id){
						return &bones[i];
					}
				}
				return NULL;
			}
		};

		struct ScmlObject{
			std::map<int, Folder> folders; // <folder> tags
			std::vector<Entity> entities; // <entity> tags
			std::vector<Folder> activeCharacterMap;

			int currentEntity;
			int currentAnimation;

			float currentTime;

			//	SpatialInfo characterInfo()
			//	void setCurrentTime(float newTime)
			//	void applyCharacterMap(CharacterMap charMap, bool reset)
		};


		float getFloat(rapidxml::xml_node<> *n, std::string attributeName, float def = 0){
			if(n->first_attribute(attributeName.c_str()) != NULL){
				return std::stof(n->first_attribute(attributeName.c_str())->value());
			}
			return def;
		}

		int getInt(rapidxml::xml_node<> *n, std::string attributeName, int def = 0){
			if(n->first_attribute(attributeName.c_str()) != NULL){
				return std::stoi(n->first_attribute(attributeName.c_str())->value());
			}
			return def;
		}

		std::string getString(rapidxml::xml_node<> *n, std::string attributeName, std::string def = std::string("")){
			if(n->first_attribute(attributeName.c_str()) != NULL){
				return std::string(n->first_attribute(attributeName.c_str())->value());
			}
			return def;
		}


		ScmlObject *s;

		void parseNode(rapidxml::xml_node<> *n){

			if(n == NULL)
				return;

			using namespace rapidxml;
			std::string name = std::string(n->name());

			if(name == "spriter_data"){
				parseNode(n->first_node());
			}
			else{

				if(name == "folder"){
					Folder f;
					f.name = getString(n, "name");
					for(xml_node<> *child = n->first_node(); child; child = child->next_sibling()){
						File fi;
						fi.name = getString(child, "name");
						fi.fileReference = fi.name;
						fi.pivotX = getFloat(child, "pivot_x");
						fi.pivotY = getFloat(child, "pivot_y");
						fi.width = getInt(child, "width");
						fi.height = getInt(child, "height");
						f.files[getInt(child, "id")] = fi;
					}
					s->folders[getInt(n, "id")] = f;
				}
				else if(name == "entity"){
					Entity e;
					e.name = getString(n, "name");
					for(xml_node<> *animationNode = n->first_node(); animationNode; animationNode = animationNode->next_sibling()){
						if(std::string(animationNode->name()) == "obj_info" && getString(animationNode, "type") == "bone"){
							Bone b;
							b.width = getFloat(animationNode, "w");
							b.height = getFloat(animationNode, "h");
							b.name = getString(animationNode, "name");
							e.bones.push_back(b);
						}
						else if(std::string(animationNode->name()) == "animation"){
							Animation a;

							a.length = getInt(animationNode, "length");
							a.loopType = getString(animationNode, "looping", "false") == "false" ? NO_LOOPING : LOOPING;
							a.name = getString(animationNode, "name");

							for(xml_node<> *timelineNode = animationNode->first_node(); timelineNode; timelineNode = timelineNode->next_sibling()){
								if(std::string(timelineNode->name()) == "mainline"){
									for(xml_node<> *mainlineNode = timelineNode->first_node(); mainlineNode; mainlineNode = mainlineNode->next_sibling()){
										MainlineKey k;
										k.time = getInt(mainlineNode, "time");
										for(xml_node<> *refNode = mainlineNode->first_node(); refNode; refNode = refNode->next_sibling()){
											if(std::string(refNode->name()) == "bone_ref"){
												Ref r;
												r.key = getInt(refNode, "key");
												r.timeline = getInt(refNode, "timeline");
												r.parent = getInt(refNode, "parent", -1);
												r.id = getInt(refNode, "id");
												r.type = 0;
												k.boneRefs.push_back(r);
											}
											else if(std::string(refNode->name()) == "object_ref"){
												Ref r;
												r.key = getInt(refNode, "key");
												r.timeline = getInt(refNode, "timeline");
												r.parent = getInt(refNode, "parent", -1);
												r.id = getInt(refNode, "id");
												r.type = 1;
												k.objectRefs.push_back(r);
											}
										}
										a.mainlineKeys.push_back(k);
									}
								}
								else if(std::string(timelineNode->name()) == "timeline"){
									Timeline t;
									t.name = getString(timelineNode, "name");
									t.id = getInt(timelineNode, "id");
									t.o = getString(timelineNode, "objectType", "sprite") == "sprite" ? SPRITE : BONE;
									for(xml_node<> *keyNode = timelineNode->first_node(); keyNode; keyNode = keyNode->next_sibling()){
										for(xml_node<> *detailNode = keyNode->first_node(); detailNode; detailNode = detailNode->next_sibling()){
											if(std::string(detailNode->name()) == "bone"){
												BoneTimelineKey k;
												k.id = getInt(keyNode, "id");
												k.time = getInt(keyNode, "time");
												SpatialInfo s;
												s.x = getFloat(detailNode, "x");
												s.y = getFloat(detailNode, "y");
												s.angle = getFloat(detailNode, "angle");
												s.scaleX = getFloat(detailNode, "scale_x", 1);
												s.scaleY = getFloat(detailNode, "scale_y", 1);
												s.a = getFloat(detailNode, "a", 1);
												s.spin = getInt(keyNode, "spin", 1);
												k.info = s;
												t.boneKeys.push_back(k);
											}
											else if(std::string(detailNode->name()) == "object"){
												SpriteTimelineKey k;
												k.id = getInt(keyNode, "id");
												k.time = getInt(keyNode, "time");
												SpatialInfo s;
												s.x = getFloat(detailNode, "x");
												s.y = getFloat(detailNode, "y");
												s.angle = getFloat(detailNode, "angle");
												s.scaleX = getFloat(detailNode, "scale_x", 1);
												s.scaleY = getFloat(detailNode, "scale_y", 1);
												s.a = getFloat(detailNode, "a", 1);
												s.spin = getInt(keyNode, "spin", 1);
												k.info = s;
												k.folder = getInt(detailNode, "folder");
												k.file = getInt(detailNode, "file");
												k.pivot_x = getFloat(detailNode, "pivot_x");
												k.pivot_y = getFloat(detailNode, "pivot_y");
												t.spriteKeys.push_back(k);
											}
										}
									}
									a.timelines.push_back(t);
								}
							}

							e.animations.push_back(a);
						}
					}

					s->entities.push_back(e);
				}

				parseNode(n->next_sibling());
			}
		}

		void transformSCMLObject(std::string texturePath){

			float pixelSize = options::getVirtualHeight() * 2 / 1080;

			std::map<std::string, int> boneIdMap; //map the scml bone names to ids
			int lastBoneId = 0;

			for(unsigned int i = 0; i < s->entities.size(); i++){
				Entity *e = &s->entities[i];
				Character *c = new Character();

				c->setName(e->name);

				for(unsigned int j = 0; j < e->bones.size(); j++){ //create InEngine bones from the raw bones
					Bone2 b;
					Bone *rb = &e->bones[j];
					b.id = lastBoneId;
					lastBoneId ++;
					boneIdMap[rb->name] = b.id;
					b.width = rb->width;
					b.height = rb->height;
					c->addBone(b);
				}

				for(unsigned int j = 0; j < e->animations.size(); j++){ // proccess the animations
					Animation *ra = & e->animations[j];
					Animation2 *a = new Animation2();
					a->length = ra->length;
					a->name = ra->name;

					for(unsigned int h = 0; h < ra->mainlineKeys.size(); h++){ //transform coordinates from relative to absolute
						MainlineKey *rm = &ra->mainlineKeys[h];

						std::vector<Ref> resolvedReferences;

						for(unsigned int ii = 0; ii < rm->boneRefs.size(); ii++){
							if(rm->boneRefs[ii].parent == -1){
								resolvedReferences.push_back(rm->boneRefs[ii]);
							}
						}

						unsigned int lastSize = -1;

						while(resolvedReferences.size() < rm->boneRefs.size() + rm->objectRefs.size()){
							lastSize = resolvedReferences.size();

							for(Ref r : rm->boneRefs){
								for(Ref r2 : resolvedReferences){
									if(r2.id == r.id){ //reference has already been resolved
										break;
									}
									else if(r2.id == r.parent && r2.type == 0){ // r2 is parent and bone
										Timeline* t1 = NULL, *t2 = NULL;
										for(unsigned int jj = 0; jj < ra->timelines.size(); jj++){ //find the two timelines the refs point to
											if(ra->timelines[jj].id == r.timeline){
												t1 = &ra->timelines[jj];
											}
											else if(ra->timelines[jj].id == r2.timeline){
												t2 = &ra->timelines[jj];
											}
											if(t1 != 0 && t2 != 0){
												if(r2.type == 0){
													t1->boneKeys[r.key].info.unmapFromParent(t2->boneKeys[r2.key].info); //do the unmapping
												}
												break;
											}
										}

										resolvedReferences.push_back(r);
										break;
									}
								}
							}

							if(lastSize == resolvedReferences.size()){ //did we actuall continue resolving, or are we stuck in a dead end
								warpLogger.err("unable to resolve all coordinates. Character %s", c->getName().c_str());
								break;
							}
						}

					}

					while(ra->mainlineKeys.size() > 0){
						int smallest = 2147483647;
						int smallestIndex = 0;
						for(unsigned int h = 0; h < ra->mainlineKeys.size(); h++){
							if(ra->mainlineKeys[h].time < smallest){
								smallestIndex = h;
								smallest = ra->mainlineKeys[h].time;
							}
						}

						MainlineKey *rm = &ra->mainlineKeys[smallestIndex];

						std::pair<int, std::vector<Key2>> keys(rm->time, std::vector<Key2>());

						for(Ref r : rm->boneRefs){
							Key2 k;

							Timeline *t = NULL;

							for(unsigned int jj = 0; jj < ra->timelines.size(); jj++){
								if(ra->timelines[jj].id == r.timeline){
									t = &ra->timelines[jj];
								}
							}

							if(t != NULL){
								BoneTimelineKey *b = &t->boneKeys[r.key];

								k.boneId = boneIdMap[t->name];
								k.rot = (b->info.angle - 90) / 360 * 2 * 3.1415;
								k.x = (b->info.x + cos(b->info.angle / 360 * 2 * 3.1415) * e->getBoneById(k.boneId, boneIdMap)->width * 0.5) * pixelSize;
								k.y = (b->info.y + sin(b->info.angle / 360 * 2 * 3.1415) * e->getBoneById(k.boneId, boneIdMap)->width * 0.5) * pixelSize;
								keys.second.push_back(k);

								if(a->name == "idle" && rm->time == 0){ //if the animation equals the idle animation and the time equals 0 copy the position to the default position of the bones;
									for(unsigned int i = 0; i < c->getBonesPointer()->size(); i++){
										if(c->getBonesPointer()->at(i).id == boneIdMap[t->name]){
											c->getBonesPointer()->at(i).x = k.x;
											c->getBonesPointer()->at(i).y = k.y;
											c->getBonesPointer()->at(i).rot = k.rot;
											break;
										}
									}
								}
							}

						}


						for(Ref r : rm->objectRefs){
							Timeline *t = NULL;

							for(unsigned int jj = 0; jj < ra->timelines.size(); jj++){
								if(ra->timelines[jj].id == r.timeline){
									t = &ra->timelines[jj];
								}
							}

							if(t != NULL){
								if(a->name == "idle" && rm->time == 0){


									for(Ref br : rm->boneRefs){
										if(br.id == r.parent){
											for(unsigned int jj = 0; jj < ra->timelines.size(); jj++){
												//										warpLogger.log(std::to_string(ra->timelines[jj].id) + " == " + std::to_string(br.timeline));
												if(ra->timelines[jj].id == br.timeline){
													int bid = boneIdMap[ra->timelines[jj].name];
													for(unsigned int i = 0; i < c->getBonesPointer()->size(); i++){
														if(c->getBonesPointer()->at(i).id == bid){
															SpriteTimelineKey *sk = &t->spriteKeys[r.key];
															c->getBonesPointer()->at(i).texture = s->folders[sk->folder].files[sk->file].fileReference;
															c->getBonesPointer()->at(i).width = s->folders[sk->folder].files[sk->file].width * pixelSize;
															c->getBonesPointer()->at(i).height = s->folders[sk->folder].files[sk->file].height * pixelSize;
															loadTexture(c->getBonesPointer()->at(i).texture, texturePath + c->getBonesPointer()->at(i).texture);
														}
													}
												}

											}
											break;
										}
									}

								}
							}
						}

						a->keyframes.push_back(keys);

						ra->mainlineKeys.erase(ra->mainlineKeys.begin() + smallestIndex);

					}


					for(unsigned int i = 0; i < a->keyframes.size() - 1; i++){
						for(unsigned int j = 0; j < a->keyframes[i].second.size(); j++){
							for(unsigned int h = 0; h < a->keyframes[i + 1].second.size(); h++){
								if(a->keyframes[i].second[j].boneId == a->keyframes[i + 1].second[h].boneId){
									a->keyframes[i].second[j].nextKey = &a->keyframes[i + 1].second[h];
									break;
								}
							}
						}
					}
					//			for(int h = 0; h < ra->mainlineKeys.size(); h++){
					//				ra->mainlineKeys[h].
					//			}

					c->addAnimation(a);
				}

				characters.push_back(c);
			}

		}



		Character *getCharacter(std::string name){
			for(Character *c: characters){
				if(c->getName() == name){
					return c->getCopy();
				}
			}
			return NULL;
		}


		void loadSCMLFile(std::string src, std::string texturePath){

			src = toOsFileString(src);

			//	if(pixelWidth == 0 || pixelHeight == 0){
			//		return NULL;
			//	}

			std::string text;
			std::string line;


			std::ifstream input(src.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

			if(!input.is_open()){
				std::cout << "could not open scml file" << toOsFileString(src) << std::endl;
				return;
			}

			int fileSize = input.tellg();
			input.close();
			input.open(src.c_str(), std::ios::binary);

			char * file = new char[fileSize + 1];
			for(int i = 0; i < fileSize; i++){
				file[i] = 0;
			}
			int size = 0;
			std::streamsize read = 1;
			while(input.read(file + size, fileSize)){ // > 0
				read = input.gcount();
				size += read;
			}

			//	for(int i = fileSize - 4; i < fileSize; i++){
			//		file[i] = 0;
			//	}
			file[fileSize] = 0;

			//	warpLogger.log(file);

			s = new ScmlObject();

			try{
				using namespace rapidxml;
				xml_document<> doc;    // character type defaults to char
				doc.parse<0>(file);    // 0 means default parse flags
				parseNode(doc.first_node());

				transformSCMLObject(texturePath);
			}
			catch(std::exception &e){
				warpLogger.log(e.what());
			}

			delete s;

		}

	}
}


