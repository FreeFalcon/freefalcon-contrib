/*
 * Machine Generated source file for message "Sim Data Toggle".
 * NOTE: The functions here must be completed by hand.
 * Generated on 20-March-1997 at 16:09:17
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#include "MsgInc\SimDataToggle.h"
#include "mesg.h"

FalconSimDataToggle::FalconSimDataToggle(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SimDataToggle, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
	RequestReliableTransmit ();
	RequestOutOfBandTransmit ();
}

FalconSimDataToggle::FalconSimDataToggle(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SimDataToggle, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

FalconSimDataToggle::~FalconSimDataToggle(void)
{
   // Your Code Goes Here
}

int FalconSimDataToggle::Process(uchar autodisp)
{
SimMoverClass* theEntity;

	if (autodisp)
		return 0;

   theEntity = (SimMoverClass*)(vuDatabase->Find (dataBlock.entityID));
   if (theEntity && IsLocal())
   {
      theEntity->AddDataRequest(dataBlock.flag);
   }

   return TRUE;
}

