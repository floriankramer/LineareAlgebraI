#include "ResourceStorage.h"

namespace ant{

	warp::Color resourceColors[] = {
		warp::Color(0, 1, 0, 1), // green
		warp::Color(1, 1, 0, 1), // yellow
		warp::Color(1, .3, 0, 1), // orange
		warp::Color(0, 1, 0.94, 1), // turqoise
		warp::Color(0.47, 0, 1, 1), // purple
		warp::Color(0.399, 0.214, 0.015, 1), // brown
		warp::Color(1, 0, 1, 1)  // pink
	};

	ResourceStorage::ResourceStorage() : maxResourceTotal(1 << 31){
		for(int i = 0; i < NUM_RESOURCES; i++){
			resources[i] = 0;
			maxResources[i] = 0;
		}
	}


	ResourceStorage::~ResourceStorage(){

	}


	float ResourceStorage::getPercentageFull(int resource){
		if(resource >= NUM_RESOURCES){
			return 1;
		}
		return (resources[resource] + 0.0) / maxResources[resource];
	}

	int ResourceStorage::getAmountStored(int resource){
		return resources[resource];
	}

	float ResourceStorage::getPercentageFullTotal(){
		unsigned int sum = 0;
		for(int i = 0; i < NUM_RESOURCES; i++){
			sum += resources[i];
		}
		return (sum + 0.0) / maxResourceTotal;
	}


	int ResourceStorage::getResources(int resource, unsigned int amount){
		if(resource >= NUM_RESOURCES){
			return 0;
		}
		if(amount > resources[resource]){
			int r = resources[resource];
			resources[resource] = 0;
			return r;
		}
		else{
			resources[resource] -= amount;
			return amount;
		}
	}


	int ResourceStorage::storeResource(int resource, unsigned int amount){
		if(resource >= NUM_RESOURCES){
			return amount;
		}
		if(resources[resource] + amount <= maxResources[resource] && checkForTotal(amount)){
			resources[resource] += amount;
			return 0;
		}
		else{
			int left = getSpaceLeft(resource);
			resources[resource] += left;
			return amount - left;
		}	
	}


	int ResourceStorage::getSpaceLeft(int resource){
		unsigned int sum = 0;
		for(int i = 0; i < NUM_RESOURCES; i++){
			sum += resources[i];
		}
		if(maxResourceTotal - sum > maxResources[resource] - resources[resource]){
			int r = maxResources[resource] - resources[resource];
			return r;
		}
		else{	
			return maxResourceTotal - sum;
		}
	}

	bool ResourceStorage::hasResource(int resource, int amount){
		if(resource > NUM_RESOURCES){
			return false;
		}
		return resources[resource] >= amount;
	}


	bool ResourceStorage::checkForTotal(unsigned int additional){
		unsigned int sum = additional;
		for(int i = 0; i < NUM_RESOURCES; i++){
			sum += resources[i];
		}
		return sum <= maxResourceTotal;
	}


	void ResourceStorage::setResourceMaxTotal(unsigned int m){
		maxResourceTotal = m;
	}


	void ResourceStorage::setResourceMaxAll(unsigned int m){
		for(int i = 0; i < NUM_RESOURCES; i++){
			maxResources[i] = m;
		}
	}


	void ResourceStorage::setResourceMax(int resource, unsigned int m){
		if(resource < NUM_RESOURCES){
			maxResources[resource] = m;
		}
	}


}
