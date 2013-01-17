/*
 * Machine Generated source file for message "Send Persistant List".
 * NOTE: The functions here must be completed by hand.
 * Generated on 29-July-1997 at 17:49:00
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#include "MsgInc\SendPersistantList.h"
#include "CmpClass.h"
#include "Persist.h"
#include "mesg.h"

extern void CampaignJoinKeepAlive (void);

FalconSendPersistantList::FalconSendPersistantList(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendPersistantList, FalconEvent::CampaignThread, entityId, target, loopback)
	{
	dataBlock.data = NULL;
	dataBlock.size = -1;
	}

FalconSendPersistantList::FalconSendPersistantList(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendPersistantList, FalconEvent::CampaignThread, senderid, target)
	{
	dataBlock.data = NULL;
	dataBlock.size = -1;
	type;
	}

FalconSendPersistantList::~FalconSendPersistantList(void)
	{
	if (dataBlock.data)
		delete dataBlock.data;
	dataBlock.data = NULL;
	}
      
int FalconSendPersistantList::Size (void) 
	{ 
	ShiAssert ( dataBlock.size  >= 0 );

	return sizeof (short) + dataBlock.size + FalconEvent::Size();
	}
	  
int FalconSendPersistantList::Decode (VU_BYTE **buf, int length)
	{
	int size = 0;

	memcpy (&dataBlock.size, *buf, sizeof (short));		*buf += sizeof (short);		size += sizeof (short);		
    ShiAssert(dataBlock.size >= 0);
	dataBlock.data = new VU_BYTE[dataBlock.size];
	memcpy (dataBlock.data, *buf, dataBlock.size);		*buf += dataBlock.size;		size += dataBlock.size;		
	size += FalconEvent::Decode (buf, length);

	ShiAssert (size == Size());

	CampaignJoinKeepAlive();

	return size;
	}

int FalconSendPersistantList::Encode (VU_BYTE **buf)
	{
	int size = 0;

    ShiAssert(dataBlock.size >= 0);
	memcpy (*buf, &dataBlock.size, sizeof (short));		*buf += sizeof (short);		size += sizeof (short);		
	memcpy (*buf, dataBlock.data, dataBlock.size);		*buf += dataBlock.size;		size += dataBlock.size;		
	size += FalconEvent::Encode (buf);

	ShiAssert (size == Size());

	return size;
	}

int FalconSendPersistantList::Process(uchar autodisp)
{
	VU_BYTE*	buf;

	if (autodisp || !TheCampaign.IsPreLoaded())
		return -1;

	if (TheCampaign.Flags & CAMP_NEED_PERSIST)
	{
		CampaignJoinKeepAlive();

		buf = (VU_BYTE*) dataBlock.data;
		DecodePersistantList(&buf);
		TheCampaign.Flags &= ~CAMP_NEED_PERSIST;
		if(gMainHandler)
			PostMessage(gMainHandler->GetAppWnd(),FM_GOT_CAMPAIGN_DATA,CAMP_NEED_PERSIST,0);
		TheCampaign.GotJoinData();
	}

	return 0;
}

