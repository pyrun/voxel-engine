#ifndef NETWORK_IDS_H
#define NETWORK_IDS_H

#include "BitStream.h"
#include "MessageIdentifiers.h"

enum {
    ID_SET_TIMED_MINE = ID_USER_PACKET_ENUM,
    ID_SET_BLOCK,
    ID_SET_CHUNK_DATA,
    ID_DEL_CHUNK,
    ID_GET_CHUNK_DATA,
    ID_WORLD_TRANSFER_FINISH,
    ID_SET_WORLD_SPAWN_POINT,
};

#endif // NETWORK_IDS_H
