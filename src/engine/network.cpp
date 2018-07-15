#include "network.h"

network::network( config *config)
{
    p_rakPeerInterface = NULL;

    p_port = atoi( config->get( "port", "network", "27558").c_str() );
    p_maxamountplayer = atoi( config->get( "max_player", "network", "32").c_str() );

    // broadcast on 255.255.255.255 at IPv4
    p_socketdescriptor = new RakNet::SocketDescriptor();
    //p_socketdescriptor->socketFamily=AF_UNSPEC;

    p_topology = NONE;

    p_average_ping = 0;
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

void network::sendSpawnPoint( std::string name, RakNet::AddressOrGUID address) {
    glm::vec3 l_position;
    BitStream l_bitstream;
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
    l_broadcast = false;
    l_bitstream.Write((RakNet::MessageID)ID_SET_WORLD_SPAWN_POINT);
    p_string_compressor.EncodeString( name.c_str(), 16, &l_bitstream);
    l_bitstream.Write( (float)l_position.x);
    l_bitstream.Write( (float)l_position.y);
    l_bitstream.Write( (float)l_position.z);
    p_rakPeerInterface->Send( &l_bitstream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, address, l_broadcast);
}

void network::receiveSpawnPoint( BitStream *bitstream) {
    char l_name[NETWORK_WOLRD_NAME_LENGTH];
    world *l_world;
    glm::vec3 l_position;

    p_string_compressor.DecodeString( l_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
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

void network::sendAllObjects( world *targetworld, RakNet::AddressOrGUID address) {
    std::vector<object *> l_objects = targetworld->getObjects();

    for( object *l_object:l_objects) {
        sendCreateObject( address, false, targetworld, l_object->getType()->getName(), l_object->getPosition(), l_object->getId());
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
    char l_world_name[NETWORK_WOLRD_NAME_LENGTH];
    char l_type_name[32];
    world *l_world;
    Chunk *l_chunk;
    unsigned int l_id;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    p_string_compressor.DecodeString( l_type_name, 32, bitstream);
    bitstream->Read( l_position.x);
    bitstream->Read( l_position.y);
    bitstream->Read( l_position.z);
    bitstream->Read( l_id);

    l_world = getWorld( l_world_name);
    if( !l_world )
        return;

    l_world->createObject( l_type_name, l_position, l_id, false);

    printf( "network::receiveCreateObject %s %.1f %.1f %.1f\n", l_world_name, l_position.x, l_position.y, l_position.z);
}

void network::sendMatrixObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, object *object) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_MATRIX_OBJECT);
    p_string_compressor.EncodeString( targetworld->getName().c_str(), 16, &l_bitstream);
    l_bitstream.Write( object->getId());
    l_bitstream.Write( object->getPosition().x); // Pos
    l_bitstream.Write( object->getPosition().y);
    l_bitstream.Write( object->getPosition().z);
    l_bitstream.Write( object->getRotation().x); // Rotation
    l_bitstream.Write( object->getRotation().y);
    l_bitstream.Write( object->getRotation().z);
    l_bitstream.Write( object->getVerlocity().x); // Verlocity
    l_bitstream.Write( object->getVerlocity().y);
    l_bitstream.Write( object->getVerlocity().z);
    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, UNRELIABLE, 0, address, broadcast);
}

void network::receiveMatrixObject( BitStream *bitstream) {
    glm::vec3 l_position;
    glm::vec3 l_rotation;
    glm::vec3 l_verlocity;
    char l_world_name[NETWORK_WOLRD_NAME_LENGTH];
    world *l_world;
    unsigned int l_id;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    bitstream->Read( l_id);
    bitstream->Read( l_position.x);
    bitstream->Read( l_position.y);
    bitstream->Read( l_position.z);
    bitstream->Read( l_rotation.x);
    bitstream->Read( l_rotation.y);
    bitstream->Read( l_rotation.z);
    bitstream->Read( l_verlocity.x);
    bitstream->Read( l_verlocity.y);
    bitstream->Read( l_verlocity.z);


    l_world = getWorld( l_world_name);
    if( !l_world )
        return;

    object *l_object = l_world->getObject( l_id);
    if( l_object) {
        l_object->setPosition( l_position);
        l_object->setRotation( l_rotation);
        l_object->setVelocity( l_verlocity);
    }
}

