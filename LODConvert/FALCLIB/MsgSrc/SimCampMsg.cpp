/*
 * Machine Generated source file for message "Sim Camp Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 03-August-1998 at 13:53:14
 * Generated from file EVENTS.XLS by MicroProse
 */

#include "MsgInc\SimCampMsg.h"
#include "mesg.h"
#include "Campaign.h"
#include "TimerThread.h"//me123

#ifdef DEBUG
int	decode_count = 0, encode_count = 0;
#endif

extern "C"
{
	int check_bandwidth (int);
	int check_rudp_bandwidth (int);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FalconSimCampMessage::FalconSimCampMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SimCampMsg, FalconEvent::SimThread, entityId, target, loopback)
{
	dataBlock.size = 0;
	dataBlock.data = NULL;
	RequestReliableTransmit ();
//me123	RequestOutOfBandTransmit ();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FalconSimCampMessage::FalconSimCampMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SimCampMsg, FalconEvent::SimThread, senderid, target)
{
	dataBlock.size = 0;
	dataBlock.data = NULL;
	type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FalconSimCampMessage::~FalconSimCampMessage(void)
{
	if (dataBlock.data)
		delete [] dataBlock.data;
	dataBlock.data = NULL;
	dataBlock.size = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconSimCampMessage::Size (void)
{
    ShiAssert(dataBlock.size >= 0);
	int size = FalconEvent::Size() +
		sizeof(VU_ID) +
		sizeof(unsigned int) + 
		sizeof(ushort) +		
		dataBlock.size;
	//MonoPrint("SimCampMessage::Size(): %d\n",size);
	return size;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconSimCampMessage::Decode (VU_BYTE **buf, int length)
{
	int size = 0;
	
#ifdef DEBUG
	decode_count ++;
	//MonoPrint ("CampDataMessageDecode %d\n", decode_count);
#endif
	
	size += FalconEvent::Decode (buf, length);
	memcpy (&dataBlock.from, *buf, sizeof(VU_ID));				*buf += sizeof(VU_ID);			size += sizeof(VU_ID);
	memcpy (&dataBlock.message, *buf, sizeof(unsigned int));	*buf += sizeof(unsigned int);   size += sizeof(unsigned int);
	memcpy (&dataBlock.size, *buf, sizeof(ushort));				*buf += sizeof(ushort);			size += sizeof(ushort);		
    ShiAssert(dataBlock.size >= 0);
	dataBlock.data = new uchar[dataBlock.size];
	memcpy (dataBlock.data, *buf, dataBlock.size);				*buf += dataBlock.size;			size += dataBlock.size;		
	
	ShiAssert (size == Size());
	
	return size;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconSimCampMessage::Encode (VU_BYTE **buf)
{
	int size = 0;
	
#ifdef DEBUG
	encode_count ++;
	//MonoPrint ("CampDataMessageEncode %d\n", encode_count);
#endif
	
    ShiAssert(dataBlock.size >= 0);
	size += FalconEvent::Encode (buf);
	memcpy (*buf, &dataBlock.from, sizeof(VU_ID));				*buf += sizeof(VU_ID);			size += sizeof(VU_ID);
	memcpy (*buf, &dataBlock.message, sizeof(unsigned int));	*buf += sizeof(unsigned int);   size += sizeof(unsigned int);
	memcpy (*buf, &dataBlock.size, sizeof(ushort));				*buf += sizeof(ushort);			size += sizeof(ushort);		
	memcpy (*buf, dataBlock.data, dataBlock.size);				*buf += dataBlock.size;			size += dataBlock.size;		
	
	ShiAssert (size == Size());
	
	return size;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int FalconSimCampMessage::Process(uchar autodisp)
{
	CampEntity				ent = (CampEntity)vuDatabase->Find(EntityId());
	FalconSessionEntity		*session = (FalconSessionEntity*) vuDatabase->Find(dataBlock.from);
	
	if(autodisp || !ent || !session || !FalconLocalGame)
		return 0;
	
	CampEnterCriticalSection();
	switch (dataBlock.message)
	{
	case simcampReaggregate:
		if (check_bandwidth (150))
		{
			ent->Reaggregate(session);
			//MonoPrint ("Reag   %d\n", ent->Id().num_);
		}
		break;

	case simcampDeaggregate:

// OW: me123 MP Fix
#if 0
		if (check_bandwidth (150))
		{
#else
		if (check_bandwidth (150)|| ent->IsSetFalcFlag(FEC_PLAYER_ENTERING|FEC_HASPLAYERS))//me123
		{//me123 addet the player check
			//me123 send player deags so they can go past pie 2 fast
#endif
			//MonoPrint ("Deag   %d\n", ent->Id().num_);
			ent->Deaggregate(session);
		}
		else
		{
			//MonoPrint ("NoDeag %d\n", ent->Id().num_)
		}
		break;

	case simcampChangeOwner:
		//			MonoPrint ("Sim Camp Change Owner %08x %08x%08x\n", ent, session->Id ());
		ent->RecordCurrentState (session, FALSE);
		ent->SetDeagOwner (session->Id ());
		break;
		
	case simcampRequestDeagData:
		// MonoPrint ("Request Deag Data\n");
		ent->SendDeaggregateData(FalconLocalGame);
		break;

	case simcampReaggregateFromData:
		ent->ReaggregateFromData(dataBlock.size,dataBlock.data);
		break;

	case simcampDeaggregateFromData:
		ent->DeaggregateFromData(dataBlock.size,dataBlock.data);
		break;

	case simcampChangeOwnerFromData:
		break;

	case simcampRequestAllDeagData:
		{
			SetTimeCompression(1);//me123 if a client is callign this he's in the pie
			//let's set the compresion to 1 on the host so we don'e fuck up the realtime
			//becourse the clients stops transmitting timecompresion and we go to 64 again for awhile.
			int count = 0;
			VuListIterator	deag_it(DeaggregateList);
			CampEntity c;
			c = (CampEntity) deag_it.GetFirst();
			while (c)
			{
				if ((!c->IsAggregate()) && (c->IsLocal()))
				{
					c->SendDeaggregateData(FalconLocalGame);
					count ++;
				}
				c = (CampEntity) deag_it.GetNext();
			}
			
			//			MonoPrint ("Request All Deag Data = %d\n", count);
			break;
		}
	}
	CampLeaveCriticalSection();
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
