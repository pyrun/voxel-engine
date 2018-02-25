#include "network.h"

network_object::network_object()
{
    var1Unreliable=0;
    var2Unreliable=0;
    var3Reliable=0;
    var4Reliable=0;
}

network_object::~network_object()
{

}

void network_object::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const {
    allocationIdBitstream->Write(GetName());
}

void network_object::PrintStringInBitstream(RakNet::BitStream *bs)
{
    if (bs->GetNumberOfBitsUsed()==0)
        return;
    RakNet::RakString rakString;
    bs->Read(rakString);
    printf("Receive: %s\n", rakString.C_String());
}

void network_object::SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{

    // variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
    // This call adds another remote system to track
    variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

    constructionBitstream->Write(GetName() + RakNet::RakString(" SerializeConstruction"));
}

bool network_object::DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection) {
    PrintStringInBitstream(constructionBitstream);
    return true;
}

void network_object::SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection)	{

    // variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
    // This call removes a remote system
    variableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

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
    variableDeltaSerializer.OnPreSerializeTick();
}

RM3SerializationResult network_object::Serialize(SerializeParameters *serializeParameters)	{

    VariableDeltaSerializer::SerializationContext serializationContext;

    // Put all variables to be sent unreliably on the same channel, then specify the send type for that channel
    serializeParameters->pro[0].reliability=UNRELIABLE_WITH_ACK_RECEIPT;
    // Sending unreliably with an ack receipt requires the receipt number, and that you inform the system of ID_SND_RECEIPT_ACKED and ID_SND_RECEIPT_LOSS
    serializeParameters->pro[0].sendReceipt=replicaManager->GetRakPeerInterface()->IncrementNextSendReceipt();
    serializeParameters->messageTimestamp=RakNet::GetTime();

    // Begin writing all variables to be sent UNRELIABLE_WITH_ACK_RECEIPT
    variableDeltaSerializer.BeginUnreliableAckedSerialize(
        &serializationContext,
        serializeParameters->destinationConnection->GetRakNetGUID(),
        &serializeParameters->outputBitstream[0],
        serializeParameters->pro[0].sendReceipt
        );
    // Write each variable
    variableDeltaSerializer.SerializeVariable(&serializationContext, var1Unreliable);
    // Write each variable
    variableDeltaSerializer.SerializeVariable(&serializationContext, var2Unreliable);
    // Tell the system this is the last variable to be written
    variableDeltaSerializer.EndSerialize(&serializationContext);

    // All variables to be sent using a different mode go on different channels
    serializeParameters->pro[1].reliability=RELIABLE_ORDERED;

    // Same as above, all variables to be sent with a particular reliability are sent in a batch
    // We use BeginIdenticalSerialize instead of BeginSerialize because the reliable variables have the same values sent to all systems. This is memory-saving optimization
    variableDeltaSerializer.BeginIdenticalSerialize(
        &serializationContext,
        serializeParameters->whenLastSerialized==0,
        &serializeParameters->outputBitstream[1]
        );
    variableDeltaSerializer.SerializeVariable(&serializationContext, var3Reliable);
    variableDeltaSerializer.SerializeVariable(&serializationContext, var4Reliable);
    variableDeltaSerializer.EndSerialize(&serializationContext);

    // This return type makes is to ReplicaManager3 itself does not do a memory compare. we entirely control serialization ourselves here.
    // Use RM3SR_SERIALIZED_ALWAYS instead of RM3SR_SERIALIZED_ALWAYS_IDENTICALLY to support sending different data to different system, which is needed when using unreliable and dirty variable resends
    return RM3SR_SERIALIZED_ALWAYS;
}
void network_object::Deserialize(RakNet::DeserializeParameters *deserializeParameters) {

    VariableDeltaSerializer::DeserializationContext deserializationContext;

    // Deserialization is written similar to serialization
    // Note that the Serialize() call above uses two different reliability types. This results in two separate Send calls
    // So Deserialize is potentially called twice from a single Serialize
    variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
    if (variableDeltaSerializer.DeserializeVariable(&deserializationContext, var1Unreliable))
        printf("var1Unreliable changed to %i\n", var1Unreliable);
    if (variableDeltaSerializer.DeserializeVariable(&deserializationContext, var2Unreliable))
        printf("var2Unreliable changed to %i\n", var2Unreliable);
    variableDeltaSerializer.EndDeserialize(&deserializationContext);

    variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[1]);
    if (variableDeltaSerializer.DeserializeVariable(&deserializationContext, var3Reliable))
        printf("var3Reliable changed to %i\n", var3Reliable);
    if (variableDeltaSerializer.DeserializeVariable(&deserializationContext, var4Reliable))
        printf("var4Reliable changed to %i\n", var4Reliable);
    variableDeltaSerializer.EndDeserialize(&deserializationContext);
}

/** \brief network main class
 *
 *
 */
network::network( config *config, texture* image, block_list *block_list)
{
    p_rakPeerInterface = NULL;

    // broadcast on 255.255.255.255 at IPv4
    p_socketdescriptor.socketFamily=AF_INET;
    p_port = atoi( config->get( "port", "network", "27558").c_str() );
    p_maxamountplayer = atoi( config->get( "max_player", "network", "32").c_str() );

    p_client = false;

    p_starchip = new world( image, block_list);
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
	p_rakPeerInterface->Startup( p_maxamountplayer, &p_socketdescriptor, 1);
	p_rakPeerInterface->AttachPlugin( &p_replicaManager);
	p_replicaManager.SetNetworkIDManager( &p_networkIdManager);
	p_rakPeerInterface->SetMaximumIncomingConnections( p_maxamountplayer);

	printf("network::start: GUID is %s\n", p_rakPeerInterface->GetMyGUID().ToString());

	if (network_topology == CLIENT)
	{
		p_rakPeerInterface->Connect( p_ip.c_str(), p_port, 0, 0, 0);
		printf("network::start Connecting...\n");
	}

	p_starchip->addChunk( glm::vec3( 0, -1, 0) );
}

void network::start_sever()
{
    p_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
    network_topology = SERVER;
    p_socketdescriptor.port = p_port;
    start();
}

void network::start_client( std::string ip)
{
    p_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
    network_topology = CLIENT;
    p_socketdescriptor.port = 0;
    p_ip = ip;
    p_client = true;
    start();
}

void network::start_p2p()
{
    p_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
    network_topology = P2P;
    p_socketdescriptor.port = p_port;
    // freie stelle suchen
    while (IRNS2_Berkley::IsPortInUse( p_socketdescriptor.port, p_socketdescriptor.hostAddress, p_socketdescriptor.socketFamily, SOCK_DGRAM) == true)
        p_socketdescriptor.port++;
    start();
}

bool network::process()
{
    bool l_quit = false;
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
        }
    }

    // broadcast
    for (int i=0; i < 4; i++)
    {
        if ( p_rakPeerInterface->GetInternalID( RakNet::UNASSIGNED_SYSTEM_ADDRESS, 0).GetPort() != p_port+i)
             p_rakPeerInterface->AdvertiseSystem( "255.255.255.255", p_port+i, 0, 0, 0);
    }

    p_starchip->process();

    return l_quit;
}

void network::draw( graphic *graphic, config *config, glm::mat4 viewmatrix)
{
    p_starchip->draw( graphic, config, viewmatrix);
}
