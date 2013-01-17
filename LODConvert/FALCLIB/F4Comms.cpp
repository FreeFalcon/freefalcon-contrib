#pragma optimize( "", off ) // JB 010718

#include "falclib.h"
#include "f4comms.h"
#include "capiopt.h"
#include "comms\capi.h"
#include "router.h" //KCK This needs to go away
//#include "stdhdr.h"
#include "ui\include\uicomms.h" // UI comms manager
#include "falclib\include\f4find.h"
#include "FalcMesg.h"
#include "MsgInc\TimingMsg.h"
#include "Falcmesg.h"
#include "DispCfg.h"
#include "CmpClass.h"
#include "ComData.h"
#include "ui\include\queue.h"
#include "ui\include\falcuser.h"
#include "TimerThread.h"
#include "acselect.h"
#include "pilot.h"
#include "flight.h"
#include "voicecomunication\voicecom.h"
#include "falclib\include\MsgInc\PlayerStatusMsg.h"
#include "falclib\include\MsgInc\SimCampMsg.h"
#include "aircrft.h"
#include "SimBase.h"
#include <objbase.h>
#include <cguid.h>
#include <dplay8.h>
#include <initguid.h>



// ==============================================
// Insert DPLAY crap here
// ==============================================

DEFINE_GUID (OVERRIDE_GUID, 0x126e6180, 0xd307, 0x11d0, 0x9c, 0x4f, 0x0, 0xa0, 0xc9, 0x5, 0x42, 0x5e);
GUID	gOurGUID = OVERRIDE_GUID;

// ========================================================================
// Some defines 
// ========================================================================

#define F4COMMS_CONNECTION_TIMEOUT_SECS 8

#define F4COMMS_CONNECTED_TO_NO_ONE		0
#define F4COMMS_PARTIALLY_CONNECTED		1
#define F4COMMS_FULLY_CONNECTED			2

// KCK: These really need to come from comms itself!!
#define COMMS_TCP_OVERHEAD				40
#define COMMS_UDP_OVERHEAD				12

#define F4COMMS_MAX_PACKETS				32

// ====================================
// Globals
// ====================================

ComAPIHandle FalconTCPListenHandle = NULL;
ComAPIHandle FalconGlobalUDPHandle = NULL;
ComAPIHandle FalconGlobalTCPHandle = NULL;
//ComAPIHandle FalconInitialUDPHandle = NULL;
int FalconServerTCPStatus = VU_CONN_INACTIVE;
int FalconConnectionProtocol = 0;
int FalconConnectionType = 0;
int FalconConnectionDescription = 0;
int gConnectionStatus = 0;
int gTimeModeServer = 0;

extern bool g_bServer;
int g_b_forcebandwidth = 0;

//DCNode *DanglingConnections = NULL;
FalconPrivateList *DanglingSessionsList = NULL;

// debug bandwidth limiters
int F4CommsBandwidth = 0;
int F4CommsLatency = 100;
int F4CommsDropInterval = 0;
int F4CommsSwapInterval = 0;
int F4SessionUpdateTime = 0;
int F4SessionAliveTimeout = 0;
int F4LatencyPercentage = 100;
int F4CommsMTU = 0; // MTU variable declared Unz
extern bool g_bF4CommsMTU; // Unz switch for MTU 
// ========================================================================
// Message sizing variables
// ========================================================================

// Ideal packet size we'll send over the wire;
int	F4CommsIdealPacketSize;
// Corrisponding content sizes
int F4CommsIdealTCPPacketSize;
int F4CommsIdealUDPPacketSize;
// Corrisponding messages sizes
int F4CommsMaxTCPMessageSize;
int F4CommsMaxUDPMessageSize;
// Maximum sized message vu can accept
int F4VuMaxTCPMessageSize;
int F4VuMaxUDPMessageSize;
// Maximum sized packet vu with pack messages into
int F4VuMaxTCPPackSize;
int F4VuMaxUDPPackSize;

// ====================================
// Prototypes
// ====================================

void CleanupDanglingList (void);
void AddDanglingSession (ComAPIHandle ch1, ComAPIHandle ch2, ulong ipAddress);
int RemoveDanglingSession (VuSessionEntity *newSess);
void SendConnectionList (FalconSessionEntity *session);
int F4CommsConnectionCallback (int result);
void FillSerialDataString(char serial_data[], ComDataClass *comData);
void ModemConnectCallback(ComAPIHandle ch, int ret);
void TcpConnectCallback(ComAPIHandle ch, int ret);
int CleanupComms (void);
void ShutdownCampaign (void);
ulong TimeStampFunction (void);
static void set_bandwidth_limit (int bd);

// ====================================
extern VuDriverSettings *vuxDriverSettings;
// ====================================

// ====================================
// COMMS startup and shutdown stuff
// ====================================

// This sets up a comms session (both udp and tcp handles) using the current protocol.
// FalconGlobalHandles are handles to the whole world (ie: broadcast on a LAN, or a server address, 
// or NULL if not available)
// tcpListenHandle is a listen handle where we can recieve tcp connection requests.
//
// InitCommsStuff takes the following parameters:
// int GameType:				Nonzero if a game is currently running and needs to
//								Attach to a group immediately.
// FalconConnectionType fct:	FalconConnectionType enumerated type
// ulong IPAddress:				IPAddress of server or other machine, if any.
//
// Returns < 0 on error.
char* g_ipadress = NULL;
extern IDirectPlay8Server* g_pDPServer;
extern IDirectPlay8Client* g_pDPClient;
extern bool g_bVoiceCom;
extern char g_strVoiceHostIP[0x40];
extern bool stoppingvoice;
extern bool g_bACPlayerCTDFix;

