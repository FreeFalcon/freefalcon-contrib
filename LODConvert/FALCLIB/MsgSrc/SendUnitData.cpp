/*
 * Machine Generated source file for message "Send Unit Data".
 * NOTE: The functions here must be completed by hand.
 * Generated on 24-November-1997 at 21:03:10
 * Generated from file EVENTS.XLS by MicroProse
 */

#include "MsgInc\SendUnitData.h"
#include "mesg.h"
#include "F4Comms.h"

#define	DEBUG_STARTUP			1

extern void CampaignJoinKeepAlive (void);
extern uchar	gCampJoinTries;

// Maximum size data block we can send per message
// Since this needs an instance of the message to really be sized correctly,
// I wait and calculate it the first chance I get.
ulong	gUnitBlockSize = 0;

FalconSendUnitData::FalconSendUnitData(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendUnitData, FalconEvent::CampaignThread, entityId, target, loopback)
	{
	RequestReliableTransmit();
	dataBlock.unitData = NULL;
	dataBlock.size = 0;
	}

FalconSendUnitData::FalconSendUnitData(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendUnitData, FalconEvent::CampaignThread, senderid, target)
	{
	dataBlock.unitData = NULL;
	dataBlock.size = 0;
	type;
	}

FalconSendUnitData::~FalconSendUnitData(void)
	{
	if (dataBlock.unitData)
		delete[] dataBlock.unitData;
	dataBlock.unitData = NULL;
	}

int FalconSendUnitData::Size (void) 
	{ 
	ShiAssert ( dataBlock.size >= 0 );
	
	return sizeof(dataBlock) + dataBlock.size + FalconEvent::Size();
	}

int FalconSendUnitData::Decode (VU_BYTE **buf, int length)
	{
	int	size = 0;
	int offset = 0;

	memcpy (&dataBlock, *buf, sizeof (dataBlock));      *buf += sizeof (dataBlock);		size += sizeof (dataBlock);
    ShiAssert(dataBlock.size >= 0);
	dataBlock.unitData = new VU_BYTE[dataBlock.size];
	memcpy (dataBlock.unitData, *buf, dataBlock.size);	*buf += dataBlock.size;			size += dataBlock.size;
	size += FalconEvent::Decode (buf, length);

	ShiAssert ( size == Size() );

	CampaignJoinKeepAlive ();

	uchar				*bufptr;
	FalconSessionEntity	*session = (FalconSessionEntity*) vuDatabase->Find(dataBlock.owner);

	MonoPrint ("RecvUnitData %08x:%08x %d %d %d\n",
		dataBlock.owner,
		dataBlock.set,
		dataBlock.block,
		dataBlock.size);

	if (!TheCampaign.IsPreLoaded() || !session)
		return size;

	if (TheCampaign.Flags & CAMP_NEED_UNIT_DATA)
		{
		if (dataBlock.set != session->unitDataReceiveSet)
			{
			MonoPrint("Tossing old Set, starting a new one\n");
			// New data - toss the old stuff
			delete session->unitDataReceiveBuffer;
			session->unitDataReceiveBuffer = NULL;
			session->unitDataReceiveSet = dataBlock.set;
			memset(session->unitDataReceived,0,FS_MAXBLK/8);
			}

		if (!session->unitDataReceiveBuffer)
			session->unitDataReceiveBuffer = new uchar[dataBlock.totalSize];

		if (dataBlock.block + 1 == dataBlock.totalBlocks)
		{
			offset = dataBlock.totalSize - dataBlock.size;
		}
		else
		{
			offset = dataBlock.size * dataBlock.block;
		}

		bufptr = session->unitDataReceiveBuffer + offset;
		memcpy(bufptr, dataBlock.unitData, dataBlock.size);

	#ifdef DEBUG_STARTUP
		MonoPrint("Got Unit Block #%d!\n",dataBlock.block);
	#endif

		gCampJoinTries = 0;

		// Mark this block as being received.
		session->unitDataReceived[dataBlock.block/8] |= (1 << (dataBlock.block%8));

		// Check if we've gotten all our blocks
		for (int i=0; i<dataBlock.totalBlocks; i++)
			{
	//		if (!StillNeeded(dataBlock.block, session->unitDataReceived))
			if (!(session->unitDataReceived[i/8] & (1 << (i %8))))
				return size;
			}

		// If we get here, it's because all blocks are read
		TheCampaign.Flags &= ~CAMP_NEED_UNIT_DATA;
		bufptr = session->unitDataReceiveBuffer;

		CampEnterCriticalSection();
		DecodeUnitData((VU_BYTE**) &bufptr, session);
		CampLeaveCriticalSection();

		session->unitDataReceiveBuffer = NULL;
		session->unitDataReceiveSet = 0;

		// Let the UI know we've received some data
		if(gMainHandler)
			PostMessage(gMainHandler->GetAppWnd(),FM_GOT_CAMPAIGN_DATA,CAMP_NEED_UNIT_DATA,0);
		}
	return size;
	}

