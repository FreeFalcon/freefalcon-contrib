/*
 * Machine Generated source file for message "Data Link Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 16-April-1998 at 12:26:05
 * Generated from file EVENTS.XLS by Microprose
 */

#include "MsgInc\DLinkMsg.h"
#include "mesg.h"
#include "navsystem.h"



char* DLink_Type_Str[5] = {
		"NODINK",
		"IP",
		"TGT",
		"EGR",
		"CP"
	};

FalconDLinkMessage::FalconDLinkMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (DLinkMsg, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

FalconDLinkMessage::FalconDLinkMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (DLinkMsg, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

FalconDLinkMessage::~FalconDLinkMessage(void)
{
   // Your Code Goes Here
}

int FalconDLinkMessage::Process(uchar autodisp)
{
	SimBaseClass* theEntity;

   theEntity = (SimBaseClass*)(vuDatabase->Find (EntityId()));

   if (theEntity && theEntity->IsLocal() && theEntity == SimDriver.playerEntity) {
		gNavigationSys->SetDataLinks(dataBlock.numPoints,
												dataBlock.targetType,
												dataBlock.threatType,
												(FalconDLinkMessage::DLinkPointType *) dataBlock.ptype,
												dataBlock.px,
												dataBlock.py,
												dataBlock.pz,
												dataBlock.arrivalTime);

		OTWDriver.pCockpitManager->mpIcp->SetICPUpdateFlag(DLINK_UPDATE);
	}

	return 0;
	autodisp;
}
