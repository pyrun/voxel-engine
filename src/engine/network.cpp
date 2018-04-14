#include "network.h"

void network_object::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const {
    allocationIdBitstream->Write(GetName());
}

void network_object::PrintStringInBitstream(RakNet::BitStream *bs)
{
    if (bs->GetNumberOfBitsUsed()==0)
        return;
    RakNet::RakString rakString;
    bs->Read(rakString);
}

void network_object::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{

    // p_variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
    // This call adds another remote system to track
    p_variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

    constructionBitstream->Write(GetName() + RakNet::RakString(" SerializeConstruction"));
}

bool network_object::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
    PrintStringInBitstream(constructionBitstream);
    return true;
}

void network_object::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{

    // p_variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
    // This call removes a remote system
    p_variableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

    destructionBitstream->Write(GetName() + RakNet::RakString(" SerializeDestruction"));
}

bool network_object::DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
    PrintStringInBitstream(destructionBitstream);
    return true;
}

void network_object::DeallocReplica(RakNet::Connection_RM3 *sourceConnection) {
    delete this;
}

void network_object::OnUserReplicaPreSerializeTick(void)
{
    /// Required by VariableDeltaSerializer::BeginIdenticalSerialize()
    p_variableDeltaSerializer.OnPreSerializeTick();
}

RM3SerializationResult network_object::Serialize(SerializeParameters *serializeParameters)	{
    VariableDeltaSerializer::SerializationContext serializationContext;
    serializeParameters->pro[0].reliability=UNRELIABLE_WITH_ACK_RECEIPT;
    serializeParameters->pro[0].sendReceipt=replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
    serializeParameters->messageTimestamp=RakNet::GetTime();

    // Begin writing all variables to be sent UNRELIABLE_WITH_ACK_RECEIPT
    p_variableDeltaSerializer.BeginUnreliableAckedSerialize(
        &serializationContext,
        serializeParameters->destinationConnection->GetRakNetGUID(),
        &serializeParameters->outputBitstream[0],
        serializeParameters->pro[0].sendReceipt
        );

    // Write each variable
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_pos.x);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_pos.y);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_pos.z);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_rot.x);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_rot.y);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_rot.z);
    p_variableDeltaSerializer.EndSerialize(&serializationContext);

    serializeParameters->pro[1].reliability=RELIABLE_ORDERED;
    p_variableDeltaSerializer.BeginIdenticalSerialize(
        &serializationContext,
        serializeParameters->whenLastSerialized==0,
        &serializeParameters->outputBitstream[1]
        );
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_scale.x);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_scale.y);
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, (float)p_scale.z);
    p_variableDeltaSerializer.EndSerialize(&serializationContext);

    serializeParameters->pro[2].reliability=RELIABLE_ORDERED;
    p_variableDeltaSerializer.BeginIdenticalSerialize(
        &serializationContext,
        serializeParameters->whenLastSerialized==0,
        &serializeParameters->outputBitstream[2]
        );
    p_variableDeltaSerializer.SerializeVariable(&serializationContext, p_name);
    p_variableDeltaSerializer.EndSerialize(&serializationContext);

    return RM3SR_SERIALIZED_ALWAYS;
}
void network_object::Deserialize(RakNet::DeserializeParameters *deserializeParameters) {

    VariableDeltaSerializer::DeserializationContext deserializationContext;

    p_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_pos.x),
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_pos.y);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_pos.z);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_rot.x),
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_rot.y);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_rot.z);
    p_variableDeltaSerializer.EndDeserialize(&deserializationContext);

    p_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[1]);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_scale.x);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_scale.y);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_scale.z);
    p_variableDeltaSerializer.EndDeserialize(&deserializationContext);

    p_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[2]);
    p_variableDeltaSerializer.DeserializeVariable(&deserializationContext, p_name);
    p_variableDeltaSerializer.EndDeserialize(&deserializationContext);

    p_model_change = true;

    setTransform( p_pos, p_rot, true);
}

void network_object::SerializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{
    serializationBitstream->Write(GetName() + RakNet::RakString(" SerializeConstructionRequestAccepted"));
}

