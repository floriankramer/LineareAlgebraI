#ifndef RESOURCE_STORAGE_H_
#define RESOURCE_STORAGE_H_

#define NUM_RESOURCES 3

#include "warp/Renderer.h"

namespace ant{

	extern warp::Color resourceColors[];

	class ResourceStorage{

		public:
			ResourceStorage();
			virtual ~ResourceStorage();
			float getPercentageFull(int resource);
			float getPercentageFullTotal();
			//returns the amount of resources removed from this storage
			virtual int getResources(int resource, unsigned int amount);
			//returns the amount of resources not stored in this storage
			virtual int storeResource(int resource, unsigned int amount);
			int getSpaceLeft(int resource);
			int getAmountStored(int resource);
			bool hasResource(int resource, int amount);

			void setResourceMaxTotal(unsigned int m);
			void setResourceMaxAll(unsigned int m);
			void setResourceMax(int resource, unsigned int m);
		private:
			unsigned int resources[NUM_RESOURCES];
			unsigned int maxResources[NUM_RESOURCES];
			unsigned int maxResourceTotal;
			bool checkForTotal(unsigned int additional = 0);

	};

}
#endif
