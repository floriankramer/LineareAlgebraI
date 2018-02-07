#include "Options.h"
#include <cstdio>
#include "ResourceStorage.h"
#include <fstream>
#include <string>
#include "warp/os.h"
#include <algorithm>
#include <cstring>


#define NUM_RESOURCE_EFFECTS 11

namespace ant{
	namespace options{

		int ressourceEffectMatrix[NUM_RESOURCES][NUM_RESOURCE_EFFECTS];

		int antSpawnCooldown = 1000;
		int antLifetime = 120000;
		int antCost=7;
		int combinedRessourceCostReduction=3;
		int antStrength = 5;
		int antConqueringPower = 1;
		int antSpeed = 200;
	
		int resourceMaxStored = 20;
		int resourceRegrowthCooldown = 1000;
		int resourceExhaustionThreshold = 3;
		int resourceExhaustionCooldown = 10000;
		int resourceStartStored = 10;
		int resourceRegrowthAmount = 1;
		int resourceHarvestTime = 1000;

		int tunnelCostPerUnit = 20;
		int tunnelBaseCost = 500;

		int conqueringThreshold = 10;
		int conqueringTime = 1000;

		bool debugRendering = false;


		int getNumResourceEffects(){
			return NUM_RESOURCE_EFFECTS;
		}


		bool isDebugRendering(){
			return debugRendering;
		}

		void setDebugRendering(bool b){
			debugRendering = b;
		}



		//Basic ant production and stats
		int getAntSpawnCooldown(){
			return antSpawnCooldown;
		}

		int getAntLifetime(){
			return antLifetime;
		}

		int getResourceRegrowthCooldown(){
			return resourceRegrowthCooldown;
		}

		int getAntCost(){
			return antCost;
		}

		int getCombinedRessourceCostReduction(){
			return combinedRessourceCostReduction;
		}
			
		int getAntStrength(){
			return antStrength;
		}

		int getResourceHarvestTime(){
			return resourceHarvestTime;
		}

		int getAntConqueringPower(){
			return antConqueringPower;
		}

		int getAntSpeed(){
			return antSpeed;
		}



		//Resource Node stats
		int getResourceMaxStored(){
			return resourceMaxStored;
		}


		int getResourceExhaustionThreshold(){
			return resourceExhaustionThreshold;
		}


		int getResourceExhaustionCooldown(){
			return resourceExhaustionCooldown;
		}


		int getResourceStartStored(){
			return resourceStartStored;
		}


		int getResourceRegrowthAmount(){
			return resourceRegrowthAmount;
		}



		//Tunnel stats
		int getTunnelCostPerUnit(){
			return tunnelCostPerUnit;
		}


		int getTunnelBaseCost(){
			return tunnelBaseCost;
		}



		//Territory node stats
		int getConqueringThreshold(){
			return conqueringThreshold;
		}


		int getConqueringTime(){
			return conqueringTime;
		}


		void loadOptions(){
			std::ifstream in;
			in.open(warp::toOsFileString("./tuning"));
			char line[4096];
			int ressourcesToLoad = 0;
			while(!in.eof()){
				in.getline(line, 4095);
				std::string s(line);
				if(s.size() > 0){
					s = s.substr(s.find_first_not_of(" "));
					if(s[0] != '#'){
						std::string key = s.substr(0, s.find_first_of(' '));
						if(ressourcesToLoad > 0){
							int i = 0;
							int value = 0;
							int sign = 1;
							for(char c : s){
								if(c == '\t'){
									ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][i] = value;
									i ++;
									value = 0;
									sign = 1;
								}
								else if(c == '-'){
									sign = -1;
								}
								else{
									value *= 10;
									value += sign * (c - 48);
								}
							}
							ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][i] = value;
							ressourcesToLoad --;
							// printf("Ressource %d, has values: %d %d %d %d %d %d %d %d %d %d %d\n", NUM_RESOURCES - ressourcesToLoad, ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][0], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][1], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][2], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][3], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][4], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][5], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][6], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][7], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][8], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][9], ressourceEffectMatrix[NUM_RESOURCES - ressourcesToLoad][10]);
						}
						else{
							if(key == "ResourceEffectMatrix"){
								ressourcesToLoad = NUM_RESOURCES;
							}
							else{	
								int value = std::stoi(s.substr(s.find_first_of('=') + 1));
								if(key == "AntSpawnCooldown"){
									antSpawnCooldown = value;
								}
								else if(key == "AntLifetime"){
									antLifetime = value;
								}
								else if(key == "ResourceRegrowthCooldown"){
									resourceRegrowthCooldown = value;
								}
								else if(key == "AntCost"){
									antCost = value;
								}
								else if(key == "CombinedRessourceCostReduction"){
									combinedRessourceCostReduction = value;
								}
								else if(key == "TunnelCostPerUnit"){
									tunnelCostPerUnit = value;
								}
								else if(key == "TunnelBaseCost"){
									tunnelBaseCost = value;
								}
								else if(key == "AntStrength"){
									antStrength = value;
								}
								else if(key == "ResourceMaxStored"){
									resourceMaxStored = value;
								}
								else if(key == "ResourceExhaustionThreshold"){
									resourceExhaustionThreshold = value;
								}
								else if(key == "ResourceExhaustionCooldown"){
									resourceExhaustionCooldown = value;
								}
								else if(key == "ResourceStartStored"){
									resourceStartStored = value;	
								}
								else if(key == "ResourceRegrowthAmount"){
									resourceRegrowthAmount = value;
								}
								else if(key == "ResourceHarvestTime"){
									resourceHarvestTime = value;
								}
								else if(key == "ConqueringThreshold"){
									conqueringThreshold = value;
								}
								else if(key == "AntConqueringPower"){
									antConqueringPower = value;
								}
								else if(key == "ConqueringTime"){
									conqueringTime = value;
								}
								else if(key == "AntSpeed"){
									antSpeed = value;
								}
							}
						}
					}
				}	
			}
			in.close();
		}

		int *getRessourceEffectMatrix(int resourceType){
			return ressourceEffectMatrix[resourceType];
		}
	}
}