int InitCommsStuff (ComDataClass *comData)
	{
	g_ipadress = ComAPIinet_htoa(comData->ip_address);//me123
	ComAPIHandle tmpHandle = NULL;
        int bigPipeBandwidth = -1;
        int smallPipeBandwidth = 2000;

	MonoPrint ("InitCommsStuff\n");

	FalconTCPListenHandle = NULL;
	FalconGlobalUDPHandle = NULL;
	FalconGlobalTCPHandle = NULL;
//	FalconInitialUDPHandle = NULL;
	FalconServerTCPStatus = VU_CONN_INACTIVE;
//	DanglingConnections = NULL;7

        if (0)//me123 F4CommsBandwidth != 0)
        {
            bigPipeBandwidth = F4CommsBandwidth;
            smallPipeBandwidth = F4CommsBandwidth;
        }
#ifdef CAPI_NET_DEBUG_FEATURES
        if (F4CommsLatency > 0 || F4CommsDropInterval > 0 || F4CommsSwapInterval > 0)
        {
            ComAPIInitLatency(F4CommsLatency, F4CommsLatency*3,
                              F4CommsDropInterval, 1, 100, F4CommsSwapInterval);
        }
#endif

	// Shutdown any current games/connections
	SendMessage(FalconDisplay.appWin,FM_SHUTDOWN_CAMPAIGN,0,0);
	if (gConnectionStatus)
		EndCommsStuff();
	gConnectionStatus = F4COMMS_ERROR_UDP_NOT_AVAILABLE;

	FalconConnectionDescription = comData->protocol;
	SetupMessageSizes (comData->protocol);

	switch (comData->protocol)
		{
		case FCT_NoConnection:
		default:
			FalconConnectionProtocol = 0;
			FalconConnectionType = 0;
			F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);
			break;
		case FCT_ModemToModem:
		{
			set_bandwidth_limit (comData->modem_rate);
			if (comData->connect_type == F4_COMMS_HOST)
				FalconGlobalUDPHandle = ComDPLAYOpen(CAPI_DPLAY_MODEM_PROTOCOL, CAPI_HOST, "", F4CommsMaxTCPMessageSize, &gOurGUID, TcpConnectCallback, 120);
			else
				FalconGlobalUDPHandle = ComDPLAYOpen(CAPI_DPLAY_MODEM_PROTOCOL, CAPI_JOIN, comData->phone_number, F4CommsMaxTCPMessageSize, &gOurGUID, TcpConnectCallback, 120);
			FalconConnectionProtocol = FCP_SERIAL_AVAILABLE;	// Serial is assumed reliable
			FalconConnectionType = FCT_SERIAL_AVAILABLE;
			gConnectionStatus = F4COMMS_PENDING;
			// KCK: Remainder of connection done in ModemConnectCallback();
			// PETER TODO: Time out/cancle connection if not connected in X seconds
			break;
		}

		case FCT_NullModem:
			char	serial_data[80];
			FillSerialDataString(serial_data, comData);
			if (comData->connect_type == F4_COMMS_HOST)
				{
				MonoPrint("Joining with data: %s\n",serial_data);
				FalconGlobalUDPHandle = ComDPLAYOpen(CAPI_DPLAY_SERIAL_PROTOCOL, CAPI_JOIN, serial_data, F4CommsMaxTCPMessageSize, &gOurGUID, TcpConnectCallback, 120);
				}
			else
				{
				MonoPrint("Hosting with data: %s\n",serial_data);
				FalconGlobalUDPHandle = ComDPLAYOpen(CAPI_DPLAY_SERIAL_PROTOCOL, CAPI_HOST, serial_data, F4CommsMaxTCPMessageSize, &gOurGUID, TcpConnectCallback, 120);
				}
			FalconConnectionProtocol = FCP_SERIAL_AVAILABLE;	// Serial is assumed reliable
			FalconConnectionType = FCT_SERIAL_AVAILABLE;
			gConnectionStatus = F4COMMS_PENDING;
			// KCK: Remainder of connection done in ModemConnectCallback();
			// PETER TODO: Time out/cancle connection if not connected in X seconds
			break;
		case FCT_LAN:
			set_bandwidth_limit (comData->lan_rate);
			FalconGlobalUDPHandle = ComUDPOpen("LAN Broadcast UDP", F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, 0);
			if (!FalconGlobalUDPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);

			MonoPrint ("CreateGroup LAN FalconGlobalTCPHandle\n");
			FalconGlobalTCPHandle = ComAPICreateGroup("LAN RUDP Group", F4CommsMaxTCPMessageSize,0);
			if (!FalconGlobalTCPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_MULTICAST_NOT_AVAILABLE);

			FalconConnectionProtocol = FCP_UDP_AVAILABLE | FCP_RUDP_AVAILABLE;
			FalconConnectionType = FCT_PTOP_AVAILABLE | FCT_BCAST_AVAILABLE;
			F4CommsConnectionCallback(F4COMMS_CONNECTED);
			break;
		case FCT_WAN:
		case FCT_JetNet:
			set_bandwidth_limit(comData->protocol == FCT_WAN ? comData->internet_rate : comData->jetnet_rate);
			// Our UDP Handle is a group.
			FalconGlobalUDPHandle = ComAPICreateGroup("CreateGroup WAN FalconGlobalUDPHandle\n", F4CommsMaxUDPMessageSize,0);
			if (!FalconGlobalUDPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);

			// We need to add the one person we know about to our UDP com handle so we can
			// send our initial requests for global entities. Once we connect to this machine
			// we'll use this com handle instead of creating a new one.

			if (!comData->ip_address)
			{
				// if we don't have an ip address - tell comms to use our own.
				tmpHandle = ComUDPOpen("WAN My UDP", F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, 0xfffffff);
				if (!tmpHandle)
				{
					return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);
				}
			}
			else
			{
				tmpHandle = ComUDPOpen("WAN KnownIP UDP", F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, comData->ip_address);
				if (!tmpHandle)
				{
					return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);
				}
			}

			// if we know the other guy, add him into our global group
			if (comData->ip_address)
			{
				AddDanglingSession(tmpHandle, NULL, comData->ip_address);
			}
			else
			{
				AddDanglingSession(tmpHandle, NULL, 0xffffffff);
			}

			MonoPrint ("ComAPIAddToGroup FalconGlobalUDPHandle %08x CH%08x\n", FalconGlobalUDPHandle, tmpHandle);
			ComAPIAddToGroup(FalconGlobalUDPHandle,tmpHandle);

			// Out TCP Handle will be a group as well (implemented as RUDP)
			MonoPrint ("CreateGroup WAN FalconGlobalTCPHandle\n");
			FalconGlobalTCPHandle = ComAPICreateGroup("WAN RUDP GROUP", F4CommsMaxTCPMessageSize,0);
			
			if (!FalconGlobalTCPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_MULTICAST_NOT_AVAILABLE);

			// We have UDP & RUDP and only PTOP
			FalconConnectionProtocol = FCP_UDP_AVAILABLE | FCP_RUDP_AVAILABLE;
			FalconConnectionType = FCT_PTOP_AVAILABLE;
			F4CommsConnectionCallback(F4COMMS_CONNECTED);

			break;
		case FCT_Server:
			ComAPISetBandwidth (smallPipeBandwidth);
			FalconServerTCPStatus = VU_CONN_PENDING;

			FalconTCPListenHandle = ComTCPOpenListen(F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, TcpConnectCallback);
			if (!FalconTCPListenHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_TCP_NOT_AVAILABLE);

			FalconGlobalUDPHandle = ComUDPOpen("Server", F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, comData->ip_address);
			if (!FalconGlobalUDPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);

			FalconGlobalTCPHandle = ComTCPOpenConnect(F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, comData->ip_address, TcpConnectCallback, 15);
			if (!FalconGlobalTCPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_TCP_NOT_AVAILABLE);

			FalconConnectionProtocol = FCP_UDP_AVAILABLE | FCP_TCP_AVAILABLE;
			FalconConnectionType = FCT_SERVER_AVAILABLE;
			gConnectionStatus = F4COMMS_PENDING;
			// KCK: Remainder of connection done in ModemConnectCallback();
			// PETER TODO: Time out/cancle connection if not connected in X seconds
			break;
		case FCT_TEN:
			ComAPISetBandwidth (smallPipeBandwidth);
			FalconServerTCPStatus = VU_CONN_PENDING;

			FalconTCPListenHandle = ComTCPOpenListen(F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, TcpConnectCallback);
			if (!FalconTCPListenHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_TCP_NOT_AVAILABLE);

			FalconGlobalUDPHandle = ComUDPOpen("TEN", F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, comData->ip_address);
			if (!FalconGlobalUDPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_UDP_NOT_AVAILABLE);

			FalconGlobalTCPHandle = ComTCPOpenConnect(F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, comData->ip_address, TcpConnectCallback, 15);
			if (!FalconGlobalTCPHandle)
				return F4CommsConnectionCallback(F4COMMS_ERROR_TCP_NOT_AVAILABLE);

			FalconConnectionProtocol = FCP_UDP_AVAILABLE | FCP_TCP_AVAILABLE;
			FalconConnectionType = FCT_SERVER_AVAILABLE;
			gConnectionStatus = F4COMMS_PENDING;
			// KCK: Remainder of connection done in ModemConnectCallback();
			// PETER TODO: Time out/cancle connection if not connected in X seconds
			break;
		}

	ComAPISetTimeStampFunction(TimeStampFunction);

	return gConnectionStatus;
	}

void VuxAddDanglingSession (VU_SESSION_ID id)
{
	static int
		count = 0;

	ComAPIHandle
		tmpHandle;

	char
		buffer[100];

	sprintf (buffer, "Dangling%08x", id);

	MonoPrint ("VuxAddDanglingSession %08x\n", id);

	if (FalconConnectionType & FCT_BCAST_AVAILABLE)
	{
	}
	else
	{
		VuListIterator			dsit(DanglingSessionsList);

		FalconSessionEntity		*session;

		for (session = (FalconSessionEntity*)dsit.GetFirst(); session; session = (FalconSessionEntity*)dsit.GetNext())
		{
			if (session->OwnerId().creator_.value_ == id)
			{
				if (FalconConnectionProtocol & FCP_RUDP_AVAILABLE)
				{
					ComAPIHandle
						sh = NULL;
					
					count ++;
					
					if (count % 4 == 0)
					{
						sh = ComRUDPOpen(buffer, F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, id, F4CommsIdealPacketSize);
						
						if (sh)
						{
							ComAPIClose (sh);
						}
					}
				}

				return;
			}
		}

		tmpHandle = ComUDPOpen(buffer, F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, id);

		if (tmpHandle)
		{
			AddDanglingSession(tmpHandle, NULL, id);
			MonoPrint ("ComAPIAddToGroup FalconGlobalUDPHandle %08x CH%08x\n", FalconGlobalUDPHandle, tmpHandle);
			ComAPIAddToGroup (FalconGlobalUDPHandle,tmpHandle);
		}
	}

	if (FalconConnectionProtocol & FCP_RUDP_AVAILABLE)
	{
		ComAPIHandle
			sh = NULL;

		count ++;
		
		if (count % 4 == 0)
		{
			sh = ComRUDPOpen(buffer, F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, id, F4CommsIdealPacketSize);

			if (sh)
			{
				ComAPIClose (sh);
			}
		}
	}
}

