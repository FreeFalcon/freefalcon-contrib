/*
 * Machine Generated source file for message "Flight Plan Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 10-November-1998 at 16:18:21
 * Generated from file EVENTS.XLS by .
 */

#include "MsgInc\FalconFlightPlanMsg.h"
#include "mesg.h"

FalconFlightPlanMessage::FalconFlightPlanMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (FalconFlightPlanMsg, FalconEvent::CampaignThread, entityId, target, loopback)
{
	dataBlock.data = NULL;
	dataBlock.size = 0;
//me123	RequestOutOfBandTransmit ();
	RequestReliableTransmit ();
}

FalconFlightPlanMessage::FalconFlightPlanMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (FalconFlightPlanMsg, FalconEvent::CampaignThread, senderid, target)
{
	dataBlock.data = NULL;
	dataBlock.size = 0;
	type;
}

FalconFlightPlanMessage::~FalconFlightPlanMessage(void)
{
	if (dataBlock.data)
		delete dataBlock.data;
	dataBlock.data = NULL;
}

int FalconFlightPlanMessage::Size (void)
{ 
    	ShiAssert ( dataBlock.size >= 0 );
	return sizeof (uchar) + sizeof (long) + dataBlock.size + FalconEvent::Size();
}

int FalconFlightPlanMessage::Decode (VU_BYTE **buf, int length)
{
	int size;

	size = FalconEvent::Decode (buf, length);
    memcpy (&dataBlock.type, *buf, sizeof(uchar));		*buf += sizeof (uchar);			size += sizeof (uchar);
    memcpy (&dataBlock.size, *buf, sizeof(long));		*buf += sizeof (long);			size += sizeof (long);
    	ShiAssert ( dataBlock.size >= 0 );
	dataBlock.data = new uchar[dataBlock.size];
    memcpy (dataBlock.data, *buf, dataBlock.size);		*buf += dataBlock.size;			size += dataBlock.size;
	return size;
}

int FalconFlightPlanMessage::Encode (VU_BYTE **buf)
{
	int size;

	size = FalconEvent::Encode (buf);
    	ShiAssert ( dataBlock.size >= 0 );
    memcpy (*buf, &dataBlock.type, sizeof(uchar));		*buf += sizeof (uchar);			size += sizeof (uchar);
    memcpy (*buf, &dataBlock.size, sizeof(long));		*buf += sizeof (long);			size += sizeof (long);
	memcpy (*buf, dataBlock.data, dataBlock.size);		*buf += dataBlock.size;			size += dataBlock.size;
	return size;
}

int FalconFlightPlanMessage::Process(uchar autodisp)
{
	Unit		unit = (Unit) Entity();
	uchar		*buffer=dataBlock.data;
	long		lbsfuel,planes;

	if (autodisp || !unit || !unit->IsLocal())
		return -1;

	switch (dataBlock.type)
		{
		case squadronStores:
			short		weapon[HARDPOINT_MAX];
			unsigned char weapons[HARDPOINT_MAX];

			memcpy(weapon,buffer,HARDPOINT_MAX * sizeof(short));		buffer += HARDPOINT_MAX * sizeof(short);
			memcpy(weapons,buffer,HARDPOINT_MAX);		buffer += HARDPOINT_MAX;
			memcpy(&lbsfuel,buffer,sizeof(long));		buffer += sizeof(long);
			memcpy(&planes,buffer,sizeof(long));		buffer += sizeof(long);
			((Squadron)unit)->UpdateSquadronStores (weapon, weapons, lbsfuel, planes);
			((Squadron)unit)->MakeSquadronDirty (DIRTY_SQUAD_STORES, DDP[149].priority);
			//	((Squadron)unit)->MakeSquadronDirty (DIRTY_SQUAD_STORES, SEND_EVENTUALLY);
			break;
		case loadoutData:
			uchar				ac;
			LoadoutStruct		*loadout;
			int					i;

			memcpy(&lbsfuel,buffer,sizeof(long));						buffer += sizeof(long);
			memcpy(&ac,buffer,sizeof(uchar));							buffer += sizeof(uchar);
			loadout = new LoadoutStruct[ac];
			for (i=0; i<ac; i++)
				{
				memcpy(loadout[i].WeaponID, buffer, HARDPOINT_MAX * sizeof(short));		buffer += HARDPOINT_MAX * sizeof(short);
				memcpy(loadout[i].WeaponCount, buffer, HARDPOINT_MAX);	buffer += HARDPOINT_MAX;
				}
			((Flight)unit)->SetLoadout(loadout,ac);
			((Flight)unit)->MakeFlightDirty (DIRTY_STORES, DDP[150].priority);
			//	((Flight)unit)->MakeFlightDirty (DIRTY_STORES, SEND_NOW);
			break;
		case waypointData:
			unit->DecodeWaypoints(&buffer);
			unit->MakeUnitDirty (DIRTY_WP_LIST, DDP[151].priority);
			//	unit->MakeUnitDirty (DIRTY_WP_LIST, SEND_NOW);
			break;
		default:
			return -1;
		}
	return 0;
}

