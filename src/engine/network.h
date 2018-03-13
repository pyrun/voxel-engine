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

#include "../graphic/debug_draw.h"

#define STATICLIB

#include "miniupnpc.h"
#include "upnpcommands.h"
#include "upnperrors.h"

#include "btBulletDynamicsCommon.h"
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

// ReplicaManager3 is in the namespace RakNet
using namespace RakNet;

enum
{
	CLIENT,
	SERVER
} network_topology;

struct network_object : public Replica3
{
    public:
        network_object();
        ~network_object();

        void init( btDiscreteDynamicsWorld *world);

        void draw( Shader *shader, glm::mat4 vp, object_handle *types);

        virtual RakNet::RakString GetName(void) const=0;

        virtual void WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const;
        void PrintStringInBitstream(RakNet::BitStream *bs);


        virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
        virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);

        virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection);

        virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection);
        virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection);

        /// Overloaded Replica3 function
        virtual void OnUserReplicaPreSerializeTick(void);

        virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters);

        virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters);

        virtual void SerializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{
            serializationBitstream->Write(GetName() + RakNet::RakString(" SerializeConstructionRequestAccepted"));
        }
        virtual void DeserializeConstructionRequestAccepted(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *acceptingConnection) {
            PrintStringInBitstream(serializationBitstream);
        }
        virtual void SerializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *requestingConnection)	{
            serializationBitstream->Write(GetName() + RakNet::RakString(" SerializeConstructionRequestRejected"));
        }
        virtual void DeserializeConstructionRequestRejected(RakNet::BitStream *serializationBitstream, RakNet::Connection_RM3 *rejectingConnection) {
            PrintStringInBitstream(serializationBitstream);
        }

        virtual void OnPoppedConnection(RakNet::Connection_RM3 *droppedConnection) { p_variableDeltaSerializer.RemoveRemoteSystemVariableHistory(droppedConnection->GetRakNetGUID()); }
        void NotifyReplicaOfMessageDeliveryStatus(RakNetGUID guid, uint32_t receiptId, bool messageArrived) { p_variableDeltaSerializer.OnMessageReceipt(guid,receiptId,messageArrived); }

        RakNet::RakString getTypeName() { return p_name; }

        void update_model();
        void setPos( glm::vec3 pos) { p_pos = pos; p_model_change = true; }
        void setRotate( glm::vec3 rotate) { p_rot = rotate; p_model_change = true; }
        glm::vec3 getPos() { return p_pos; };
        btRigidBody *getPhysicBody() { return p_body; }

    public:
        btRigidBody *p_body;
        btTransform p_transform;

        bool p_model_change;
        glm::vec3 p_pos;
        glm::vec3 p_rot;
        glm::vec3 p_scale;

        RakNet::RakString p_name;

        VariableDeltaSerializer p_variableDeltaSerializer;

        glm::mat4 p_model;

        object_type *p_type;
};

