#include "network.h"

/** \brief network main class
 *
 *
 */
network::network( config *config)
{
    p_rakPeerInterface = NULL;

    p_port = atoi( config->get( "port", "network", "27558").c_str() );
    p_maxamountplayer = atoi( config->get( "max_player", "network", "32").c_str() );

    // broadcast on 255.255.255.255 at IPv4
    p_socketdescriptor = new RakNet::SocketDescriptor();
    //p_socketdescriptor->socketFamily=AF_UNSPEC;

    p_topology = NONE;
}

network::~network()
{
    if( p_rakPeerInterface) {
        p_rakPeerInterface->Shutdown( 100, 0);
        RakNet::RakPeerInterface::DestroyInstance( p_rakPeerInterface);
    }
}

void network::start()
{
    // Start RakNet
    p_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
	p_rakPeerInterface->Startup( p_maxamountplayer, p_socketdescriptor, 1);
	p_rakPeerInterface->SetMaximumIncomingConnections( p_maxamountplayer);

	// connect if client
	if( p_topology == CLIENT)
	{
	    printf("network::start Connecting...\n");
	    p_rakPeerInterface->Connect( p_ip.c_str(), p_port, 0, 0, 0);
	}

	printf("network::start GUID is \"%s\" IP \"%s\"\n", p_rakPeerInterface->GetMyGUID().ToString(), p_rakPeerInterface->GetMyBoundAddress().ToString());
}

void network::start_sever()
{
    p_topology = SERVER;
    p_socketdescriptor->port = p_port;
    start();
}

void network::start_client( std::string ip)
{
    p_topology = CLIENT;
    p_socketdescriptor->port = 0;
    p_ip = ip;
    start();
}

void network::sendSpawnPoint( std::string name) {
    glm::vec3 l_position;
    BitStream l_bitstream;
    RakNet::AddressOrGUID l_address;
    bool l_broadcast;

    // get world spawn point
    world *l_world;
    l_world = getWorld( name);
    if( !l_world ) {
        createWorld( name);
        l_world = getWorld( name);
        // still missing? -> abort
        if( !l_world)
            return;
    }
    l_position = l_world->getSpawnPoint();

    // send
    l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    l_broadcast = true;
    l_bitstream.Write((RakNet::MessageID)ID_SET_WORLD_SPAWN_POINT);
    p_string_compressor.EncodeString( name.c_str(), 16, &l_bitstream);
    l_bitstream.Write( (float)l_position.x);
    l_bitstream.Write( (float)l_position.y);
    l_bitstream.Write( (float)l_position.z);
    p_rakPeerInterface->Send( &l_bitstream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
}

void network::receiveSpawnPoint( BitStream *bitstream) {
    char l_name[16];
    world *l_world;
    glm::vec3 l_position;

    p_string_compressor.DecodeString( l_name, 16, bitstream);
    bitstream->Read( l_position.x);
    bitstream->Read( l_position.y);
    bitstream->Read( l_position.z);

    // set world spawn point
    l_world = getWorld( l_name);
    if( !l_world ) {
        createWorld( l_name);
        l_world = getWorld( l_name);
        // still missing? -> abort
        if( !l_world)
            return;
    }
    //printf("receiveSpawnPoint set\n");
    l_world->setSpawnPoint( l_position);
}

void network::sendAllObjects( world *targetworld) {
    std::vector<object *> l_objects = targetworld->getObjects();

    for( object *l_object:l_objects) {
        sendCreateObject( RakNet::UNASSIGNED_SYSTEM_ADDRESS, true, targetworld, l_object->getType()->getName(), l_object->getPosition(), l_object->getId());
    }
}

void network::sendCreateObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, std::string type, glm::vec3 position, unsigned int id) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_CREATE_OBJECT);
    p_string_compressor.EncodeString( targetworld->getName().c_str(), 16, &l_bitstream);
    p_string_compressor.EncodeString( type.c_str(), 32, &l_bitstream);
    l_bitstream.Write( position.x);
    l_bitstream.Write( position.y);
    l_bitstream.Write( position.z);
    l_bitstream.Write( id);
    p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED , 0, address, broadcast);
}