void network_object::DeserializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *acceptingConnection) {
    PrintStringInBitstream(serializationBitstream);
}
void network_object::SerializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{
    serializationBitstream->Write(GetName() + RakNet::RakString(" SerializeConstructionRequestRejected"));
}
void network_object::DeserializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *rejectingConnection) {
    PrintStringInBitstream(serializationBitstream);
}

/** \brief network main class
 *
 *
 */
network::network( config *config, texture* image, block_list *block_list)
{
    p_rakPeerInterface = NULL;

    p_types = new object_handle();
    p_types->load( config);
    // broadcast on 255.255.255.255 at IPv4
    p_socketdescriptor.socketFamily=AF_INET;
    p_port = atoi( config->get( "port", "network", "27558").c_str() );
    p_maxamountplayer = atoi( config->get( "max_player", "network", "32").c_str() );
    p_isClient = false;
    p_isServer = false;
    p_starchip = new world( image, block_list);
    p_blocks = block_list;
}

network::~network()
{
    if( p_rakPeerInterface) {
        p_rakPeerInterface->Shutdown( 100, 0);
        RakNet::RakPeerInterface::DestroyInstance( p_rakPeerInterface);
    }

    delete p_types;
}

bool network::init_upnp()
{
    bool l_successful = true;
    struct UPNPDev * devlist = 0;
    devlist = upnpDiscover( 2000, 0, 0, 0, 0, 0);
    if (devlist)
    {
        printf("List of UPNP devices found on the network :\n");
        struct UPNPDev * device;
        for( device = devlist; device; device = device->pNext)
        {
            printf(" desc: %s\n st: %s\n\n",
                device->descURL, device->st);
        }

        char lanaddr[64];	/* my ip address on the LAN */
        struct UPNPUrls urls;
        struct IGDdatas data;
        if ( UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr))==1)
        {
            // Use same external and internal ports
            DataStructures::List<RakNetSocket2* > sockets;

            p_rakPeerInterface->GetSockets(sockets);

            std::string l_iport;
            l_iport = std::to_string( sockets[0]->GetBoundAddress().GetPort());

            // Version miniupnpc-1.6.20120410
            int r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                                        l_iport.c_str(), l_iport.c_str(), lanaddr, 0, "UDP", 0, "0");

            if(r!=UPNPCOMMAND_SUCCESS)
                printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n",
                l_iport.c_str(), l_iport.c_str(), lanaddr, r, strupnperror(r));

            char intPort[6];
            char intClient[16];

            // Version miniupnpc-1.6.20120410
            char desc[128];
            char enabled[128];
            char leaseDuration[128];
            r = UPNP_GetSpecificPortMappingEntry(urls.controlURL,
                data.first.servicetype,
                l_iport.c_str(), "UDP",
                intClient, intPort,
                desc, enabled, leaseDuration);

            if(r!=UPNPCOMMAND_SUCCESS)
            {
                printf("GetSpecificPortMappingEntry() failed with code %d (%s)\n",
                r, strupnperror(r));
            }
        } else {
            l_successful = false;
        }
    } else {
        l_successful = false;
    }
    return l_successful;
}

void network::start()
{
    // Start RakNet
    p_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
	p_rakPeerInterface->Startup( p_maxamountplayer, &p_socketdescriptor, 1);
	p_rakPeerInterface->AttachPlugin( &p_replicaManager);
	p_replicaManager.SetNetworkIDManager( &p_networkIdManager);
	p_rakPeerInterface->SetMaximumIncomingConnections( p_maxamountplayer);

	printf("network::start: GUID is %s\n", p_rakPeerInterface->GetMyGUID().ToString());

	// connect if client
	if( network_topology == CLIENT)
	{
	    p_rakPeerInterface->Connect( p_ip.c_str(), p_port, 0, 0, 0);
        if( !init_upnp())
           printf ( "network::start upnp didnt work\n");
		printf("network::start Connecting...\n");
	}

	if( isServer()) {
        p_starchip->addChunk( glm::vec3( 0, -1, 0), true);
        int l_size = 5;
        int l_end = -1;
        for( int x = -l_size; x <= l_size; x++)
            for( int y = -l_size; y <= l_size; y++)
                for( int z = l_end; z <= 0; z++)
                    p_starchip->addChunk( glm::vec3( x, z, y), true);

        ServerCreated_ServerSerialized* l_obj = new ServerCreated_ServerSerialized();
        l_obj->p_name = "box";
        l_obj->setPosition( glm::vec3( 0, 40, 5) );

        //p_replicaManager.Reference( l_obj);

        addObject( l_obj);
	}
}

