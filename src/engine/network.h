#ifndef NETWORK_H
#define NETWORK_H

#include "RakPeerInterface.h"
#include <string.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "ReplicaManager3.h"
#include "NetworkIDManager.h"
#include "VariableDeltaSerializer.h"
#include "RakSleep.h"
#include "GetTime.h"
#include "StringCompressor.h"
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "world.h"
#include "network_ids.h"
#include "object.h"
#include "player.h"

using namespace RakNet;

enum network_topology
{
    NONE,
	CLIENT,
	SERVER
};

class network
{
    public:
        network( config *config);
        virtual ~network();

        void start();
        void start_sever();
        void start_client( std::string ip = "127.0.0.1");

        void sendSpawnPoint( std::string name);
        void receiveSpawnPoint( BitStream *bitstream);

        void sendAllObjects( world *targetworld);
        void sendCreateObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, std::string type, glm::vec3 position);
        void receiveCreateObject( BitStream *bitstream);

        void sendBlockChange( world *world, Chunk *chunk, glm::ivec3 position, unsigned int id);
        void receiveBlockChange( BitStream *bitstream);

        void receiveChunk( BitStream *bitstream);
        void sendChunkData( std::string name, Chunk *chunk, RakNet::AddressOrGUID address, int l_start, int l_end);
        void sendChunk( std::string name, Chunk *chunk, RakNet::AddressOrGUID address);
        void sendAllChunks( world *world,RakNet::AddressOrGUID address);
        void receiveGetChunkData( BitStream *bitstream, RakNet::AddressOrGUID address);
        void sendGetChunkData( RakNet::AddressOrGUID address, Chunk *chunk, int start, int end);

        void sendWorldFinish( std::string name);
        void receiveWorldFinish( BitStream *bitstream);

        bool process( std::vector<world *> *world, player_handle *player);

        bool isNone() { return p_topology==NONE?true:false; }
        bool isServer() { return p_topology==SERVER?true:false; }
        bool isClient() { return p_topology==CLIENT?true:false; }

        void (*createWorld)(std::string name);
        world *(*getWorld)(std::string name);
        block_list *(*getBlocklist)(void);
    protected:

    private:

        RakNet::SocketDescriptor *p_socketdescriptor;
        NetworkIDManager p_networkIdManager;
        RakNet::RakPeerInterface *p_rakPeerInterface;
        RakNet::StringCompressor p_string_compressor;

        RakNet::Packet *p_packet;
        network_topology p_topology;

        int p_port;
        int p_maxamountplayer;
        std::string p_ip;
};

#endif // NETWORK_H