void network::receiveCreateObject( BitStream *bitstream) {
    glm::vec3 l_position;
    int l_start, l_end;
    char l_name[16];
    char l_type_name[32];
    world *l_world;
    Chunk *l_chunk;
    unsigned int l_id;

    p_string_compressor.DecodeString( l_name, 16, bitstream);
    p_string_compressor.DecodeString( l_type_name, 32, bitstream);
    bitstream->Read( l_position.x);
    bitstream->Read( l_position.y);
    bitstream->Read( l_position.z);
    bitstream->Read( l_id);

    l_world = getWorld( l_name);
    if( !l_world )
        return;

    l_world->createObject( l_type_name, l_position, l_id);

    printf( "network::receiveCreateObject %s %.1f %.1f %.1f\n", l_name, l_position.x, l_position.y, l_position.z);
}

void network::sendBlockChange( world *world, Chunk *chunk, glm::ivec3 position, unsigned int id) {
    BitStream l_bitstream;
    RakNet::AddressOrGUID l_address;
    bool l_broadcast;
    // first change here
    if( isServer()) {
        //p_starchip->SetTile( chunk, pos.x, pos.y, pos.z, id);
    }

    // now send
    l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    l_broadcast = true;
    l_bitstream.Write((RakNet::MessageID)ID_SET_BLOCK);
    p_string_compressor.EncodeString( world->getName().c_str(), 16, &l_bitstream);
    l_bitstream.Write( (float)chunk->getPos().x);
    l_bitstream.Write( (float)chunk->getPos().y);
    l_bitstream.Write( (float)chunk->getPos().z);
    l_bitstream.Write( (float)position.x);
    l_bitstream.Write( (float)position.y);
    l_bitstream.Write( (float)position.z);
    l_bitstream.Write( id);
    p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
}

void network::receiveBlockChange( BitStream *bitstream) {
    RakNet::AddressOrGUID l_address;
    Chunk *l_chunk;
    glm::vec3 l_pos;
    glm::vec3 l_pos_chunk;
    int l_id;
    char l_name[16];
    world *l_world;

    p_string_compressor.DecodeString( l_name, 16, bitstream);
    bitstream->Read( l_pos_chunk.x);
    bitstream->Read( l_pos_chunk.y);
    bitstream->Read( l_pos_chunk.z);
    bitstream->Read( l_pos.x);
    bitstream->Read( l_pos.y);
    bitstream->Read( l_pos.z);
    bitstream->Read( l_id);

    // set world spawn point
    l_world = getWorld( l_name);
    if( !l_world )
        return;

    l_chunk = l_world->getChunk( l_pos_chunk);
    if( !l_chunk) {
        printf( "network::readBlockChange cant create chunk!\n");
        return;
    }

    printf( "network::readBlockChange %.2f %.2f %.2f %d\n", l_pos.x, l_pos.y, l_pos.z, l_id);

    l_world->changeBlock( l_chunk, l_pos, l_id, false);

    if( isServer()) {
        // now send
        l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
        bool l_broadcast = true;
        BitStream l_bitstream;
        l_bitstream.Write((RakNet::MessageID)ID_SET_BLOCK);
        p_string_compressor.EncodeString( l_world->getName().c_str(), 16, &l_bitstream);
        l_bitstream.Write( (float)l_chunk->getPos().x);
        l_bitstream.Write( (float)l_chunk->getPos().y);
        l_bitstream.Write( (float)l_chunk->getPos().z);
        l_bitstream.Write( (float)l_pos.x);
        l_bitstream.Write( (float)l_pos.y);
        l_bitstream.Write( (float)l_pos.z);
        l_bitstream.Write( l_id);
        p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
    }
}

void network::receiveChunk( BitStream *bitstream) {
    glm::ivec3 l_chunk_position;
    int l_start, l_end;
    char l_name[16];
    world *l_world;
    Chunk *l_chunk;

    p_string_compressor.DecodeString( l_name, 16, bitstream);
    bitstream->Read( l_chunk_position.x);
    bitstream->Read( l_chunk_position.y);
    bitstream->Read( l_chunk_position.z);
    bitstream->Read( l_start);
    bitstream->Read( l_end);

    // get world
    l_world = getWorld( l_name);
    if( !l_world ) {
        createWorld( l_name);
        l_world = getWorld( l_name);
        // still missing? -> abort
        if( !l_world)
            return;
    }

    l_chunk = l_world->getChunk( l_chunk_position);
    if( !l_chunk) {
        l_chunk = l_world->createChunk( l_chunk_position);
    }

    // read, if corrupt send send a get message
    if( !l_chunk->serialize( false, bitstream, l_start, l_end, getBlocklist()) )
        sendGetChunkData( RakNet::UNASSIGNED_SYSTEM_ADDRESS, l_chunk, l_start, l_end);

    if( l_end == CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE) {
        l_chunk->changed( true);
    }
}

