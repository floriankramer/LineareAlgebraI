#ifndef TUNNEL_H_
#define TUNNEL_H_

#include <vector>
#include "Border.h"

namespace ant{

	struct TunnelNode;

	struct WalledArea{
		std::vector<Border *> borders;
	};

	struct Tunnel : public WalledArea{
		int x1, y1, x2, y2, width = -1;
		TunnelNode *nodes[2];
		//std::vector<Border *> borders;
	};


	struct TunnelNode : public WalledArea{
		int x, y, size;
		std::vector<Tunnel *> tunnels;
		//std::vector<Border *> borders;
	};

}

#endif
