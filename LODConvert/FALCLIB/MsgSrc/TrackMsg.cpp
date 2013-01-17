/*
 * Machine Generated source file for message "Radar Track Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 28-September-1997 at 17:47:47
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#include "MsgInc\TrackMsg.h"
#include "mesg.h"
#include "rwr.h"
#include "aircrft.h"

FalconTrackMessage::FalconTrackMessage(int reliable, VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (TrackMsg, FalconEvent::SimThread, entityId, target, loopback)
{

	if (reliable == 1)
	{
		RequestReliableTransmit ();
	}
	else if (reliable == 2)
	{
		RequestOutOfBandTransmit ();
		RequestReliableTransmit ();
	}

		
}

FalconTrackMessage::FalconTrackMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (TrackMsg, FalconEvent::SimThread, senderid, target)
{
	type;
}

FalconTrackMessage::~FalconTrackMessage(void)
{
}

int FalconTrackMessage::Decode (VU_BYTE **buf, int length)
{
int size;

   size = FalconEvent::Decode (buf, length);
   memcpy (&dataBlock, *buf, sizeof (DATA_BLOCK));
   *buf += sizeof (DATA_BLOCK);
   size += sizeof (DATA_BLOCK);
   return size;
}


int FalconTrackMessage::Encode (VU_BYTE **buf)
{
int size;

   size = FalconEvent::Encode (buf);
   memcpy (*buf, &dataBlock, sizeof (DATA_BLOCK));
   *buf += sizeof (DATA_BLOCK);
   size += sizeof (DATA_BLOCK);
   return size;
}


int FalconTrackMessage::Process(uchar autodisp)
{
	SimVehicleClass*	theEntity;
	FalconEntity*		taggedEntity;
	int j;
	
	if (autodisp)
		return 0;
	
	theEntity = (SimVehicleClass*)Entity();
	if (!theEntity)
		return 0;

	taggedEntity = (FalconEntity*)(vuDatabase->Find (dataBlock.id));

	// 2002-02-25 ADDED BY S.G. 'Entity' can be a campaign object if the missile being launch (ie, dataBlock.trackType is Launch) is a SARH and its platform is still aggregated since the platform becomes the launching object. Get in if it's a Mover class only...
	if (((FalconEntity *)Entity())->IsMover()) {
		if (!theEntity->IsLocal () && ((SimMoverClass*)theEntity)->numSensors
			> 1 && ((SimVehicleClass*)theEntity)->sensorArray[1] // MN 2002-03-03 CTD
			&& theEntity->IsAirplane()// JB 010604 CTD
			)
			 {
			  if (dataBlock.trackType == Track_Lock)
			  ((SimVehicleClass*)theEntity)->sensorArray[1]->RemoteBuggedTarget = taggedEntity;

			  else if (dataBlock.trackType == Track_Unlock)
			((SimVehicleClass*)theEntity)->sensorArray[1]->RemoteBuggedTarget = NULL ;
			 }

	}

	if (taggedEntity)
	{
		// HACK HACK HACK!  This should be status info on the aircraft concerned...
		// check for smoke on/off messages
		if (dataBlock.trackType == Track_SmokeOn)
		{
			((AircraftClass *)taggedEntity)->playerSmokeOn = TRUE;
		}
		else if (dataBlock.trackType == Track_SmokeOff)
		{
			((AircraftClass *)taggedEntity)->playerSmokeOn = FALSE;
		}
		else if (dataBlock.trackType == Track_JettisonAll)
		{
			if ((!IsLocal ()) && (taggedEntity->IsAirplane ()))
			{
//				MonoPrint ("JettisonAll %08x\n", dataBlock.id, dataBlock.hardpoint);
				((AircraftClass*)taggedEntity)->Sms->EmergencyJettison ();
			}
		}
		else if (dataBlock.trackType == Track_JettisonWeapon)
		{
			if ((!IsLocal ()) && (taggedEntity->IsAirplane ()))
			{
//				MonoPrint ("Jettison Weapon %08x %d\n", dataBlock.id, dataBlock.hardpoint);
				((AircraftClass*)taggedEntity)->Sms->JettisonWeapon (dataBlock.hardpoint);
			}
		}
		else if (dataBlock.trackType == Track_RemoveWeapon)
		{
			if ((!IsLocal ()) && (taggedEntity->IsAirplane ()))
			{
//				MonoPrint ("RemoveWeapon %08x %d\n", dataBlock.id, dataBlock.hardpoint);
				((AircraftClass*)taggedEntity)->Sms->RemoveWeapon (dataBlock.hardpoint);
			}
		}
		else if (taggedEntity->IsLocal())
		{
			// 2002-02-09 COMMENT BY S.G. In here, hardpoint will be used to specify the kind of radar mode the radar was in when.
			if (taggedEntity->IsAirplane())
			{
				// Find RWR and/or HTS
				for (j=0; j<((SimVehicleClass*)taggedEntity)->numSensors; j++)
				{
					if (((SimVehicleClass*)taggedEntity)->sensorArray[j]->Type() == SensorClass::RWR)
						((RwrClass*)((SimVehicleClass*)taggedEntity)->sensorArray[j])->ObjectDetected(theEntity, dataBlock.trackType, dataBlock.hardpoint); // 2002-02-09 MODIFIED BY S.G. Added hardpoint to ObjectDetected
					if (((SimVehicleClass*)taggedEntity)->sensorArray[j]->Type() == SensorClass::HTS)
						((RwrClass*)((SimVehicleClass*)taggedEntity)->sensorArray[j])->ObjectDetected(theEntity, dataBlock.trackType, dataBlock.hardpoint);  // 2002-02-09 MODIFIED BY S.G. Added hardpoint to ObjectDetected
				}
			}
			else if (taggedEntity->IsFlight())
			{
				if ((dataBlock.trackType == Track_Lock) || (dataBlock.trackType == Track_Launch))
				{
					// Tell the campaign entity that it's being locked on to.
					((Flight)taggedEntity)->RegisterLock(theEntity);
				}
			}
		}
	}
	
	return 1;
}

