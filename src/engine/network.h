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

        void sendSpawnPoint( std::string name, RakNet::AddressOrGUID address);
        void receiveSpawnPoint( BitStream *bitstream);

        void sendAllObjects( world *targetworld, RakNet::AddressOrGUID address);
        void sendCreateObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, std::string type, glm::vec3 position, unsigned int id);
        void receiveCreateObject( BitStream *bitstream);

        void sendMatrixObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, object *object);
        void receiveMatrixObject( BitStream *bitstream);

        void sendDeleteObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, unsigned int id);
        void receiveDeleteObject( BitStream *bitstream);

        void sendBlockChange( world *world, Chunk *chunk, glm::ivec3 position, unsigned int id);
        void receiveBlockChange( BitStream *bitstream);

        void receiveChunk( BitStream *bitstream);
        void sendChunkData( std::string name, Chunk *chunk, RakNet::AddressOrGUID address, int l_start, int l_end);
        void sendChunk( std::string name, Chunk *chunk, RakNet::AddressOrGUID address);
        void sendAllChunks( world *world,RakNet::AddressOrGUID address);
        void receiveGetChunkData( BitStream *bitstream, RakNet::AddressOrGUID address);
        void sendGetChunkData( RakNet::AddressOrGUID address, Chunk *chunk, int start, int end);

        void sendBindPlayer( player *player, RakNet::AddressOrGUID address);
        void receiveBindPlayer( BitStream *bitstream, player_handle *players, world *world);

        void sendWorldFinish( std::string name, RakNet::AddressOrGUID address);
        void receiveWorldFinish( BitStream *bitstream);

        bool process( std::vector<world *> *world, player_handle *player);

        bool isNone() { return p_topology==NONE?true:false; }
        bool isServer() { return p_topology==SERVER?true:false; }
        bool isClient() { return p_topology==CLIENT?true:false; }

        void (*createWorld)(std::string name);
        world *(*getWorld)(std::string name);
        block_list *(*getBlocklist)(void);
        int getAveragePing( RakNet::RakNetGUID guid);
        RakNet::RakNetGUID getServerGUID() { return p_server_guid; }
    protected:

    private:

        RakNet::SocketDescriptor *p_socketdescriptor;
        NetworkIDManager p_networkIdManager;
        RakNet::RakPeerInterface *p_rakPeerInterface;
        RakNet::StringCompressor p_string_compressor;

        RakNet::RakNetGUID p_server_guid;

        RakNet::Packet *p_packet;
        network_topology p_topology;

        int p_port;
        int p_maxamountplayer;
        std::string p_ip;
};

#endif // NETWORK_H
