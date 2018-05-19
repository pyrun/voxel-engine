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
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "world.h"
#include "network_ids.h"
#include "object.h"

using namespace RakNet;

enum
{
	CLIENT,
	SERVER
} network_topology;

class network
{
    public:
        network( config *config);
        virtual ~network();

        void start();
        void start_sever();
        void start_client( std::string ip = "127.0.0.1");

        void sendBlockChange( Chunk *chunk, glm::vec3 pos, int id);
        void receiveBlockChange( BitStream *bitstream);

        void receiveChunk( BitStream *bitstream);
        void sendChunkData( Chunk *chunk, RakNet::AddressOrGUID address, int l_start, int l_end);
        void sendChunk( Chunk *chunk, RakNet::AddressOrGUID address);
        void sendAllChunks( world *world,RakNet::AddressOrGUID address);
        void receiveGetChunkData( BitStream *bitstream, RakNet::AddressOrGUID address);
        void sendGetChunkData( RakNet::AddressOrGUID address, Chunk *chunk, int start, int end);

        bool process( std::vector<world *> *world);

        bool isServer() { return p_isServer; }
        bool isClient() { return p_isClient; }
    protected:

    private:

        RakNet::SocketDescriptor p_socketdescriptor;
        NetworkIDManager p_networkIdManager;
        RakNet::RakPeerInterface *p_rakPeerInterface;

        RakNet::Packet *p_packet;
        bool p_isClient;
        bool p_isServer;

        int p_port;
        int p_maxamountplayer;
        std::string p_ip;
};

#endif // NETWORK_H