struct ClientCreatible_ClientSerialized : public network_object {
	virtual RakNet::RakString GetName(void) const {return RakNet::RakString("ClientCreatible_ClientSerialized");}
	virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters)
	{
		return network_object::Serialize(serializeParameters);
	}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_ClientConstruction(destinationConnection,network_topology!=CLIENT);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_ClientConstruction(sourceConnection,network_topology!=CLIENT);
	}

	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_ClientSerializable(destinationConnection,network_topology!=CLIENT);
	}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_Client(droppedConnection);
	}
};
struct ServerCreated_ClientSerialized : public network_object {
	virtual RakNet::RakString GetName(void) const {return RakNet::RakString("ServerCreated_ClientSerialized");}
	virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters)
	{
		return network_object::Serialize(serializeParameters);
	}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_ServerConstruction(destinationConnection,network_topology!=CLIENT);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_ServerConstruction(sourceConnection,network_topology!=CLIENT);
	}
	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_ClientSerializable(destinationConnection,network_topology!=CLIENT);
	}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_Server(droppedConnection);
	}
};
struct ClientCreatible_ServerSerialized : public network_object {
	virtual RakNet::RakString GetName(void) const {return RakNet::RakString("ClientCreatible_ServerSerialized");}
	virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters)
	{
		if (network_topology==CLIENT)
			return RM3SR_DO_NOT_SERIALIZE;
		return network_object::Serialize(serializeParameters);
	}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_ClientConstruction(destinationConnection,network_topology!=CLIENT);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_ClientConstruction(sourceConnection,network_topology!=CLIENT);
	}
	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_ServerSerializable(destinationConnection,network_topology!=CLIENT);
	}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_Client(droppedConnection);
	}
};
struct ServerCreated_ServerSerialized : public network_object {
	virtual RakNet::RakString GetName(void) const {return RakNet::RakString("ServerCreated_ServerSerialized");}
	virtual RM3SerializationResult Serialize(SerializeParameters *serializeParameters)
	{
		if (network_topology==CLIENT)
			return RM3SR_DO_NOT_SERIALIZE;

		return network_object::Serialize(serializeParameters);
	}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_ServerConstruction(destinationConnection,network_topology!=CLIENT);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_ServerConstruction(sourceConnection,network_topology!=CLIENT);
	}
	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_ServerSerializable(destinationConnection,network_topology!=CLIENT);
	}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_Server(droppedConnection);
	}
};
struct P2PReplica : public network_object {
	virtual RakNet::RakString GetName(void) const {return RakNet::RakString("P2PReplica");}
	virtual RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, ReplicaManager3 *replicaManager3) {
		return QueryConstruction_PeerToPeer(destinationConnection);
	}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {
		return QueryRemoteConstruction_PeerToPeer(sourceConnection);
	}
	virtual RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {
		return QuerySerialization_PeerToPeer(destinationConnection);
	}
	virtual RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {
		return QueryActionOnPopConnection_PeerToPeer(droppedConnection);
	}
};

class network_connection : public Connection_RM3 {
public:
	network_connection(const SystemAddress &_systemAddress, RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid) {}
	virtual ~network_connection() {}

	// See documentation - Makes all messages between ID_REPLICA_MANAGER_DOWNLOAD_STARTED and ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE arrive in one tick
	bool QueryGroupDownloadMessages(void) const {return true;}

	virtual Replica3 *AllocReplica(RakNet::BitStream *allocationId, ReplicaManager3 *replicaManager3)
	{
		RakNet::RakString typeName;
		allocationId->Read(typeName);
		if (typeName=="ClientCreatible_ClientSerialized") return new ClientCreatible_ClientSerialized;
		if (typeName=="ServerCreated_ClientSerialized") return new ServerCreated_ClientSerialized;
		if (typeName=="ClientCreatible_ServerSerialized") return new ClientCreatible_ServerSerialized;
		if (typeName=="ServerCreated_ServerSerialized") return new ServerCreated_ServerSerialized;
		return 0;
	}
protected:
};

class network_mananger : public ReplicaManager3
{
	virtual Connection_RM3* AllocConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID) const {
		return new network_connection(systemAddress,rakNetGUID);
	}
	virtual void DeallocConnection(Connection_RM3 *connection) const {
		delete connection;
	}
};

class network
{
    public:
        network( config *config, texture* image, block_list *block_list);
        virtual ~network();

        bool init_upnp();

        void start();
        void start_sever();
        void start_client( std::string ip = "127.0.0.1");

        void sendBlockChange( Chunk *chunk, glm::vec3 pos, int id);
        void readBlockChange( BitStream *bitstream);

        void readChunk( BitStream *bitstream);
        void sendChunk( Chunk *chunk, RakNet::AddressOrGUID address);
        void sendAllChunks( world *world,RakNet::AddressOrGUID address);

        bool process( int delta);

        void draw( graphic *graphic, config *config, glm::mat4 viewmatrix);

        void create_object();

        world *getWorld() { return p_starchip; }
        bool isServer() { return p_isServer; }
        bool isClient() { return p_isClient; }
        debug_draw *getDebugDraw() { return &p_debugdraw; }
    protected:

    private:
        btDiscreteDynamicsWorld *p_physic_world;
        debug_draw p_debugdraw;

        RakNet::SocketDescriptor p_socketdescriptor;
        NetworkIDManager p_networkIdManager;
        RakNet::RakPeerInterface *p_rakPeerInterface;
        network_mananger p_replicaManager;

        RakNet::Packet *p_packet;
        bool p_isClient;
        bool p_isServer;

        int p_port;
        int p_maxamountplayer;
        std::string p_ip;

        world *p_starchip;
        object_handle *p_types;
};

#endif // NETWORK_H
