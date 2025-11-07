#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef REUBEN_LIB
#define REUBEN_LIB
#endif

///////////////////////////////////
//		Header Include			///
///////////////////////////////////
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#ifndef USES_CONVERSION
	#include <atlbase.h>
#endif

#define GLOBAL_LOGGER GetGlobalLogger()

#include <Reuben/System/Reuben.h>
#include <Reuben/Simulation/Simulation.h>
#include <Reuben/Simulation/State.h>
#include <Reuben/Simulation/Event.h>
#include <Reuben/Simulation/Object.h>
#include <Reuben/Simulation/Group.h>
#include <Reuben/Simulation/ObjectManager.h>
#include <Reuben/Simulation/ScheduleManager.h>
#include <Reuben/Network/Network.h>
#include <Reuben/Network/Interface.h>
#include <Reuben/Network/Peer.h>
#include <Reuben/Network/RPC.h>
#include <Reuben/Network/NetObject.h>
#include <Reuben/Network/NetGroup.h>
#include <Reuben/Diagnostics/Logger.h>
#include <vector>
#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <deque>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

// using namespace Reuben::System;

///////////////////////////////////////////
//		Typedef Class/Structure			///
///////////////////////////////////////////
typedef Reuben::Network::Peer Peer;
typedef Reuben::Network::Listener Listener;
typedef Reuben::Network::RPCHeader RPCHeader;
typedef Reuben::Network::RPCContext RPCContext;
typedef Reuben::Network::Buffer Buffer;
typedef Reuben::Network::RPCID RPCID;
typedef Reuben::Network::RPCResult RPCResult;
typedef Reuben::Network::RPCMask RPCMask;
typedef Reuben::Network::Address Address;
typedef Reuben::Network::SocketID SocketID;
typedef Reuben::Network::ServiceID ServiceID;
typedef Reuben::Network::RPCNetID RPCNetID;
typedef Reuben::Network::NetObject NetObject;
typedef Reuben::Network::NetGroup NetGroup;
typedef Reuben::Network::SessionID SessionID;
typedef Reuben::Network::ProcedureID ProcedureID;
typedef Reuben::Simulation::ClassID ClassID;
typedef Reuben::Simulation::EventID EventID;
typedef Reuben::Simulation::Object Object;
typedef Reuben::Simulation::ObjectID ObjectID;
typedef Reuben::Simulation::State State;
typedef Reuben::Simulation::StateID StateID;
typedef Reuben::Simulation::Event Event;
typedef Reuben::Diagnostics::Logger Logger;
typedef Reuben::Network::Protocol Protocol;

///////////////////////////////////
//		Enum Definition			///
///////////////////////////////////
enum ServiceType
{
	SERVICE_NONE,
	SERVICE_LOG,
	SERVICE_WORLD,
	SERVICE_OBJECT,
	SERVICE_MESSAGE,
	SERVICE_MANAGER,
	SERVICE_PROXY,
	SERVICE_LOGIN,
	SERVICE_MAP,
	SERVICE_BATTLE,
	SERVICE_TYPE_COUNT
};

///////////////////////////////////
//		Type Definition			///
///////////////////////////////////
struct Service
{
	String strName;
	ServiceID serviceId;
	ServiceType type;
	Address address;
	Protocol protocol;
	Boolean bTrust;
	CONST Peer* pPeer;
	Boolean bConnected;
};

typedef StlVector<UInt32, ManagedAllocator<UInt32> > UInt32Vector;
typedef StlVector<UInt16, ManagedAllocator<UInt16> > UInt16Vector;
typedef StlVector<UInt8, ManagedAllocator<UInt8> > UInt8Vector;
typedef StlVector<Int32, ManagedAllocator<Int32> > Int32Vector;
typedef StlVector<Int16, ManagedAllocator<Int16> > Int16Vector;
typedef StlVector<Int8, ManagedAllocator<Int8> > Int8Vector;
typedef StlVector<float, ManagedAllocator<float> > FloatVector;
typedef StlVector<String, ManagedAllocator<String> > StringVector;
typedef StlVector<NetObject*, ManagedAllocator<NetObject*> > NetObjVector;
typedef StlVector<SessionID, ManagedAllocator<SessionID> > SessionIDVector;
typedef StlVector<RPCNetID, ManagedAllocator<RPCNetID> > RPCNetIDVector;


///////////////////////////////////
//		Macro Definition		///
///////////////////////////////////

#define SGN(X)	(((X) == 0) ? 0 : (((X) > 0) ? 1 : -1))
#define MIN(X, Y)	(((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)	(((X) > (Y)) ? (X) : (Y))
#define WITHIN(A, X, Y)		((A) < (X) ? (X) : ((A) > (Y) ? (Y) : (A)))

///////////////////////////////////
//		Function Prototype		///
///////////////////////////////////

CONST ServiceType StringToServiceType(PCSTRING pType);
PCSTRING ServiceTypeToString(CONST ServiceType eType);
Boolean IsFileExist(PCSTRING pFilename);

/////// Logger Functions ///////
extern Logger* gGlobalLogger;
Logger& GetGlobalLogger(VOID);

#include "Exception.h"

#endif // _COMMON_H_