void network::sendChunkData( std::string name, Chunk *chunk, RakNet::AddressOrGUID address, int l_start, int l_end) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_SET_CHUNK_DATA);
    p_string_compressor.EncodeString( name.c_str(), 16, &l_bitstream);
    l_bitstream.Write( (int)chunk->getPos().x);
    l_bitstream.Write( (int)chunk->getPos().y);
    l_bitstream.Write( (int)chunk->getPos().z);
    l_bitstream.Write( l_start);
    l_bitstream.Write( l_end);
    chunk->serialize( true, &l_bitstream, l_start, l_end, getBlocklist());
    p_rakPeerInterface->Send( &l_bitstream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED , 0, address, false);
}

void network::sendChunk( std::string name, Chunk *chunk, RakNet::AddressOrGUID address) {
    for( int l_width = 0; l_width < (CHUNK_SIZE*CHUNK_SIZE)/CHUNK_SIZE; l_width++) {
        int l_start = l_width*CHUNK_SIZE*CHUNK_SIZE;
        int l_end = l_start+(CHUNK_SIZE*CHUNK_SIZE);
        sendChunkData( name, chunk, address, l_start, l_end);
    }
}

void network::sendAllChunks( world *world, RakNet::AddressOrGUID address) {
    int i =0;
    Chunk *l_node = world->getNode();
    while( l_node != NULL)
    {
        sendChunk( world->getName(), l_node, address);
        l_node = l_node->next;
        i++;
    }
}

void network::receiveGetChunkData( BitStream *bitstream, RakNet::AddressOrGUID address) {
    Chunk *l_chunk;
    int l_start;
    int l_end;
    glm::vec3 l_pos_chunk;
    bool l_send = false;
    bitstream->Read( l_pos_chunk.x);
    bitstream->Read( l_pos_chunk.y);
    bitstream->Read( l_pos_chunk.z);
    bitstream->Read( l_start);
    bitstream->Read( l_end);

/*    if( l_start > 0 && l_start < l_end && l_end < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE + 1) {
        l_chunk = getWorld()->getChunk( l_pos_chunk);
        if( l_chunk != NULL) {
            sendChunkData( l_chunk, address, l_start, l_end);
            l_send = true;
        }
    }*/
    printf( "network::receiveGetChunkData chunk(x%dy%dz%d) data(%d to %d) send = %d\n", (int)l_pos_chunk.x, (int)l_pos_chunk.y, (int)l_pos_chunk.z, l_start, l_end, l_send);
}

void network::sendGetChunkData( RakNet::AddressOrGUID address, Chunk *chunk, int start, int end) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_GET_CHUNK_DATA);
    l_bitstream.Write( chunk->getPos().x);
    l_bitstream.Write( chunk->getPos().y);
    l_bitstream.Write( chunk->getPos().z);
    l_bitstream.Write( start);
    l_bitstream.Write( end);
    p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED , 0, address, true);
}

void network::sendBindPlayer( player *player) {
    BitStream l_bitstream;
    RakNet::AddressOrGUID l_address;
    bool l_broadcast;

    // send
    l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    l_broadcast = true;
    l_bitstream.Write((RakNet::MessageID)ID_PLAYER_BIND);

    // data
    l_bitstream.Write( player->getId() );
    p_string_compressor.EncodeString( player->getName().c_str(), 16, &l_bitstream);
    l_bitstream.Write( true );

    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
}

void network::receiveBindPlayer( BitStream *bitstream, player_handle *players, world *world) {
    unsigned int l_id;
    char l_name[16];
    bool l_player_bool = false;

    // data
    bitstream->Read( l_id);
    p_string_compressor.DecodeString( l_name, 16, bitstream);
    bitstream->Read( l_player_bool);

    // handle data
    player *l_player = players->getPlayer()[0];

    if( !l_player)
        l_player = players->createPlayer( world);
    l_player->setName( l_name);
    l_player->setId( l_id);

    printf( "receiveBindPlayer %s %d\n", l_name, l_id);
}

void network::sendWorldFinish( std::string name) {
    BitStream l_bitstream;
    RakNet::AddressOrGUID l_address;
    bool l_broadcast;

    // send
    l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    l_broadcast = true;
    l_bitstream.Write((RakNet::MessageID)ID_WORLD_TRANSFER_FINISH);
    p_string_compressor.EncodeString( name.c_str(), 16, &l_bitstream);
    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
}

