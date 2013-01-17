/*
 * Machine Generated source file for message "Camp Data Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 30-September-1997 at 16:41:13
 * Generated from file EVENTS.XLS by MicroProse
 */

#include "MsgInc\CampDataMsg.h"
#include "mesg.h"

#ifdef DEBUG
int	cdecode_count = 0, cencode_count = 0;
#endif

void DecodePrimaryObjectiveList (uchar *data, FalconEntity *fe);
void tactical_set_orders(Battalion bat, VU_ID obj, GridIndex tx, GridIndex ty);

FalconCampDataMessage::FalconCampDataMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (CampDataMsg, FalconEvent::CampaignThread, entityId, target, FALSE)
	{
	dataBlock.data = NULL;
	dataBlock.size = 0;
	loopback;
	}

FalconCampDataMessage::FalconCampDataMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (CampDataMsg, FalconEvent::CampaignThread, senderid, target)
	{
	dataBlock.data = NULL;
	dataBlock.size = 0;
	type;
	}

FalconCampDataMessage::~FalconCampDataMessage(void)
	{
	if (dataBlock.data)
	   delete [] dataBlock.data;
	dataBlock.data = NULL;
	dataBlock.size = 0;
	}

int FalconCampDataMessage::Size (void)
	{
    	ShiAssert ( dataBlock.size >= 0 );
	return FalconEvent::Size() +
		sizeof(unsigned int) + 
		sizeof(ushort) +		
		dataBlock.size;
	}

int FalconCampDataMessage::Decode (VU_BYTE **buf, int length)
	{
	int size = 0;

#ifdef DEBUG
	cdecode_count ++;
	//MonoPrint ("CampDataMessageDecode %d\n", cdecode_count);
#endif

	size += FalconEvent::Decode (buf, length);
	memcpy (&dataBlock.type, *buf, sizeof(unsigned int));		*buf += sizeof(unsigned int);   size += sizeof(unsigned int);
	memcpy (&dataBlock.size, *buf, sizeof(ushort));				*buf += sizeof(ushort);			size += sizeof(ushort);		
    	ShiAssert ( dataBlock.size >= 0 );
	dataBlock.data = new uchar[dataBlock.size];
	memcpy (dataBlock.data, *buf, dataBlock.size);				*buf += dataBlock.size;			size += dataBlock.size;		

	ShiAssert (size == Size());

	return size;
	}

int FalconCampDataMessage::Encode (VU_BYTE **buf)
	{
	int size = 0;

#ifdef DEBUG
	cencode_count ++;
	//MonoPrint ("CampDataMessageEncode %d\n",cencode_count);
#endif

    	ShiAssert ( dataBlock.size >= 0 );
	size += FalconEvent::Encode (buf);
	memcpy (*buf, &dataBlock.type, sizeof(unsigned int));		*buf += sizeof(unsigned int);   size += sizeof(unsigned int);
	memcpy (*buf, &dataBlock.size, sizeof(ushort));				*buf += sizeof(ushort);			size += sizeof(ushort);		
	memcpy (*buf, dataBlock.data, dataBlock.size);				*buf += dataBlock.size;			size += dataBlock.size;		

	ShiAssert (size == Size());

	return size;
	}

int FalconCampDataMessage::Process(uchar autodisp)
	{
	CampEntity	ent = (CampEntity)vuDatabase->Find(EntityId());

	if (autodisp)
		return 0;

	switch (dataBlock.type)
		{
		case campPriorityData:
			// Set the new priority of this objective/these objectives
			DecodePrimaryObjectiveList (dataBlock.data, ent);
			TheCampaign.Flags &= ~CAMP_NEED_PERSIST;
			if(gMainHandler)
				PostMessage(gMainHandler->GetAppWnd(),FM_GOT_CAMPAIGN_DATA,CAMP_NEED_PRIORITIES,0);
			TheCampaign.GotJoinData();
			break;
		case campOrdersData:
			// Set the new orders
			{
			VU_ID			tmpId;
			GridIndex		x,y;
			uchar			*dataPtr = dataBlock.data;

			ShiAssert(ent->IsBattalion());

			memcpy(&tmpId, dataPtr, sizeof(tmpId));		dataPtr += sizeof(tmpId);
			memcpy(&x, dataPtr, sizeof(x));				dataPtr += sizeof(x);
			memcpy(&y, dataPtr, sizeof(y));				dataPtr += sizeof(y);
			tactical_set_orders((Battalion)ent, tmpId, x, y);
			}
			break;
		default:
			break;
		}
	return 0;
	}
         