void network::start_sever()
{
    network_topology = SERVER;
    p_socketdescriptor.port = p_port;
    p_isServer = true;
    start();
}

void network::start_client( std::string ip)
{
    network_topology = CLIENT;
    p_socketdescriptor.port = 0;
    p_ip = ip;
    p_isClient = true;
    start();
}

void network::addObject( ServerCreated_ServerSerialized *l_obj) {
    p_replicaManager.Reference( l_obj);
}

void network::sendBlockChange( Chunk *chunk, glm::vec3 pos, int id) {
    BitStream l_bitstream;
    RakNet::AddressOrGUID l_address;
    bool l_broadcast;
    // first change here
    if( isServer()) {
        p_starchip->SetTile( chunk, pos.x, pos.y, pos.z, id);
    }

    // now send
    l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
    l_broadcast = true;
    l_bitstream.Write((RakNet::MessageID)ID_SET_BLOCK);
    l_bitstream.Write( (float)chunk->getPos().x);
    l_bitstream.Write( (float)chunk->getPos().y);
    l_bitstream.Write( (float)chunk->getPos().z);
    l_bitstream.Write( (float)pos.x);
    l_bitstream.Write( (float)pos.y);
    l_bitstream.Write( (float)pos.z);
    l_bitstream.Write( id);
    p_rakPeerInterface->Send( &l_bitstream, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, l_address, l_broadcast);
}