void network::receiveWorldFinish( BitStream *bitstream) {
    char l_name[16];
    world *l_world;

    p_string_compressor.DecodeString( l_name, 16, bitstream);

    // get world
    l_world = getWorld( l_name);
    if( !l_world ) {
        createWorld( l_name);
        l_world = getWorld( l_name);
        // still missing? -> abort
        if( !l_world)
            return;
    }
    // now they can recalculation the light
    l_world->caluculationLight();
}

bool network::process( std::vector<world*> *worlds, player_handle *players)
{
    bool l_quit = false;

    if( p_rakPeerInterface == NULL )
        return l_quit;

    for ( p_packet = p_rakPeerInterface->Receive(); p_packet; p_rakPeerInterface->DeallocatePacket(p_packet), p_packet = p_rakPeerInterface->Receive())
    {
        switch ( p_packet->data[0])
        {
        case ID_CONNECTION_ATTEMPT_FAILED:
            printf("ID_CONNECTION_ATTEMPT_FAILED\n");
            l_quit=true;
            break;
        case ID_NO_FREE_INCOMING_CONNECTIONS:
            printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
            l_quit=true;
            break;
        case ID_CONNECTION_REQUEST_ACCEPTED: // client
            printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
            //sendPlayer( );
            break;
        case ID_NEW_INCOMING_CONNECTION:
            {
                printf("ID_NEW_INCOMING_CONNECTION from %s\n", p_packet->systemAddress.ToString());
                // create player
                player *l_player = players->createPlayer( worlds->at(0));
                l_player->setGUID( p_packet->guid);
                l_player->createObject();

                // send all chunks data
                for( auto *l_world:*worlds) {
                    sendSpawnPoint( l_world->getName()); // set spawn point
                    sendAllChunks( l_world, p_packet->guid); // send chunks
                    sendWorldFinish( l_world->getName()); // send we are finish
                    sendAllObjects( l_world); // send all objects
                }
                sendBindPlayer( l_player);
            }
            break;
        case ID_DISCONNECTION_NOTIFICATION:
            {
                printf("ID_DISCONNECTION_NOTIFICATION\n");
                player *l_player = players->getPlayerGUID( p_packet->guid);
                if( l_player) {
                    object *l_object = l_player->getObject();
                    if( l_object) {
                        world *l_world = l_player->getWorld();
                        l_world->deleteObject( l_object->getId());
                    }
                }
            }
            break;
        case ID_CONNECTION_LOST:
            printf("ID_CONNECTION_LOST\n");
            break;
        case ID_ADVERTISE_SYSTEM:
            // The first conditional is needed because ID_ADVERTISE_SYSTEM may be from a system we are connected to, but replying on a different address.
            // The second conditional is because AdvertiseSystem also sends to the loopback
            if (p_rakPeerInterface->GetSystemAddressFromGuid(p_packet->guid)==RakNet::UNASSIGNED_SYSTEM_ADDRESS &&
                p_rakPeerInterface->GetMyGUID()!=p_packet->guid)
            {
                printf("Connecting to %s\n", p_packet->systemAddress.ToString(true));
                p_rakPeerInterface->Connect( p_packet->systemAddress.ToString(false), p_packet->systemAddress.GetPort(),0,0);
            }
            break;
        case ID_SND_RECEIPT_LOSS:
        case ID_SND_RECEIPT_ACKED:
            {

            }
        break;
        case ID_SET_CHUNK_DATA:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveChunk( &l_bitsteam);
        }
        break;
        case ID_SET_BLOCK:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveBlockChange( &l_bitsteam);
        }
        break;
        case ID_GET_CHUNK_DATA:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveGetChunkData( &l_bitsteam, p_packet->systemAddress);
        }
        break;
        case ID_WORLD_TRANSFER_FINISH:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveWorldFinish( &l_bitsteam);
        }
        break;
        case ID_SET_WORLD_SPAWN_POINT:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveSpawnPoint( &l_bitsteam);
        }
        break;
        case ID_CREATE_OBJECT:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveCreateObject( &l_bitsteam);
        }
        break;
        case ID_PLAYER_BIND:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            if( worlds->size() > 0)
                receiveBindPlayer( &l_bitsteam, players, worlds->at(0) );
        }
        break;
        }
    }

    return l_quit;
}
