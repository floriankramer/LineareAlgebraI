#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include "NetworkHelper.h"

namespace ant{
	namespace client{

		//attempt to connect to server at ip, for maximum timeout seconds
		bool connectTo(std::string ip, std::string username, int timeout);
		void processPackets();
		void stop();
		bool isConnected();
		void sendPacket(PACKET_TYPE type, const unsigned char *data, unsigned int length);
	}
}

#endif