void network::receiveBlockChange( BitStream *bitstream) {
    RakNet::AddressOrGUID l_address;
    Chunk *l_chunk;
    glm::vec3 l_pos;
    glm::vec3 l_pos_chunk;
    int l_id;
    bitstream->Read( l_pos_chunk.x);
    bitstream->Read( l_pos_chunk.y);
    bitstream->Read( l_pos_chunk.z);
    bitstream->Read( l_pos.x);
    bitstream->Read( l_pos.y);
    bitstream->Read( l_pos.z);
    bitstream->Read( l_id);

    l_chunk = getWorld()->getChunk( l_pos_chunk);
    if( !l_chunk)
        l_chunk = getWorld()->createChunk( l_pos_chunk.x, l_pos_chunk.y, l_pos_chunk.z);
    if( !l_chunk) {
        printf( "network::readBlockChange cant create chunk!\n");
        return;
    }

    printf( "network::readBlockChange %.2f %.2f %.2f %d\n", l_pos.x, l_pos.y, l_pos.z, l_id);

    getWorld()->SetTile( l_chunk, l_pos.x, l_pos.y, l_pos.z, l_id);

    if( isServer()) {
        // now send
        l_address = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
        bool l_broadcast = true;
        BitStream l_bitstream;
        l_bitstream.Write((RakNet::MessageID)ID_SET_BLOCK);
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
    int l_x, l_y, l_z;
    int l_start, l_end;

    bitstream->Read( l_x);
    bitstream->Read( l_y);
    bitstream->Read( l_z);
    bitstream->Read( l_start);
    bitstream->Read( l_end);

    Chunk *l_chunk = getWorld()->getChunk( glm::vec3( l_x, l_y, l_z) );
    if( !l_chunk) {
        l_chunk = getWorld()->createChunk( l_x, l_y, l_z, false, false);
    }
    if( !l_chunk) {
        printf( "network::readChunk cant create chunk!\n");
        return;
    }

    // read, if corrupt send send a get message
    if( !l_chunk->serialize( false, bitstream, l_start, l_end, p_blocks) )
        sendGetChunkData( RakNet::UNASSIGNED_SYSTEM_ADDRESS, l_chunk, l_start, l_end);

    if( l_end == CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)
        l_chunk->changed( true);
}

void network::sendChunkData( Chunk *chunk, RakNet::AddressOrGUID address, int l_start, int l_end) {
    BitStream l_bitstream;

    l_bitstream.Write((RakNet::MessageID)ID_SET_CHUNK_DATA);
    l_bitstream.Write( (int)chunk->getPos().x);
    l_bitstream.Write( (int)chunk->getPos().y);
    l_bitstream.Write( (int)chunk->getPos().z);
    l_bitstream.Write( l_start);
    l_bitstream.Write( l_end);
    chunk->serialize( true, &l_bitstream, l_start, l_end, p_blocks);
    p_rakPeerInterface->Send( &l_bitstream, IMMEDIATE_PRIORITY, RELIABLE_ORDERED , 0, address, false);
}

void network::sendChunk( Chunk *chunk, RakNet::AddressOrGUID address) {
    for( int l_width = 0; l_width < (CHUNK_SIZE*CHUNK_SIZE)/CHUNK_SIZE; l_width++) {
        int l_start = l_width*CHUNK_SIZE*CHUNK_SIZE;
        int l_end = l_start+(CHUNK_SIZE*CHUNK_SIZE);
        sendChunkData( chunk, address, l_start, l_end);
    }
}

void network::sendAllChunks( world *world, RakNet::AddressOrGUID address) {
    int i =0;
    Chunk *l_node = world->getNode();
    while( l_node != NULL)
    {
        sendChunk( l_node, address);
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

    if( l_start > 0 && l_start < l_end && l_end < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE + 1) {
        l_chunk = getWorld()->getChunk( l_pos_chunk);
        if( l_chunk != NULL) {
            sendChunkData( l_chunk, address, l_start, l_end);
            l_send = true;
        }
    }
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

bool network::process( int delta)
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
        case ID_CONNECTION_REQUEST_ACCEPTED:
            printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
            break;
        case ID_NEW_INCOMING_CONNECTION:
            printf("ID_NEW_INCOMING_CONNECTION from %s\n", p_packet->systemAddress.ToString());
            // send all chunks data
            sendAllChunks( getWorld(), p_packet->guid);
            break;
        case ID_DISCONNECTION_NOTIFICATION:
            printf("ID_DISCONNECTION_NOTIFICATION\n");
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
                uint32_t msgNumber;
                memcpy(&msgNumber, p_packet->data+1, 4);

                DataStructures::List<Replica3*> replicaListOut;
                p_replicaManager.GetReplicasCreatedByMe(replicaListOut);
                unsigned int idx;
                for (idx=0; idx < replicaListOut.Size(); idx++)
                {
                    ((network_object*)replicaListOut[idx])->NotifyReplicaOfMessageDeliveryStatus( p_packet->guid,msgNumber, p_packet->data[0]==ID_SND_RECEIPT_ACKED);
                }
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
        }
    }

    return l_quit;
}

void network::drawEntitys( Shader *shader, glm::mat4 view, glm::mat4 projection)
{
    unsigned int l_id;
    for (l_id=0; l_id < p_replicaManager.GetReplicaCount(); l_id++) {
        network_object *l_obj = ((network_object*)(p_replicaManager.GetReplicaAtIndex( l_id)));

        // create type if dont exit
        if( l_obj->getType() == NULL) {
            object_type *l_type = p_types->get( l_obj->p_name.C_String());
            if( l_type) // found -> set
                l_obj->setType( l_type);
        } else {
            // if no physic body -> crate one
            if( l_obj->getBody() == NULL) {
                b3BodyDef bdef;
                // set up def.
                bdef.type = b3BodyType::e_dynamicBody;
                b3Body* body = getWorld()->getPhysicWorld()->CreateBody(bdef);

                /// todo
                if( isServer())
                    l_obj->setBody( body);
            }
        }

        // set physic transform to model transform
        l_obj->process();

        // draw
        l_obj->draw( shader, view, projection);
    }
}

void network::create_object() {

}
