#ifndef ANTOPTIONS_H_
#define ANTOPTIONS_H_

namespace ant{

	namespace options{

		void loadOptions();

		bool isDebugRendering();

		void setDebugRendering(bool b);
		
		int *getRessourceEffectMatrix(int resourceType);

		
		int getAntSpawnCooldown();
		int getAntLifetime();
		int getAntCost();
		int getCombinedRessourceCostReduction();
		int getAntStrength();
		int getAntConqueringPower();
		int getAntSpeed();

		int getResourceRegrowthCooldown();
		int getResourceMaxStored();
		int getResourceExhaustionThreshold();
		int getResourceExhaustionCooldown();
		int getResourceStartStored();
		int getResourceRegrowthAmount();
		int getResourceHarvestTime();

		int getTunnelCostPerUnit();
		int getTunnelBaseCost();

		int getConqueringThreshold();
		int getConqueringTime();



		int getNumResourceEffects();

	}

}

#endif
