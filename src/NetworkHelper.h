#ifndef ANT_NETWORK_HELPER_H_
#define ANT_NETWORK_HELPER_H_

namespace ant{
  enum PACKET_TYPE {
    ID_ASSIGNMENT,
    PLAYER_JOINED,
    PLAYER_LEFT,
    NEW_NODE_RESSOURCE,
    NEW_TUNNEL_NODE,
    NEW_TUNNEL,
    NEW_BORDER,
    ASSIGN_HIVE,
    NEW_NODE_HIVE,
    USERNAME,
    ANT_MOTION,
    NEW_ANT,
    ANT_DEATH,
    NODE_RESOURCE_REGROWTH,
    NODE_RESOURCE_HARVEST,
    DELETE_BORDER,
    SET_NODE_COMBAT_STATE,
    NEW_NODE_TERRITORY,
    NODE_TERRITORY_CONQUERED,
    NODE_TERRITORY_CONQUERING,
    NODE_HIVE_CONQUERED,
    NODE_HIVE_CONQUERING
  };

  void writeIntToBuffer(int value, unsigned char *buffer, unsigned int offset=0);
  int readIntFromBuffer(unsigned char *buffer, unsigned int offset=0);

  void writeFloatToBuffer(float value, unsigned char *buffer, unsigned int offset=0);
  float readFloatFromBuffer(unsigned char *buffer, unsigned int offset=0);
}

#endif