// This gets called after we've made a connection (or failed for sure)
int F4CommsConnectionCallback (int result)
{
	if (result == F4COMMS_CONNECTED)
	{
		int
			vures;

		// Init vu's comms
		if (!FalconGlobalTCPHandle)
		{
			vures = gMainThread->InitComms
			(
				FalconGlobalUDPHandle, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize, 
				FalconGlobalUDPHandle, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize, 
				F4_EVENT_QUEUE_SIZE
			);
		}
		else
		{
			vures = gMainThread->InitComms
			(
				FalconGlobalUDPHandle, F4CommsMaxUDPMessageSize, F4CommsIdealUDPPacketSize, 
				FalconGlobalTCPHandle, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize,
				F4_EVENT_QUEUE_SIZE
			);
		}

		if (vures == VU_ERROR)
		{
			// KCK: Vu often doesn't clean up after itself in this case!
			MonoPrint("Failed to init Comms!\n");

			CleanupComms();

			return F4COMMS_ERROR_UDP_NOT_AVAILABLE;
		}

		gConnectionStatus = F4COMMS_CONNECTED;
	}
	else
	{
		CleanupComms();
	}
	// KCK: Call UI Comms connect callback
	gCommsMgr->StartCommsDoneCB(result);
	return result;
}

int CleanupComms (void)
	{
	gConnectionStatus = 0;

	if (FalconGlobalUDPHandle)
		ComAPIClose(FalconGlobalUDPHandle);
	if (FalconGlobalTCPHandle)
		ComAPIClose(FalconGlobalTCPHandle);
	if (FalconTCPListenHandle)
		ComAPIClose(FalconTCPListenHandle);
//	if (FalconInitialUDPHandle)
//		ComAPIClose(FalconInitialUDPHandle);

	// Kill off any dangling sessions
	CleanupDanglingList();

	// Shut down any dangling connections
/*	if (DanglingConnections)
		{
		DCNode		*next_node = DanglingConnections->next;
		ComAPIClose(DanglingConnections->ch);
		delete DanglingConnections;
		DanglingConnections = next_node;
		}
*/
	
//	FalconInitialUDPHandle = NULL;
	FalconTCPListenHandle = NULL;
	FalconGlobalUDPHandle = NULL;
	FalconGlobalTCPHandle = NULL;
	FalconServerTCPStatus = VU_CONN_INACTIVE;
	return (TRUE);
	}

int EndCommsStuff (void)
	{
	if (gMainThread)
		gMainThread->LeaveGame();

	// KCK HACK: To avoid vu's problem with shutting down comms when remote sessions are active
	VuSessionsIterator	siter(vuGlobalGroup);
	FalconSessionEntity	*cs;
	cs = (FalconSessionEntity*) siter.GetFirst();
	while (cs)
		{
		cs->JoinGame(NULL);
		cs = (FalconSessionEntity*) siter.GetNext();
		}
	// END HACK

	if (gConnectionStatus == F4COMMS_CONNECTED)
		gMainThread->DeinitComms();

	CleanupComms();

	return (TRUE);
	}

void SetupMessageSizes (int protocol)
	{
	// Ideal packet size comms will send over the wire
	if (g_bF4CommsMTU) F4CommsIdealPacketSize = F4CommsMTU;  // Unz and Booster MTU tweek
	else F4CommsIdealPacketSize = 500;
	MonoPrint ("****** CHECK F4CommsMTU size: %d ******",F4CommsIdealPacketSize ); // Unz Testing
	// Corrisponding content sizes
	F4CommsIdealTCPPacketSize = F4CommsIdealPacketSize - COMMS_TCP_OVERHEAD;
	F4CommsIdealUDPPacketSize = F4CommsIdealPacketSize - COMMS_UDP_OVERHEAD;
	// Corrisponding messages sizes
	F4CommsMaxTCPMessageSize = F4CommsIdealTCPPacketSize * F4COMMS_MAX_PACKETS;
	F4CommsMaxUDPMessageSize = F4CommsIdealUDPPacketSize;
	// Maximum sized message vu can accept
	F4VuMaxTCPMessageSize = F4CommsMaxTCPMessageSize - PACKET_HDR_SIZE - MAX_MSG_HDR_SIZE;
	F4VuMaxUDPMessageSize = F4CommsMaxUDPMessageSize - PACKET_HDR_SIZE - MAX_MSG_HDR_SIZE;
	// Maximum sized packet vu with pack messages into
	F4VuMaxTCPPackSize = F4CommsIdealTCPPacketSize - PACKET_HDR_SIZE - MAX_MSG_HDR_SIZE;
	F4VuMaxUDPPackSize = F4CommsIdealUDPPacketSize - PACKET_HDR_SIZE - MAX_MSG_HDR_SIZE;
	protocol;
	}

// ===========================================
// Serial/Modem data startup functions
// ===========================================

void FillSerialDataString(char serial_data[], ComDataClass *comData)
	{
	serial_data[0] = 0;
	switch (comData->com_port)
		{
		case F4_COMMS_COM1:
			strcat(serial_data,"COM1 ");
			break;
		case F4_COMMS_COM2:
			strcat(serial_data,"COM2 ");
			break;
		case F4_COMMS_COM3:
			strcat(serial_data,"COM3 ");
			break;
		case F4_COMMS_COM4:
			strcat(serial_data,"COM4 ");
			break;
		default:
			break;
		}
	switch (comData->baud_rate)
		{
		case F4_COMMS_CBR_110:
		case F4_COMMS_CBR_600:	
		case F4_COMMS_CBR_1200:	
			// KCK: Can we seriously expect anyone to connect with < 1200 baud?
			strcat(serial_data,"CBR_1200 ");
//			ComAPISetBandwidth (100, 150);
			ComAPISetBandwidth (100);
			break;
		case F4_COMMS_CBR_2400:	
			strcat(serial_data,"CBR_2400 ");
//			ComAPISetBandwidth (200, 300);
			ComAPISetBandwidth (200);
			break;
		case F4_COMMS_CBR_4800:	
			strcat(serial_data,"CBR_4800 ");
//			ComAPISetBandwidth (400, 500);
			ComAPISetBandwidth (350);
			break;
		case F4_COMMS_CBR_9600:
			strcat(serial_data,"CBR_9600 ");
//			ComAPISetBandwidth (800, 1000);
			ComAPISetBandwidth (600);
			break;
		case F4_COMMS_CBR_14400:
			strcat(serial_data,"CBR_14400 ");
//			ComAPISetBandwidth (1200, 1500);
			ComAPISetBandwidth (800);
			break;
		case F4_COMMS_CBR_19200:
			strcat(serial_data,"CBR_19200 ");
//			ComAPISetBandwidth (1500, 2000);
			ComAPISetBandwidth (1000);
			break;
		case F4_COMMS_CBR_38400:
			strcat(serial_data,"CBR_38400 ");
//			ComAPISetBandwidth (2000, 3000);
			ComAPISetBandwidth (1500);
			break;
		case F4_COMMS_CBR_56000:
			strcat(serial_data,"CBR_56000 ");
//			ComAPISetBandwidth (4000, 5000);
			ComAPISetBandwidth (4000);
			break;
		case F4_COMMS_CBR_57600:
			strcat(serial_data,"CBR_57600 ");
			ComAPISetBandwidth (4000);
			break;
		case F4_COMMS_CBR_115200:	
			strcat(serial_data,"CBR_115200 ");
			ComAPISetBandwidth (8000);
			break;
		case F4_COMMS_CBR_128000:	
//			strcat(serial_data,"CBR_128000 ");
			break;
		case F4_COMMS_CBR_256000:	
//			strcat(serial_data,"CBR_256000 ");
			break;
		default:
			break;
		}
	switch (comData->parity)
		{
		case F4_COMMS_NOPARITY:
			strcat(serial_data,"NOPARITY ");
			break;		
		case F4_COMMS_ODDPARITY:	
			strcat(serial_data,"ODDPARITY ");
			break;
		case F4_COMMS_EVENPARITY:	
			strcat(serial_data,"EVENPARITY ");
			break;
		case F4_COMMS_MARKPARITY:	
//			strcat(serial_data,"MARKPARITY ");
			break;
		default:
			break;
		}
	switch (comData->stop_bits)
		{
		case ONESTOPBIT:
			strcat(serial_data,"ONESTOPBIT ");
			break;		
		case ONE5STOPBITS:	
			strcat(serial_data,"ONESTOPBIT ");
//			strcat(serial_data,"ONE5STOPBITS ");
			break;
		case TWOSTOPBITS:	
			strcat(serial_data,"TWOSTOPBITS ");
			break;
		default:
			break;
		}
	switch (comData->flow_control)
		{
		case F4_COMMS_DPCPA_DTRFLOW:
//			strcat(serial_data,"DPCPA_DTRFLOW ");
			break;
		case F4_COMMS_DPCPA_NOFLOW:	
//			strcat(serial_data,"DPCPA_NOFLOW ");
			break;
		case F4_COMMS_DPCPA_RTSDTRFLOW:	
			strcat(serial_data,"DPCPA_RTSDTRFLOW ");
			break;
		case F4_COMMS_DPCPA_RTSFLOW:	
//			strcat(serial_data,"DPCPA_RTSFLOW ");
			break;
		case F4_COMMS_DPCPA_XONXOFFFLOW:	
//			strcat(serial_data,"DPCPA_XONXOFFFLOW ");
			break;
		default:
			break;
		}
	}