void network::sendDeleteObject( RakNet::AddressOrGUID address, bool broadcast, world *targetworld, unsigned int id) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_DELETE_OBJECT);
    p_string_compressor.EncodeString( targetworld->getName().c_str(), 16, &l_bitstream);
    l_bitstream.Write( id);
    p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED , 0, address, broadcast);
}

void network::receiveDeleteObject( BitStream *bitstream) {
    char l_world_name[NETWORK_WOLRD_NAME_LENGTH];
    world *l_world;
    unsigned int l_id;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    bitstream->Read( l_id);

    l_world = getWorld( l_world_name);
    if( !l_world )
        return;

    l_world->deleteObject( l_id);

    printf( "network::receiveDeleteObject %d\n", l_id);
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
    char l_world_name[16];
    world *l_world;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    bitstream->Read( l_pos_chunk.x);
    bitstream->Read( l_pos_chunk.y);
    bitstream->Read( l_pos_chunk.z);
    bitstream->Read( l_pos.x);
    bitstream->Read( l_pos.y);
    bitstream->Read( l_pos.z);
    bitstream->Read( l_id);

    // set world spawn point
    l_world = getWorld( l_world_name);
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
    char l_world_name[NETWORK_WOLRD_NAME_LENGTH];
    world *l_world;
    Chunk *l_chunk;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    bitstream->Read( l_chunk_position.x);
    bitstream->Read( l_chunk_position.y);
    bitstream->Read( l_chunk_position.z);
    bitstream->Read( l_start);
    bitstream->Read( l_end);

    // get world
    l_world = getWorld( l_world_name);
    if( !l_world ) {
        createWorld( l_world_name);
        l_world = getWorld( l_world_name);
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
    p_string_compressor.EncodeString( name.c_str(), NETWORK_WOLRD_NAME_LENGTH, &l_bitstream);
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

void network::sendLoginPlayer( player *player, RakNet::AddressOrGUID address) {
    BitStream l_bitstream;

    // send
    l_bitstream.Write((RakNet::MessageID)ID_PLAYER_LOGIN);

    // data
    p_string_compressor.EncodeString( player->getName().c_str(), 16, &l_bitstream);

    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void network::receiveLoginPlayer( BitStream *bitstream, RakNet::RakNetGUID guid, player_handle *players, world *world) {
    unsigned int l_id;
    char l_name_player[16];

    // data
    p_string_compressor.DecodeString( l_name_player, 16, bitstream);

    // create player
    player *l_player = players->createPlayer( world);
    l_player->setGUID( guid);
    l_player->setName( l_name_player);
    l_player->createObject();

    // send all data
    /*for( auto *l_world:*worlds) {
        sendSpawnPoint( l_world->getName(), p_packet->guid); // set spawn point
        sendAllChunks( l_world, p_packet->guid); // send chunks
        sendWorldFinish( l_world->getName(), p_packet->guid); // send we are finish
        sendAllObjects( l_world, p_packet->guid); // send all objects
    }*/

    // send only world who needed
    if( world) {
        sendSpawnPoint( world->getName(), guid); // set spawn point
        sendAllChunks( world, guid); // send chunks
        sendWorldFinish( world->getName(), guid); // send we are finish
        sendAllObjects( world, guid); // send all objects
        sendBindPlayer( l_player, world, guid);
    }

    printf( "network::receiveLoginPlayer player \"%s\" join the game(#%d)\n", l_name_player, l_id);
}

void network::sendBindPlayer( player *player, world *targetworld, RakNet::AddressOrGUID address) {
    BitStream l_bitstream;
    bool l_broadcast;

    // send
    l_broadcast = false;
    l_bitstream.Write((RakNet::MessageID)ID_PLAYER_BIND);

    // data
    l_bitstream.Write( player->getId() );
    p_string_compressor.EncodeString( player->getName().c_str(), 16, &l_bitstream);
    p_string_compressor.EncodeString( targetworld->getName().c_str(), 16, &l_bitstream);
    l_bitstream.Write( true );

    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, RELIABLE_ORDERED, 0, address, l_broadcast);
}

void network::receiveBindPlayer( BitStream *bitstream, player_handle *players) {
    unsigned int l_id;
    char l_name_player[16];
    char l_name_world[NETWORK_WOLRD_NAME_LENGTH];
    bool l_player_bool = false;
    world *l_world = NULL;

    // data
    bitstream->Read( l_id);
    p_string_compressor.DecodeString( l_name_player, 16, bitstream);
    p_string_compressor.DecodeString( l_name_world, NETWORK_WOLRD_NAME_LENGTH, bitstream);
    bitstream->Read( l_player_bool);

    // handle data
    player *l_player = players->getPlayerByName( l_name_player); //players->getPlayer()[0];

    l_world = getWorld( l_name_world);
    if( !l_world ) {
        createWorld( l_name_world);
        l_world = getWorld( l_name_world);
        // still missing? -> abort
        if( !l_world)
            return;
    }

    if( !l_player)
        l_player = players->createPlayer( l_world);
    l_player->setName( l_name_player);
    l_player->setId( l_id);
    l_player->setWorld( l_world);

    printf( "receiveBindPlayer %s %d\n", l_name_player, l_id);
}

void network::sendWorldFinish( std::string name, RakNet::AddressOrGUID address) {
    BitStream l_bitstream;
    bool l_broadcast;

    // send
    l_broadcast = false;
    l_bitstream.Write((RakNet::MessageID)ID_WORLD_TRANSFER_FINISH);
    p_string_compressor.EncodeString( name.c_str(), NETWORK_WOLRD_NAME_LENGTH, &l_bitstream);
    p_rakPeerInterface->Send( &l_bitstream, LOW_PRIORITY, RELIABLE_ORDERED, 0, address, l_broadcast);
}

void network::receiveWorldFinish( BitStream *bitstream) {
    char l_world_name[NETWORK_WOLRD_NAME_LENGTH];
    world *l_world;

    p_string_compressor.DecodeString( l_world_name, NETWORK_WOLRD_NAME_LENGTH, bitstream);

    // get world
    l_world = getWorld( l_world_name);
    if( !l_world ) {
        createWorld( l_world_name);
        l_world = getWorld( l_world_name);
        // still missing? -> abort
        if( !l_world)
            return;
    }
    // now they can recalculation the light
    l_world->caluculationLight();
}

void network::connection( RakNet::RakNetGUID guid, player_handle *players, std::vector<world*> *worlds) {
    printf("network::connection from \"%s\"\n", p_packet->systemAddress.ToString());
}

void network::disconnection( RakNet::RakNetGUID guid, player_handle *players) {
    player *l_player = players->getPlayerByGUID( guid);
    if( l_player) {
        object *l_object = l_player->getObject();
        if( l_object) {
            world *l_world = l_player->getWorld();
            l_world->deleteObject( l_object->getId());
        }
        printf( "network::disconnection \"%s\" left the game\n", l_player->getName().c_str() );
        players->deletePlayerByGUID( guid);
    }
}

bool network::process( std::vector<world*> *worlds, player_handle *players)
{
    bool l_quit = false;

    if( p_rakPeerInterface == NULL )
        return l_quit;

    for ( p_packet = p_rakPeerInterface->Receive(); p_packet; p_rakPeerInterface->DeallocatePacket(p_packet), p_packet = p_rakPeerInterface->Receive())
    {
        // todo listing ...
        p_average_ping = getAveragePing( p_packet->guid);

        // packets
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
            p_server_guid = p_packet->guid;
            if( players->getPlayer().size() > 0)
                sendLoginPlayer( players->getPlayer()[0], p_server_guid);
            break;
        case ID_NEW_INCOMING_CONNECTION:
            {
                connection( p_packet->guid, players, worlds);
            }
            break;
        case ID_CONNECTION_LOST:
            {
                printf("network::process connection lost\n");
                disconnection( p_packet->guid, players);
            }
        break;
        case ID_DISCONNECTION_NOTIFICATION:
            {
                printf("network::process disconnection\n");
                disconnection( p_packet->guid, players);
            }
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
                // todo
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
        case ID_MATRIX_OBJECT:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveMatrixObject( &l_bitsteam);
        }
        break;
        case ID_DELETE_OBJECT:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveDeleteObject( &l_bitsteam);
        }
        break;
        case ID_PLAYER_LOGIN:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            if( worlds->at(0))
                receiveLoginPlayer( &l_bitsteam, p_packet->guid, players, worlds->at(0));
        }
        break;
        case ID_PLAYER_BIND:
        {
            RakNet::BitStream l_bitsteam( p_packet->data, p_packet->length, false);
            l_bitsteam.IgnoreBytes(sizeof(RakNet::MessageID));
            receiveBindPlayer( &l_bitsteam, players );
        }
        break;
        }
    }

    return l_quit;
}

int network::getAveragePing( RakNet::RakNetGUID guid) {
    if( p_rakPeerInterface)
        return p_rakPeerInterface->GetAveragePing( guid);
    return -1;
}
