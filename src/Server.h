#ifndef SERVER_H_
#define SERVER_H_

#include "NetworkHelper.h"

namespace ant{
	namespace server{

		void start();

		void stop();
		bool isRunning();
		void processPackets();
		void broadcastPacket(PACKET_TYPE type, unsigned char *data, unsigned int length);
		void broadcastPacketExcluding(int clientId, PACKET_TYPE type, unsigned char *data, unsigned int length);

	}
}

#endif