// ==========================
// COMMS handle creation
// ==========================

// Set up a handle to communicate via UDP with everyone in this group.
int VuxGroupConnect(VuGroupEntity *group)
{
	char
		buffer[100],
		*name;

	if (group->IsGame())
	{
		name = ((VuGameEntity*)group)->GameName();
		MonoPrint("Connecting to game: %s\n",((VuGameEntity*)group)->GameName());
	}
	else
	{
		name = group->GroupName();
		MonoPrint("Connecting to group: %s\n",group->GroupName());
	}

	// Check for existing connections
	if (!group->GetCommsHandle())
		{
		if (!(FalconConnectionProtocol & FCP_UDP_AVAILABLE) && !(FalconConnectionProtocol & FCP_SERIAL_AVAILABLE))
			{
			// No udp connections available
			group->SetCommsHandle(NULL);
			group->SetCommsStatus(VU_CONN_INACTIVE);
			}
		else if (FalconConnectionType & FCT_SERIAL_AVAILABLE)
			{
			group->SetCommsHandle(NULL);		// We'll inherit from our global group
			group->SetCommsStatus(VU_CONN_ACTIVE);
			}
		else if (FalconConnectionType & FCT_SERVER_AVAILABLE && FalconGlobalUDPHandle)
			{
			// Point us to our server's UDP connection
			group->SetCommsHandle(NULL);		// We'll inherit from our global group
			group->SetCommsStatus(VU_CONN_ACTIVE);
			}
		else if (FalconConnectionType & FCT_BCAST_AVAILABLE && FalconGlobalUDPHandle)
			{
			// Since we have broadcast available, pass our broadcast handle
			group->SetCommsHandle(NULL);		// We'll inherit from our global group
			group->SetCommsStatus(VU_CONN_ACTIVE);
			}
		else if (FalconConnectionType & FCT_PTOP_AVAILABLE)
			{
			// Point to Point only - Create a new comms group which we will add shit to.
			sprintf (buffer, "%s UDP", name);
			MonoPrint ("CreateGroup %s\n", buffer);
			ComAPIHandle	gh = ComAPICreateGroup(buffer, F4CommsMaxUDPMessageSize,0);
			group->SetCommsHandle(gh, F4CommsMaxUDPMessageSize, F4CommsIdealUDPPacketSize);
			if (gh)
				group->SetCommsStatus(VU_CONN_ACTIVE);
			else
				group->SetCommsStatus(VU_CONN_INACTIVE);
			}
		else
			{
			// No communication available.
			group->SetCommsHandle(NULL);
			group->SetCommsStatus(VU_CONN_INACTIVE);
			}
		}
	if (!group->GetReliableCommsHandle())
		{
		if (!(FalconConnectionProtocol & FCP_TCP_AVAILABLE) && !(FalconConnectionProtocol & FCP_SERIAL_AVAILABLE) && !(FalconConnectionProtocol & FCP_RUDP_AVAILABLE))
			{
			// No reliable connections available
			group->SetReliableCommsHandle(NULL);
			group->SetReliableCommsStatus(VU_CONN_INACTIVE);
			}
		else if (FalconConnectionType & FCT_SERIAL_AVAILABLE)
			{
			group->SetCommsHandle(NULL);		// We'll inherit from our global group
			group->SetCommsStatus(VU_CONN_ACTIVE);
			}
		else if (FalconConnectionType & FCT_SERVER_AVAILABLE && FalconGlobalTCPHandle)
			{
			// Point us to our server's tcp connection
			group->SetCommsHandle(NULL);		// We'll inherit from our global group
			group->SetReliableCommsStatus(VU_CONN_ACTIVE);
			}
		else if (FalconConnectionType & FCT_PTOP_AVAILABLE)
			{
			// Point to Point only - Create a new comms group which we will add shit to.
			sprintf (buffer, "%s RUDP", name);
			MonoPrint ("CreateGroup %s\n", buffer);
			ComAPIHandle	gh = ComAPICreateGroup(buffer, F4CommsMaxTCPMessageSize,0);
			group->SetReliableCommsHandle(gh, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
			if (gh)
				group->SetReliableCommsStatus(VU_CONN_ACTIVE);
			else
				group->SetReliableCommsStatus(VU_CONN_INACTIVE);
			}
		else
			{
			// No reliable group communication available.
			group->SetReliableCommsHandle(NULL);
			group->SetReliableCommsStatus(VU_CONN_INACTIVE);
			}
		}
	if(gUICommsQ && group->IsGame())
		gUICommsQ->Add(_Q_GAME_ADD_,FalconNullId,group->Id());

	return 0;
	}

void VuxGroupDisconnect(VuGroupEntity *group)
	{
	ComAPIHandle ch;

	if (group->IsGame())
	{
		MonoPrint("Disconnecting to game: %s\n",((VuGameEntity*)group)->GameName());
	}
	else
	{
		MonoPrint("Disconnecting to group: %s\n",group->GroupName());
	}

	// Disconnect UDP
	ch = group->GetCommsHandle();
	group->SetCommsHandle(NULL);
	group->SetCommsStatus(VU_CONN_INACTIVE);
	if (ch && ch != FalconGlobalUDPHandle)
		ComAPIClose(ch);
	// Disconnect TCP
	ch = group->GetReliableCommsHandle();
	group->SetReliableCommsHandle(NULL);
	group->SetReliableCommsStatus(VU_CONN_INACTIVE);
	if (ch && ch != FalconGlobalTCPHandle)
		ComAPIClose(ch);

	if(gUICommsQ && group->IsGame())
		gUICommsQ->Add(_Q_GAME_REMOVE_,FalconNullId,group->Id());
	}


int VuxGroupAddSession(VuGroupEntity *group, VuSessionEntity *session)
	{
	ComAPIHandle	gh,sh;
	if (g_bVoiceCom)
	{
	if (strcmpi(g_strVoiceHostIP, "")) g_ipadress = g_strVoiceHostIP;
	if ( (gConnectionStatus == F4COMMS_CONNECTED || 
		(g_ipadress && !strcmpi(g_ipadress, "0.0.0.0"))) && !stoppingvoice && !g_pDPServer && !g_pDPClient  &&(g_ipadress)) 
		startupvoice(g_ipadress);//me123
	}

	if (group->IsGame())
	{
		MonoPrint("Adding %s to game: %s\n",((FalconSessionEntity*)session)->GetPlayerCallsign(),((VuGameEntity*)group)->GameName());
	}
	else
	{
		MonoPrint("Adding %s to group: %s\n",((FalconSessionEntity*)session)->GetPlayerCallsign(),group->GroupName());
	}

	gh = group->GetCommsHandle();
	sh = session->GetCommsHandle();
	if (gh && sh && gh != sh)
		ComAPIAddToGroup(gh,sh);
	gh = group->GetReliableCommsHandle();
	sh = session->GetReliableCommsHandle();
	if (gh && sh && gh != sh)
		ComAPIAddToGroup(gh,sh);
	
	// UI Related
	if(gUICommsQ && group->IsGame())
		gUICommsQ->Add(_Q_SESSION_ADD_,session->Id(),group->Id());

	// Send FullUpdate for session if this is our game
	if
	(
		(group->IsGame ()) &&
		(group->Id () == vuLocalSessionEntity->GameId ()) &&
		(vuLocalSessionEntity != session)
	)
	{
		VuFullUpdateEvent *msg = new VuFullUpdateEvent (vuLocalSessionEntity, session);
		msg->RequestReliableTransmit ();
		VuMessageQueue::PostVuMessage(msg);
	}

	return 1;
	}

int VuxGroupRemoveSession(VuGroupEntity *group, VuSessionEntity *session)
{
	ComAPIHandle	gh,sh;

	if (group->IsGame())
	{
		MonoPrint("Removing %s from game: %s\n",((FalconSessionEntity*)session)->GetPlayerCallsign(),((VuGameEntity*)group)->GameName());
	}
	else
	{
		MonoPrint("Removing %s from group: %s\n",((FalconSessionEntity*)session)->GetPlayerCallsign(),group->GroupName());
	}

	gh = group->GetCommsHandle();
	sh = session->GetCommsHandle();
	if (gh && sh && gh != sh)
		ComAPIDeleteFromGroup(gh,sh);
//	if (sh == FalconInitialUDPHandle && gh == FalconGlobalUDPHandle)
//		FalconInitialUDPHandle = NULL;
	gh = group->GetReliableCommsHandle();
	sh = session->GetReliableCommsHandle();
	if (gh && sh && gh != sh)
		ComAPIDeleteFromGroup(gh,sh);

	if(gUICommsQ)
		gUICommsQ->Add(_Q_SESSION_REMOVE_,session->Id(),group->Id());

	// VWF 12/1/98: Added this to clean up player's flight when he leaves game
	if ((FalconLocalGame == group) && (FalconLocalGame->IsLocal()))
	{
		Flight
			flight;

		flight = ((FalconSessionEntity*)session)->GetAssignedPlayerFlight();

		if(flight)
		{
			LeaveACSlot (((FalconSessionEntity*)session)->GetAssignedPlayerFlight(), ((FalconSessionEntity*)session)->GetAssignedAircraftNum());
			// Make sure this session doesn't have any old information
		}

	}
	// END VWF

	((FalconSessionEntity*)session)->SetPlayerSquadron (NULL);
	((FalconSessionEntity*)session)->SetPlayerFlight (NULL);
	((FalconSessionEntity*)session)->SetPlayerEntity (NULL);
	((FalconSessionEntity*)session)->SetAircraftNum (255);
	((FalconSessionEntity*)session)->SetPilotSlot (255);
	((FalconSessionEntity*)session)->SetAssignedAircraftNum (255);
	((FalconSessionEntity*)session)->SetAssignedPilotSlot (255);
	((FalconSessionEntity*)session)->SetAssignedPlayerFlight (NULL);

	return 1;
}

// Set up a handle to communicate via UDP with this session.
int VuxSessionConnect(VuSessionEntity *session)
{
	char
		buffer[100];
	int wait_for_connection = 0;
	
	// We only want to connect here during our initial insertion
	if (session->GameAction() != VU_NO_GAME_ACTION)
		return 0;
	
	MonoPrint("Connecting to session: %08x %s\n", session->Id().creator_.value_, ((FalconSessionEntity*)session)->GetPlayerCallsign());
	
	if (session == vuLocalSessionEntity)
	{
		// This is us.. We don't need to connect
		session->SetCommsHandle(NULL);
		session->SetCommsStatus(VU_CONN_INACTIVE);
		session->SetReliableCommsHandle(NULL);
		session->SetReliableCommsStatus(VU_CONN_INACTIVE);
		return 0;
	}

	// UDP connection
	if (!session->GetCommsHandle())
	{
		if (!(FalconConnectionProtocol & FCP_UDP_AVAILABLE) && !(FalconConnectionProtocol & FCP_SERIAL_AVAILABLE))
		{
			// No udp connections available
			session->SetCommsHandle(NULL);
			session->SetCommsStatus(VU_CONN_INACTIVE);
		}
		else if (FalconConnectionType & FCT_SERIAL_AVAILABLE)
		{
			session->SetCommsHandle(NULL);		// We'll inherit from our global group
			session->SetCommsStatus(VU_CONN_ACTIVE);
		}
		else if (FalconConnectionType & FCT_SERVER_AVAILABLE && FalconGlobalUDPHandle)
		{
			// Point us to our server's UDP connection
			session->SetCommsHandle(NULL);		// We'll inherit from our global group
			session->SetCommsStatus(VU_CONN_ACTIVE);
		}
		else if (FalconConnectionType & FCT_BCAST_AVAILABLE)
		{
			// Broadcast is available - just broadcast this to everyone and let them sort it out
			session->SetCommsHandle(NULL);		// We'll inherit from our global group
			session->SetCommsStatus(VU_CONN_ACTIVE);
		}
		else if (FalconConnectionType & FCT_PTOP_AVAILABLE)
		{
			// Point to Point only - Create a new udp connection to this address
			ComAPIHandle	sh = NULL;
			
			if (!RemoveDanglingSession(session))
			{
				// We've not yet established a connection, do so now. (Note: this connection
				// is immediately available for send/receive)
				sprintf (buffer, "%s UDP", ((FalconSessionEntity*)session)->GetPlayerCallsign());
				sh = ComUDPOpen(buffer, F4CommsMaxUDPMessageSize, vuxWorldName, CAPI_UDP_PORT, session->Id().creator_.value_);
				session->SetCommsHandle(sh, F4CommsMaxUDPMessageSize, F4CommsIdealUDPPacketSize);
				if (sh)
					session->SetCommsStatus(VU_CONN_ACTIVE);
				else
					session->SetCommsStatus(VU_CONN_INACTIVE);
			}
		}
		else
		{
			// No communication available - we'll default to group's comms handle though..
			session->SetCommsHandle(NULL);
			session->SetCommsStatus(VU_CONN_INACTIVE);
		}
	}
	
	// Request all global entities as soon as we know how to do so (even unreliably)
	if (session->GetCommsStatus() == VU_CONN_ACTIVE)
	{
		VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,session);
        req->RequestReliableTransmit();
		VuMessageQueue::PostVuMessage(req);
	}
	
	// TCP connection
	if (!session->GetReliableCommsHandle())
	{
		if (!(FalconConnectionProtocol & FCP_TCP_AVAILABLE) && !(FalconConnectionProtocol & FCP_SERIAL_AVAILABLE) && !(FalconConnectionProtocol & FCP_RUDP_AVAILABLE))
		{
			// No reliable connections available
			session->SetReliableCommsHandle(NULL);
			session->SetReliableCommsStatus(VU_CONN_INACTIVE);
		}
		else if (FalconConnectionProtocol & FCP_RUDP_AVAILABLE)
		{
			ComAPIHandle sh = NULL;

			if (!RemoveDanglingSession(session))
			{
				sprintf (buffer, "%s RUDP", ((FalconSessionEntity*)session)->GetPlayerCallsign());
				sh = ComRUDPOpen(buffer, F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, session->Id().creator_.value_, F4CommsIdealPacketSize);

				session->SetReliableCommsHandle(sh, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
				if (sh)
					session->SetReliableCommsStatus(VU_CONN_ACTIVE);
				else
					session->SetReliableCommsStatus(VU_CONN_INACTIVE);
			}
		}
		else if (FalconConnectionType & FCT_SERIAL_AVAILABLE)
		{
			session->SetCommsHandle(NULL);		// We'll inherit from our global group
			session->SetReliableCommsStatus(VU_CONN_ACTIVE);
		}
		else if (FalconConnectionType & FCT_SERVER_AVAILABLE && FalconGlobalTCPHandle)
		{
			// Point us to our server's tcp connection
			session->SetCommsHandle(NULL);		// We'll inherit from our global group
			session->SetReliableCommsStatus(VU_CONN_ACTIVE);
		}
		else if (FalconConnectionType & FCT_PTOP_AVAILABLE)
		{
			// Point to Point only - Open a new TCP socket to this session (will be pending until callback answered)
			ComAPIHandle sh = NULL;
			session->SetReliableCommsStatus(VU_CONN_INACTIVE);		// KCK: Try inactive - then we'll use the UDP if we don't connect
			if (session->Id().creator_.value_ > vuLocalSession.creator_.value_) 
			{
				// Attempt to open a TCP socket
				sh = ComTCPOpenConnect(F4CommsMaxTCPMessageSize, vuxWorldName, CAPI_TCP_PORT, session->Id().creator_.value_, TcpConnectCallback, 15);
				session->SetReliableCommsHandle(sh, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
			} 
			else 
			{
				// Check if the handle is currently dangling
				if (RemoveDanglingSession(session))
				{
					// Request global entities from this guy
					VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,session);
					req->RequestReliableTransmit();
					VuMessageQueue::PostVuMessage(req);
				}
				else
				{
					// Wait for this machine to open a TCP socket with us
					sh = ComTCPOpenAccept(session->Id().creator_.value_, CAPI_TCP_PORT, 15);
					session->SetReliableCommsHandle(sh, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
				}
			}
			wait_for_connection = 1;
		}
		else
		{
			// No reliable group communication available.
			session->SetReliableCommsHandle(NULL);
			session->SetReliableCommsStatus(VU_CONN_INACTIVE);
		}
	}
	
	// Add to the global group
    VuxGroupAddSession(vuGlobalGroup,session);

	{
		VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,session);
		req->RequestReliableTransmit();
		VuMessageQueue::PostVuMessage(req);
	}

	{
		VuMessage *req = new VuFullUpdateEvent (vuLocalSessionEntity, session);
		VuMessageQueue::PostVuMessage(req);
	}

	return 0;
}

static bool test = false;

void VuxSessionDisconnect(VuSessionEntity *session)
	{

	if (session == FalconLocalSession)
		return; 

	// We only want to do this if we're leaving a game (i.e: This session is going away)
	if (session->GameAction() != VU_LEAVE_GAME_ACTION)
		return;


	Flight playerFlight = (Flight)((FalconSessionEntity*)session)->GetAssignedPlayerFlight();
	int acnumber = ((FalconSessionEntity*)session)->GetAssignedAircraftNum();

	if (g_bACPlayerCTDFix && playerFlight && FalconLocalGame->IsLocal()) // only the host...
	{
		FalconPlayerStatusMessage	*msg = new FalconPlayerStatusMessage(((FalconSessionEntity*)session)->Id(), FalconLocalGame);
		SimBaseClass				*playerEntity = (SimBaseClass*) ((FalconSessionEntity*)session)->GetPlayerEntity();

	// first change the deag owner of our disconnected player back to the host
/*	if (playerEntity)
	{
		FalconSimCampMessage	*simmsg = new FalconSimCampMessage (playerEntity->Id(), FalconLocalGame); // target);
		simmsg->dataBlock.from = FalconLocalGame->OwnerId();
		simmsg->dataBlock.message = FalconSimCampMessage::simcampChangeOwner;
		FalconSendMessage(simmsg);
	}
*/
		if (((FalconSessionEntity*)session)->GetPlayerFlight())
		{
			msg->dataBlock.campID		= ((FalconSessionEntity*)session)->GetPlayerFlight()->GetCampID();
		}
		else
		{
			msg->dataBlock.campID		= 0;
		}
	
		if (playerEntity)
		{
			msg->dataBlock.playerID         = playerEntity->Id();
		}
	
		_tcscpy(msg->dataBlock.callsign,((FalconSessionEntity*)session)->GetPlayerCallsign()); 
	
		msg->dataBlock.side             = ((FalconSessionEntity*)session)->GetCountry();
		msg->dataBlock.pilotID          = ((FalconSessionEntity*)session)->GetPilotSlot();
		msg->dataBlock.vehicleID		= ((FalconSessionEntity*)session)->GetAircraftNum();
		msg->dataBlock.state            = PSM_STATE_LEFT_SIM;
	
		FalconSendMessage(msg, TRUE);
		((FalconSessionEntity*)session)->SetFlyState(FLYSTATE_IN_UI);

	// and now we need to do some hacking... the current code doesn't really support to change ownership
	// of single aircraft


	// Get the Aircraftclass entity
		AircraftClass *playerAircraft = NULL;

		if (playerFlight)
			playerAircraft = (AircraftClass *)playerFlight->GetComponentEntity(acnumber);

	// when we still have one, just make local to the host
		if (playerAircraft)
		{
			playerAircraft->MakeLocal();
		}
	}

// 2002-04-14 MN remove the disconnected (CTD'ed, Internet connection lost) player from its slot so it can be occupied again
	LeaveACSlot (((FalconSessionEntity*)session)->GetAssignedPlayerFlight(), ((FalconSessionEntity*)session)->GetAssignedAircraftNum());

	// Remove from the global group
    VuxGroupRemoveSession(vuGlobalGroup,session);

	if (session->GetCommsStatus() != VU_CONN_INACTIVE || session->GetReliableCommsStatus() != VU_CONN_INACTIVE)
	{
		MonoPrint("Disconnecting to session: %s\n",((FalconSessionEntity*)session)->GetPlayerCallsign());
	}

/* me123 commented this out
it prevents us from removing the session from other lists then the global group.
it should not matter since the sessino will be removed totaly right after this.	
// Disconnect UDP
	ch = session->GetCommsHandle();
	session->SetCommsHandle(NULL);
	session->SetCommsStatus(VU_CONN_INACTIVE);
	if (session && ch && ch != FalconGlobalUDPHandle) 
		ComAPIClose(ch);
	// Disconnect TCP
	ch = session->GetReliableCommsHandle();
	session->SetReliableCommsHandle(NULL);
	session->SetReliableCommsStatus(VU_CONN_INACTIVE);
	if (session && ch && ch != FalconGlobalTCPHandle)
		ComAPIClose(ch);
		*/
	}

// =====================================
// Dangling session stuff
// =====================================

void InitDanglingList (void)
	{
	VuEnterCriticalSection();
	VuSessionFilter		DanglingSessionFilter(FalconNullId);
	DanglingSessionsList = new FalconPrivateList(&DanglingSessionFilter);
	// KCK: No reason to Init() this list. We're never going to insert a real entity into it.
	VuExitCriticalSection();
	}

void CleanupDanglingList (void)
	{
	ComAPIHandle	ch;

	VuEnterCriticalSection();
	if (DanglingSessionsList)
		{
		VuListIterator		dsit(DanglingSessionsList);
		VuSessionEntity		*session;

		for (session = (VuSessionEntity*)dsit.GetFirst(); session; session = (VuSessionEntity*)dsit.GetNext())
			{
			DanglingSessionsList->Remove(session);
			ch = session->GetCommsHandle();
			if (ch)
				ComAPIClose(ch);
			ch = session->GetReliableCommsHandle();
			if (ch)
				ComAPIClose(ch);
			MonoPrint("Removing dangling session for 0x%x.\n",session->OwnerId().creator_.value_);
			delete session;
			}
		delete DanglingSessionsList;
		}
	DanglingSessionsList = NULL;
	VuExitCriticalSection();
	}

void AddDanglingSession (ComAPIHandle ch1, ComAPIHandle ch2, ulong ipAddress)
	{
	VuEnterCriticalSection();
	FalconSessionEntity	*tempSess = NULL;

	if (!DanglingSessionsList)
		InitDanglingList();

	// Check if it's already in our list
	VuListIterator			dsit(DanglingSessionsList);
	FalconSessionEntity		*session;

	for (session = (FalconSessionEntity*)dsit.GetFirst(); session; session = (FalconSessionEntity*)dsit.GetNext())
		{
		if (session->OwnerId().creator_.value_ == ipAddress)
			{
			tempSess = session;
			break;
			}
		}

	if (!tempSess)
		{
		tempSess = new FalconSessionEntity(0xffffffff, "tmp");
		// KCK: Mucking with the ID isn't a big deal if we never insert this entity
		VU_ID		tempId;
		tempId.creator_.value_ = ipAddress;
		tempSess->SetOwnerId(tempId);
		// KCK: Hackish. Need to trick VU into thinking the memory is active, otherwise
		// it won't insert it into our list.
		tempSess->SetVuStateAccess(VU_MEM_ACTIVE);
		DanglingSessionsList->ForcedInsert(tempSess);
		}

	if (ch1)
		{
		MonoPrint("Adding dangling UDP handle for 0x%x.\n",ipAddress);
		tempSess->SetCommsHandle(ch1, F4CommsMaxUDPMessageSize, F4CommsIdealUDPPacketSize);
		tempSess->SetCommsStatus(VU_CONN_ACTIVE);
		}
	if (ch2)
		{
		MonoPrint("Adding dangling TCP handle for 0x%x.\n",ipAddress);
		tempSess->SetReliableCommsHandle(ch2, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
		tempSess->SetReliableCommsStatus(VU_CONN_ACTIVE);
		}
	VuExitCriticalSection();
	}

int RemoveDanglingSession (VuSessionEntity *newSess)
	{
	int		retval = 0;
	char
		buffer[100];

	if (!DanglingSessionsList)
		return retval;

	VuEnterCriticalSection();
	VuListIterator		dsit(DanglingSessionsList);
	VuSessionEntity		*session;

	for (session = (VuSessionEntity*)dsit.GetFirst(); session; session = (VuSessionEntity*)dsit.GetNext())
		{
		if ((newSess->IsSession() && newSess->Id().creator_.value_ == session->OwnerId().creator_.value_) || (session->OwnerId().creator_.value_ == 0))
			{
			if (!newSess->GetCommsHandle() && session->GetCommsHandle())
				{
				MonoPrint("Attaching dangling UDP handle for 0x%x.\n",session->OwnerId().creator_.value_);
				newSess->SetCommsHandle(session->GetCommsHandle(), F4CommsMaxUDPMessageSize, F4CommsIdealUDPPacketSize);
				sprintf (buffer, "%s UDP", ((FalconSessionEntity*)newSess)->GetPlayerCallsign());
				ComAPISetName (session->GetCommsHandle(), buffer);
				newSess->SetCommsStatus(session->GetCommsStatus());
				}
			if (!newSess->GetReliableCommsHandle() && session->GetReliableCommsHandle())
				{
				MonoPrint("Attaching dangling TCP handle for 0x%x.\n",session->OwnerId().creator_.value_);
				newSess->SetReliableCommsHandle(session->GetReliableCommsHandle(), F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
				sprintf (buffer, "%s RUDP", ((FalconSessionEntity*)newSess)->GetPlayerCallsign());
				ComAPISetName (session->GetCommsHandle(), buffer);
				newSess->SetReliableCommsStatus(session->GetReliableCommsStatus());
				}
			MonoPrint("Removing dangling session for 0x%x.\n",session->OwnerId().creator_.value_);
			DanglingSessionsList->Remove(session);
			delete session;
			retval = 1;
			}
		}
	VuExitCriticalSection();

	if (!retval)
		MonoPrint ("No dangling handles found.\n");

	return retval;
	}

int UpdateDanglingSessions (void)
{
	if (gConnectionStatus)
	{
		VuEnterCriticalSection();
		VuListIterator		dsit(DanglingSessionsList);
		VuSessionEntity		*session;
		int					count = 0;

		for (session = (VuSessionEntity*)dsit.GetFirst(); session; session = (VuSessionEntity*)dsit.GetNext())
		{
			count += session->GetMessages();
			// attempt to send one packet of each type
			session->FlushOutboundMessageBuffer();
		}

		VuExitCriticalSection();

		return count;
	}
	else
	{
		return 0;
	}
}

// =====================================
// COMMS callbacks (registering handles)
// =====================================

// Callback from comms to register a tcp handle
// This gets called when someone sent a message to our TCP listen socket
void TcpAcceptCallback(ComAPIHandle ch)
	{
	ulong				ipaddr;
	VuSessionEntity*	s;
	VuSessionsIterator	siter(vuGlobalGroup);

	// need to find session...
	ipaddr = ComAPIQuery(ch, COMAPI_CONNECTION_ADDRESS);

	// map ipaddr to session
	for (s = siter.GetFirst(); s; s = siter.GetNext()) 
		{
		if (s->Id().creator_.value_ == ipaddr) 
			{
			MonoPrint("TcpAcceptCallback connection -- made to 0x%x ch = %p\n", ipaddr, ch);
			s->SetReliableCommsHandle(ch, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
			s->SetReliableCommsStatus(VU_CONN_ACTIVE);
			// Request global entities from this guy
			VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,s);
			req->RequestReliableTransmit();
			VuMessageQueue::PostVuMessage(req);
			return;
			}
		}
	MonoPrint("TcpAcceptCallback error -- couldn't find session 0x%x\n", ipaddr);
	// should we disconnect here?
	}

// This gets called as a result of us calling ComTCPOpenConnect (attempting to connect TCP)
// ret == 0 means success
void TcpConnectCallback(ComAPIHandle ch, int ret)
	{
	ulong				ipaddr;

	ipaddr = ComAPIQuery(ch, COMAPI_CONNECTION_ADDRESS);

	MonoPrint("Calling TcpConnectCallback.\n");

   if (ret != 0)
   {
      MonoPrint ("RequestConnection failed %d, will retry\n", ret);
      return;
   }

	// Check if we're connecting to a server first...
	if (FalconConnectionType == FCT_SERVER_AVAILABLE)
		{
		FalconGlobalTCPHandle = ch;
		if (ch)
			{
			FalconServerTCPStatus = VU_CONN_ACTIVE;
			if (gConnectionStatus == F4COMMS_PENDING)
				F4CommsConnectionCallback(F4COMMS_CONNECTED);
			}
		else
			{
			FalconServerTCPStatus = VU_CONN_INACTIVE;	// This would be bad..
			F4CommsConnectionCallback(F4COMMS_ERROR_COULDNT_CONNECT_TO_SERVER);
			}
		return;
		}

	// Otherwise, look for the correct session
	VuEnterCriticalSection();
	VuSessionEntity*	s;
	VuSessionsIterator	siter(vuGlobalGroup);
	for (s = siter.GetFirst(); s; s = siter.GetNext()) 
		{
		if (s->Id().creator_.value_ == ipaddr) 
			{
			RemoveDanglingSession(s);
			if (ch && s->GetCommsHandle() == ch)
				s->SetCommsStatus(VU_CONN_ACTIVE);
			if (ch && s->GetReliableCommsHandle() == ch)
				s->SetReliableCommsStatus(VU_CONN_ACTIVE);
			VuExitCriticalSection();
			if (gConnectionStatus == F4COMMS_PENDING)
				F4CommsConnectionCallback(F4COMMS_CONNECTED);
			// Request global entities from this guy
			VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,s);
			req->RequestReliableTransmit();
			VuMessageQueue::PostVuMessage(req);
			return;
			}
		}

	// We're connected (although we're not yet ready to send)
	if (gConnectionStatus == F4COMMS_PENDING)
		F4CommsConnectionCallback(F4COMMS_CONNECTED);

	// Add this connection to our "dangling connection" list
	AddDanglingSession(NULL, ch, ipaddr);
	VuExitCriticalSection();
	}

// This gets called as a result of us calling ComDPLAYOpen and getting a modem connection
// ret == 0 means success
void ModemConnectCallback(ComAPIHandle ch, int ret)
	{
	ulong				ipaddr;

	ipaddr = ComAPIQuery(ch, COMAPI_CONNECTION_ADDRESS);

	ShiAssert ( ch == FalconGlobalUDPHandle );		// We should only have one connection!

	// need to find session... There should be only two (us and them)
	VuEnterCriticalSection();
	VuSessionEntity*	s;
	VuSessionsIterator	siter(vuGlobalGroup);
	for (s = siter.GetFirst(); s; s = siter.GetNext()) 
		{
		if (s != FalconLocalSession) 
			{
			if (ret == 0) 
				{
				MonoPrint("ModemConnectCallback invoked: connected!\n");
				s->SetReliableCommsHandle(ch, F4CommsMaxTCPMessageSize, F4CommsIdealTCPPacketSize);
				s->SetReliableCommsStatus(VU_CONN_ACTIVE);
				// Request global entities from this guy
				VuMessage *req = new VuGetRequest(VU_GET_GLOBAL_ENTS,s);
				req->RequestReliableTransmit();
				VuMessageQueue::PostVuMessage(req);
				if (gConnectionStatus == F4COMMS_PENDING)
					F4CommsConnectionCallback(F4COMMS_CONNECTED);
				}
			else 
				{
				MonoPrint("ModemConnectCallback invoked: bad connection.\n");
				s->SetReliableCommsHandle(NULL);
				s->SetReliableCommsStatus(VU_CONN_INACTIVE);
				}
			VuExitCriticalSection();
			return;
			}
		}
	if (gConnectionStatus == F4COMMS_PENDING)
		F4CommsConnectionCallback(F4COMMS_CONNECTED);
	// Add this connection to our "dangling connection" list
	AddDanglingSession(NULL, ch,  ipaddr);
	MonoPrint("ModemConnectCallback invoked: saving handle..\n");
	VuExitCriticalSection();
	}

// ====================================
// Time synchonizing stuff
// ====================================

// This routine will determine the proper time compression to run at after polling
// all sessions in the game.

//why are we taking force as a parameter when we aren't using it?
void ResyncTimes (int force)
{
	VuEnterCriticalSection ();

	VuSessionsIterator	sit(FalconLocalGame);
	FalconSessionEntity *session;
	int
		count,
		best_comp;

	best_comp = 1;
	
	if (gTimeModeServer||g_bServer)
	{
		session = (FalconSessionEntity*)sit.GetFirst();
		count = 0;

		while (session)
		{
			if (!session->IsLocal ())
			{
				count ++;
				best_comp = session->GetReqCompression();
				break;
			}

			session = (FalconSessionEntity*)sit.GetNext();
		}

		if (count == 0)
		{
			best_comp = FalconLocalSession->GetReqCompression();
		}
	}
	else
	{
		best_comp = FalconLocalSession->GetReqCompression();
	}
	
	// KCK: Currently, everyone polls locally, but only sets time compression upon
	// notice from the host. We may prefer to send the host's poll data with the timing
	// message.
	remoteCompressionRequests = 0;

	session = (FalconSessionEntity*)sit.GetFirst();
	while (session)
	{
		if (gTimeModeServer||g_bServer)
		{
			if (session->IsLocal ())
			{
				session = (FalconSessionEntity *) sit.GetNext ();

				continue;
			}
		}

		if (session->GetReqCompression() > 1 && session->GetReqCompression() < best_comp)
		{
			best_comp = session->GetReqCompression();
			remoteCompressionRequests |= 1 << (best_comp-1);
		}
		
		if (session->GetReqCompression() < 1 && session->GetReqCompression() > best_comp)
		{
			best_comp = session->GetReqCompression();
			remoteCompressionRequests |= REMOTE_REQUEST_PAUSE;
		}
		
		if (session->GetReqCompression() == 1)
			best_comp = session->GetReqCompression();
		
		if (session->GetReqCompression() == 0 && best_comp > 1)
			best_comp = 1;
		
		session = (FalconSessionEntity*)sit.GetNext();
	}
	
	VuExitCriticalSection ();
	
	if (FalconLocalGame && FalconLocalGame->IsLocal())
	{
		lastStartTime = vuxRealTime;
		
		// Let the Campaign adjust itself to the target compression ratio
		targetCompressionRatio = best_comp;
		
//		MonoPrint ("Sending Timing Message %08x %d\n", vuxGameTime, targetCompressionRatio);
		FalconTimingMessage	*msg = new FalconTimingMessage(FalconNullId,FalconLocalGame);
		msg->RequestOutOfBandTransmit();
		FalconSendMessage(msg,TRUE);
	}
	force;
}

// Time stamp function used by Comms and Vu to try and reduce latency in time stamping
ulong TimeStampFunction (void)
	{
	return vuxRealTime;
	}

void VuxAdjustLatency(VU_TIME, VU_TIME)
	{
	// KCK NOTE: This is stubbed 'cause we're not using VU's latency checking
//	SynchronizeTime(vuLocalSessionEntity->Group());
//	MonoPrint("Called VuxAdjustLatency(%d, %d)\n", t1, t2);
	}

void set_bandwidth_limit (int bd)
{
	/*switch (bd)
	{
		case F4_BANDWIDTH_14:
		{
			MonoPrint ("Setting Bandwidth 1000\n");
			ComAPISetBandwidth (1000);
			vuxDriverSettings->globalPosTolMod_ = 0.2F;
			vuxDriverSettings->globalAngleTolMod_ = 0.2F;
			break;
		}

		case F4_BANDWIDTH_28:
		{
			MonoPrint ("Setting Bandwidth 2000\n");
			ComAPISetBandwidth (2000);
			vuxDriverSettings->globalPosTolMod_ = 0.1F;
			vuxDriverSettings->globalAngleTolMod_ = 0.1F;
			break;
		}
	
		case F4_BANDWIDTH_33:
		{
			MonoPrint ("Setting Bandwidth 2400\n");
			ComAPISetBandwidth (2400);
			vuxDriverSettings->globalPosTolMod_ = 0.091F;
			vuxDriverSettings->globalAngleTolMod_ = 0.091F;
			break;
		}

		case F4_BANDWIDTH_56Modem:
		{
			MonoPrint ("Setting Bandwidth 2000\n");
			ComAPISetBandwidth (2000);
			vuxDriverSettings->globalPosTolMod_ = 0.051F;
			vuxDriverSettings->globalAngleTolMod_ = 0.051F;
			break;
		}

		case F4_BANDWIDTH_56ISDN:
		{
			MonoPrint ("Setting Bandwidth 3900\n");
			ComAPISetBandwidth (3900);
			vuxDriverSettings->globalPosTolMod_ = 0.051F;
			vuxDriverSettings->globalAngleTolMod_ = 0.051F;
			break;
		}

		case F4_BANDWIDTH_112:
		{
			MonoPrint ("Setting Bandwidth 7800\n");
			ComAPISetBandwidth (7800);
			vuxDriverSettings->globalPosTolMod_ = 0.026F;
			vuxDriverSettings->globalAngleTolMod_ = 0.026F;
			break;
		}

		case F4_BANDWIDTH_256:
		{
			MonoPrint ("Setting Bandwidth 15600\n");
			ComAPISetBandwidth (15600);
			vuxDriverSettings->globalPosTolMod_ = 0.013F;
			vuxDriverSettings->globalAngleTolMod_ = 0.013F;
			break;
		}

		case F4_BANDWIDTH_T1:
		{
			MonoPrint ("Setting Bandwidth 31200\n");
			ComAPISetBandwidth (31200);
			vuxDriverSettings->globalPosTolMod_ = 0.006F;
			vuxDriverSettings->globalAngleTolMod_ = 0.006F;
			break;
		}
	}


*/
//	removet to revert back to oldmp code 
	if (F4CommsBandwidth)
	{
			vuLocalSessionEntity->SetBandWidth((F4CommsBandwidth/8*1000));//me123
			ComAPISetBandwidth (3300);
	}

	else
switch (bd)
	{
		case F4_BANDWIDTH_14:
		{
			vuLocalSessionEntity->SetBandWidth(1800);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_28:
		{
			vuLocalSessionEntity->SetBandWidth(3600);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}
	
		case F4_BANDWIDTH_33:
		{
			vuLocalSessionEntity->SetBandWidth(4200);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_56Modem:
		{
			vuLocalSessionEntity->SetBandWidth(4200);//Unz & Booster (56 is same as 33.6 modem)
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_56ISDN:
		{
			vuLocalSessionEntity->SetBandWidth(7000);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_112:
		{
			vuLocalSessionEntity->SetBandWidth(14000);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_256:
		{
			vuLocalSessionEntity->SetBandWidth(32000);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}

		case F4_BANDWIDTH_T1:
		{
			vuLocalSessionEntity->SetBandWidth(187500);//Unz & Booster
			ComAPISetBandwidth (3300);
			break;
		}
	}
	
}
