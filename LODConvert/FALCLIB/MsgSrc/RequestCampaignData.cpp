/*
 * Machine Generated source file for message "Request Campaign Data".
 * NOTE: The functions here must be completed by hand.
 * Generated on 30-July-1997 at 10:45:40
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#include "MsgInc\RequestCampaignData.h"
#include "MsgInc\SendObjData.h"
#include "MsgInc\SendUnitData.h"
#include "MsgInc\SendVCMsg.h"
#include "mesg.h"
#include "Campaign.h"
#include "dogfight.h"

extern ulong gResendEvalRequestTime;
extern int EncodeObjectiveDeltas(VU_BYTE **stream);
extern int DecodeObjectiveDeltas(VU_BYTE **stream);
extern void SendVCData (FalconSessionEntity *);
extern void ResyncTimes (void);
extern int CheckNumberPlayers (void);
extern void RequestEvalData (void);
extern void SendPrimaryObjectiveList (uchar teammask);

static int MatchPlayStarted (void);

FalconRequestCampaignData *gRequestQueue = NULL;

FalconRequestCampaignData::FalconRequestCampaignData(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (RequestCampaignData, FalconEvent::CampaignThread, entityId, target, loopback)
	{
	RequestOutOfBandTransmit();
	nextRequest = 0;
	dataBlock.size = 0;
	dataBlock.data = NULL;
	}

FalconRequestCampaignData::FalconRequestCampaignData(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (RequestCampaignData, FalconEvent::CampaignThread, senderid, target)
	{
	nextRequest = 0;
	dataBlock.size = 0;
	dataBlock.data = NULL;
	type;
	}

FalconRequestCampaignData::~FalconRequestCampaignData(void)
	{
	if (dataBlock.data)
		delete dataBlock.data;
	dataBlock.data = NULL;
	}

int FalconRequestCampaignData::Size (void) 
	{ 
	ShiAssert ( dataBlock.size >= 0 );

	return	FalconEvent::Size() +
			sizeof(VU_ID) +
			sizeof(ulong) +
			sizeof(uchar) +
			dataBlock.size;
	}

int FalconRequestCampaignData::Decode (VU_BYTE **buf, int length)
	{
	int size = 0;

	size += FalconEvent::Decode (buf, length);

	memcpy (&dataBlock.who, *buf, sizeof(VU_ID));				*buf += sizeof(VU_ID);		size += sizeof(VU_ID);
	memcpy (&dataBlock.dataNeeded, *buf, sizeof(ulong));		*buf += sizeof(ulong);		size += sizeof(ulong);
	memcpy (&dataBlock.size, *buf, sizeof(uchar));				*buf += sizeof(uchar);		size += sizeof(uchar);
	ShiAssert ( dataBlock.size >= 0 );
	if (dataBlock.size > 0)
		{
		dataBlock.data = new uchar[dataBlock.size];
		memcpy (dataBlock.data, *buf, dataBlock.size);			*buf += dataBlock.size;		size += dataBlock.size;
		}

	ShiAssert ( size == Size() );

	return size;
	}

int FalconRequestCampaignData::Encode (VU_BYTE **buf)
	{
	int size = 0;

	size += FalconEvent::Encode (buf);

	ShiAssert ( dataBlock.size >= 0 );
	memcpy (*buf, &dataBlock.who, sizeof(VU_ID));				*buf += sizeof(VU_ID);		size += sizeof(VU_ID);
	memcpy (*buf, &dataBlock.dataNeeded, sizeof(ulong));		*buf += sizeof(ulong);		size += sizeof(ulong);
	memcpy (*buf, &dataBlock.size, sizeof(uchar));				*buf += sizeof(uchar);		size += sizeof(uchar);
	if (dataBlock.size > 0 && dataBlock.data)
		{
		memcpy (*buf, dataBlock.data, dataBlock.size);			*buf += dataBlock.size;		size += dataBlock.size;
		}

	ShiAssert ( size == Size() );

	return size;
	}

int FalconRequestCampaignData::Process(uchar autodisp)
	{
	if (autodisp)
		return -1;

	MonoPrint ("Process %08x\n", dataBlock.dataNeeded);

	if (dataBlock.dataNeeded & CAMP_GAME_FULL)
	{
		MonoPrint ("Bang Crash Whollop\n");

		if(gMainHandler)
			PostMessage(gMainHandler->GetAppWnd(),FM_GAME_FULL,0,0);
	}

	if (dataBlock.dataNeeded & DF_MATCH_IN_PROGRESS)
	{
		MonoPrint ("Whollop Crash Bang\n");

		if(gMainHandler)
			PostMessage(gMainHandler->GetAppWnd(),FM_MATCH_IN_PROGRESS,0,0);
	}

	if (!TheCampaign.IsLoaded())
		return -1;

	// KCK TODO: Check if a request from this machine is already on the queue,
	// and toss, if so.

	// Reference the message and put it on our request list.
	Ref();
	
	nextRequest = gRequestQueue;
	gRequestQueue = this;

	return 1;
	}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ===============================================================================
// This function is called from the campaign thread, and actually does the sending
// ===============================================================================

void SendRequestedData (void)
{
	FalconRequestCampaignData *request = gRequestQueue;
	int	sent=0;

	CampEnterCriticalSection();

	while (request)
	{
		VU_BYTE*			buf;
		uchar				*dataptr = request->dataBlock.data;
		FalconSessionEntity	*requester = (FalconSessionEntity*)vuDatabase->Find(request->dataBlock.who);
		
		if (requester && TheCampaign.IsLoaded())
		{
			TheCampaign.SetOnlineStatus(1);

			if ((!(request->dataBlock.dataNeeded & CAMP_NEED_PRELOAD)) && (CheckNumberPlayers () < 0))
			{
				FalconRequestCampaignData
					*msg;

				MonoPrint ("Too Many Players");

				msg = new FalconRequestCampaignData(requester->Id(), requester);
				msg->dataBlock.who = vuLocalSessionEntity->Id();
				msg->dataBlock.dataNeeded = CAMP_GAME_FULL;
				FalconSendMessage (msg,TRUE);
			}
			else if ((!(request->dataBlock.dataNeeded & CAMP_NEED_PRELOAD)) && (MatchPlayStarted()))
			{
				FalconRequestCampaignData
					*msg;

				MonoPrint ("Send Match Play In Progress");

				msg = new FalconRequestCampaignData(requester->Id(), requester);
				msg->dataBlock.who = vuLocalSessionEntity->Id();
				msg->dataBlock.dataNeeded = DF_MATCH_IN_PROGRESS;
				FalconSendMessage (msg,TRUE);
			}
			else
			{
				MonoPrint ("Data Needed %08x\n", request->dataBlock.dataNeeded);

				// Send back what was requested:
				if (request->dataBlock.dataNeeded & CAMP_NEED_PRELOAD)
				{
					MonoPrint ("Sending Preload\n");
					FalconSendCampaign*	msg = new FalconSendCampaign(request->dataBlock.who,requester);
					msg->dataBlock.campTime = Camp_GetCurrentTime();
					msg->dataBlock.from = vuLocalSessionEntity->Id();
					msg->RequestOutOfBandTransmit ();
					FalconSendMessage(msg,TRUE);
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_WEATHER)
				{
					MonoPrint ("Sending Weather\n");
					((WeatherClass*)realWeather)->SendWeather(requester);
				}

				if (request->dataBlock.dataNeeded & CAMP_NEED_PERSIST)
				{
					MonoPrint ("Sending Persist\n");
					FalconSendPersistantList*	msg = new FalconSendPersistantList(request->dataBlock.who,requester);
					int							maxSize = F4VuMaxTCPMessageSize - sizeof(class FalconSendPersistantList);
					msg->dataBlock.size = (short)SizePersistantList(maxSize);
					msg->dataBlock.data = new VU_BYTE[msg->dataBlock.size];
					buf = (VU_BYTE*) msg->dataBlock.data;
					EncodePersistantList(&buf, maxSize);
					FalconSendMessage(msg,TRUE);
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_OBJ_DELTAS)
				{
					MonoPrint ("Sending Obj Deltas\n");
					SendObjectiveDeltas (requester,requester,dataptr);
					if (dataptr)
						dataptr += FS_MAXBLK/8;
				}
				else if (requester->objDataSendBuffer)
				{
					MonoPrint ("Sending Obj Data\n");
					delete requester->objDataSendBuffer;
					requester->objDataSendBuffer = NULL;
					requester->objDataSendSet = 0;
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_UNIT_DATA)
				{
					MonoPrint ("Sending Unit Data\n");
					SendCampaignUnitData (requester,requester,dataptr);
					if (dataptr)
						dataptr += FS_MAXBLK/8;
				}
				else if (requester->unitDataSendBuffer)
				{
					MonoPrint ("Sending Unit Data\n");
					delete requester->unitDataSendBuffer;
					requester->unitDataSendBuffer = NULL;
					requester->unitDataSendSet = 0;
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_ENTITIES && request->dataBlock.who != vuLocalSession)
				{
					MonoPrint ("Sending Entity Data\n");
					// KCK: I don't think there's anything we need here -
					// Non-weapon sim entities are sent with the campaign data.
					// We could send weapons, I suppose...
					//
	/*				// We really only want to send owned non campaign entities
					VuMessage *resp = 0;
					VuEntity *ent = NULL;
					VuDatabaseIterator iter;
					ent = iter.GetFirst();
					while (ent) 
						{
						if (!ent->IsPrivate() && ent->IsLocal() && (ent->EntityType())->classInfo_[VU_CLASS] != CLASS_OBJECTIVE && (ent->EntityType())->classInfo_[VU_CLASS] != CLASS_UNIT && (ent->EntityType())->classInfo_[VU_CLASS] != CLASS_MANAGER) 
							{
							resp = new VuFullUpdateEvent(ent, requester);
							resp->RequestReliableTransmit();
							VuMessageQueue::PostVuMessage(resp);
							}
						ent = iter.GetNext();
						}
	*/
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_TEAM_DATA && request->dataBlock.who != vuLocalSession)
				{
					MonoPrint ("Sending Team Data\n");
					// We really only want to send manager entities && team entities
					for (int t=0; t<NUM_TEAMS; t++)
					{
						if (TeamInfo[t] && TeamInfo[t]->IsLocal())
							TeamInfo[t]->DoFullUpdate(requester);
					}
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_VC && request->dataBlock.who != vuLocalSession)
				{
					MonoPrint ("Sending VC Data\n");
					SendVCData (requester);
				}
				if (request->dataBlock.dataNeeded & CAMP_NEED_PRIORITIES)
					{
					MonoPrint ("Sending Priorities\n");
					// Send priorities for all teams
					SendPrimaryObjectiveList(0);
					}
			}
		}

		sent++;

		gRequestQueue = request->nextRequest;
		request->UnRef();
		request = gRequestQueue;
	}

	if (sent)
	{
		// Force time resync, just for good measure.
		ResyncTimes(TRUE);
	}

	// Resend any pending eval requests
	if (gResendEvalRequestTime > vuxRealTime)
	{
		RequestEvalData();
		gResendEvalRequestTime = 0;
	}

	CampLeaveCriticalSection();
}

// Check if this is a Dogfight game and match play is in progress
int MatchPlayStarted (void)
{
int retval = FALSE;

   if (FalconLocalGame->GetGameType() == game_Dogfight)
   {
      if (SimDogfight.GetGameType() == dog_TeamMatchplay)
      {
         if (SimDogfight.GameStarted())
         {
            retval = TRUE;
         }
      }
   }

   return retval;
}