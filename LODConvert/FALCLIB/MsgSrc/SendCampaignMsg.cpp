/*
 * Machine Generated source file for message "Send Campaign".
 * NOTE: The functions here must be completed by hand.
 * Generated on 30-October-1996 at 19:32:10
 * Generated from file EVENTS.XLS by KEVINK
 */

#include "MsgInc\SendCampaignMsg.h"
#include "mesg.h"
#include "CmpClass.h"
#include "FalcSess.h"
#include "MsgInc\RequestCampaignData.h"
#include "Campaign.h"
#include "Weather.h"
#include "ui\include\falcuser.h"
#include "ui95\chandler.h"

extern C_Handler *gMainHandler;
extern int gCurrentDataVersion;
extern int gCampDataVersion;

extern void CampaignJoinKeepAlive (void);

FalconSendCampaign::FalconSendCampaign(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendCampaignMsg, FalconEvent::CampaignThread, entityId, target, loopback)
	{
	uchar	*buffer;

	dataBlock.dataSize = (short)TheCampaign.Encode(&buffer);
	dataBlock.campInfo = buffer;
	}

FalconSendCampaign::FalconSendCampaign(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendCampaignMsg, FalconEvent::CampaignThread, senderid, target)
	{
	dataBlock.campInfo = NULL;
	dataBlock.dataSize = -1;
	type;
	}

FalconSendCampaign::~FalconSendCampaign(void)
	{
	delete dataBlock.campInfo;
	dataBlock.campInfo = NULL;
	}

int FalconSendCampaign::Size (void)
	{ 
	ShiAssert ( dataBlock.dataSize >= 0 );

	return sizeof(dataBlock) + dataBlock.dataSize + FalconEvent::Size();
	}

int FalconSendCampaign::Decode (VU_BYTE **buf, int length)
	{
	int size = 0;

	ShiAssert ( dataBlock.dataSize >= 0 );
	memcpy (&dataBlock, *buf, sizeof (dataBlock));				*buf += sizeof (dataBlock);		size += sizeof (dataBlock);
	dataBlock.campInfo = new uchar[dataBlock.dataSize];
	memcpy (dataBlock.campInfo, *buf, dataBlock.dataSize);		*buf += dataBlock.dataSize;		size += dataBlock.dataSize;
	size += FalconEvent::Decode (buf, length);

	ShiAssert (size == Size());

	CampaignJoinKeepAlive();

	return size;
	};

int FalconSendCampaign::Encode (VU_BYTE **buf)
	{
	int size = 0;

	memcpy (*buf, &dataBlock, sizeof (dataBlock));				*buf += sizeof (dataBlock);		size += sizeof (dataBlock);
	memcpy (*buf, dataBlock.campInfo, dataBlock.dataSize);		*buf += dataBlock.dataSize;		size += dataBlock.dataSize;
	ShiAssert ( dataBlock.dataSize >= 0 );
	size += FalconEvent::Encode (buf);

	ShiAssert (size == Size());

	return size;
	};

int FalconSendCampaign::Process(uchar autodisp)
{
	uchar		*bufptr = dataBlock.campInfo;

	if (autodisp)
		return 0;

	if (TheCampaign.Flags & CAMP_NEED_PRELOAD)
	{
		CampEnterCriticalSection();
		gCampDataVersion = gCurrentDataVersion;
		TheCampaign.Decode(&bufptr);
		TheCampaign.Flags &= ~CAMP_NEED_PRELOAD;
		TheCampaign.Flags |= CAMP_PRELOADED;
		CampLeaveCriticalSection();

		if(gMainHandler)
			PostMessage(gMainHandler->GetAppWnd(),FM_GOT_CAMPAIGN_DATA,CAMP_NEED_PRELOAD,0);

		return 1;
	}
	else
	{
		return 0;
	}
}
