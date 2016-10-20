#ifndef NET_ANT_H
#define NET_ANT_H

#include <unistd.h>


namespace ant{

	class Node;	

	class NetAnt{

		public:
			NetAnt(int id, int playerId);
			NetAnt(int x, int y, int strength, int id, int playerId);
			virtual ~NetAnt();
			virtual void render();
			virtual void update(float updateFactor);

			int getX();
			int getY();
			void setBaseX(int x);
			void setBaseY(int y);

			float getOffsetX();
			float getOffsetY();
			void setOffsetX(float x);
			void setOffsetY(float y);
			bool isDead();
			virtual void onDeath(){}
			void setSpeedX(float speedX);
			void setSpeedY(float speedY);
			void die();
			int getId();
			int getStrength();
			void setStrength(int i);
			void attack(NetAnt *a);
			virtual void setCurrentNode(Node *n);
			int getPlayerId();

      // resets the sinusodial offset applied to all ant motions
      void recomputeMotionNoise();

			void sendDeath();

      // returns x and y including motion noise
      float getVisualX();
      float getVisualY();


		protected:
			void sendSync();
			void sendCreate();
			Node *current;
			
		private:

			int  x, y, baseX, baseY;
			float speedX, speedY;
      // saves a normalized vector perpendicular to speedX, speedY
      float perpX, perpY;
      float motionNoisePhase;
			float offsetX, offsetY;
			int strength;
			bool dead;
			int id;
			int playerId;

			
	};



}
#endif