int FalconSendUnitData::Encode (VU_BYTE **buf)
	{
	int size = 0;

    ShiAssert(dataBlock.size >= 0);
	memcpy (*buf, &dataBlock, sizeof (dataBlock));      *buf += sizeof (dataBlock);		size += sizeof (dataBlock);
	memcpy (*buf, dataBlock.unitData, dataBlock.size);	*buf += dataBlock.size;			size += dataBlock.size;
	size += FalconEvent::Encode (buf);

	ShiAssert ( size == Size() );

	return size;
	}

int FalconSendUnitData::Process(uchar autodisp)
	{
	return 0;
	autodisp;
	}

// =========================================
// Global functions
// =========================================

void SendCampaignUnitData (FalconSessionEntity *session, VuTargetEntity *target, uchar *blocksNeeded)
{
	int			blocks,curBlock=0,blocksize;
	ulong		sizeleft;
	uchar		*buffer,*bufptr;
	FalconSendUnitData	*msg;
	CampBaseClass *ent;
	
	if (!blocksNeeded)
	{
		int		set = rand();
		
		if (!set)
			set++;
		
		if (session->unitDataSendBuffer)
			delete session->unitDataSendBuffer;
		
		// Encode the unit data
		session->unitDataSendSize = EncodeUnitData((VU_BYTE**)&buffer, FalconLocalSession);
		session->unitDataSendBuffer	= buffer;
		session->unitDataSendSet = (short)set;
	}
	
	// Find the block size, if we havn't already
	if (!gUnitBlockSize)
	{
		// This is a temporary message, purely for sizing purposes
		FalconSendUnitData	tmpmsg(session->Id(), target);
		gUnitBlockSize = F4VuMaxTCPMessageSize - tmpmsg.Size() - 16;
	}
	
	sizeleft = session->unitDataSendSize;
	blocks = (session->unitDataSendSize + gUnitBlockSize - 1) / gUnitBlockSize;
	
#ifdef DEBUG_STARTUP
	if (!blocksNeeded)
		MonoPrint("Sending unit data (%d blocks): ",blocks);
	else
		MonoPrint("Resending unit data: ");
#endif
	
	buffer = bufptr = session->unitDataSendBuffer;
	
	while (sizeleft)
	{
		if (sizeleft < gUnitBlockSize)
		{
			blocksize = sizeleft;
			sizeleft = 0;
		}
		else
		{
			blocksize = gUnitBlockSize;
			sizeleft -= gUnitBlockSize;
		}
		
		msg = new FalconSendUnitData(session->Id(), target);
		msg->dataBlock.size = (short)blocksize;
		msg->dataBlock.unitData = new uchar[blocksize];
		memcpy (msg->dataBlock.unitData, bufptr, blocksize);
		bufptr += blocksize;
		msg->dataBlock.owner = FalconLocalSessionId;
		msg->dataBlock.set = session->unitDataSendSet;
		msg->dataBlock.totalSize = session->unitDataSendSize;
		msg->dataBlock.block = (uchar)curBlock;
		msg->dataBlock.totalBlocks = (uchar)blocks;
		
#ifdef DEBUG_STARTUP
		MonoPrint("%d:%d ",msg->dataBlock.block,msg->dataBlock.size);
#endif
		
		FalconSendMessage(msg,TRUE);
		msg = NULL;
		
		curBlock++;
	}

	VuListIterator	iterator(DeaggregateList);

	ent = (CampBaseClass *) iterator.GetFirst ();
	while (ent)
	{
		ent->SendDeaggregateData (target);

		ent = (CampBaseClass *) iterator.GetNext ();
	}
	
#ifdef DEBUG_STARTUP
	MonoPrint("\n");
#endif
}

int StillNeeded (int block, uchar gotData[])
	{

	ShiAssert (block < FS_MAXBLK);

	if (gotData[block/8] & (1 << (block%8)))
		return 1;
	return 0;
	}


/*
	// Split this message into blocks, if necessary
	bufptr = buffer;
	blocks = (size + gUnitBlockSize - 1) / gUnitBlockSize;

#ifdef DEBUG_STARTUP
	MonoPrint("Sending unit data (%d blocks): ",blocks);
#endif

	while (sizeleft)
		{
		if (!msg)
			msg = new FalconSendUnitData(id, target);
		if (sizeleft < gUnitBlockSize)
			{
			msg->dataBlock.size = sizeleft;
			msg->dataBlock.unitData = new uchar[sizeleft];
			memcpy (msg->dataBlock.unitData, bufptr, sizeleft);
			bufptr += sizeleft;
			sizeleft = 0;
			}
		else
			{
			msg->dataBlock.size = gUnitBlockSize;
			msg->dataBlock.unitData = new uchar[gUnitBlockSize];
			memcpy (msg->dataBlock.unitData, bufptr, gUnitBlockSize);
			bufptr += gUnitBlockSize;
			sizeleft -= gUnitBlockSize;
			}
		msg->dataBlock.owner = FalconLocalSessionId;
		msg->dataBlock.set = set;
		msg->dataBlock.totalSize = size;
		msg->dataBlock.block = curBlock;
		msg->dataBlock.totalBlocks = blocks;

#ifdef DEBUG_STARTUP
		MonoPrint("%d:%d ",msg->MsgId().id_,msg->dataBlock.size);
#endif

		FalconSendMessage(msg,TRUE);
		msg = NULL;
		curBlock++;
		}

#ifdef DEBUG_STARTUP
	MonoPrint("\n");
#endif
	}
*/

