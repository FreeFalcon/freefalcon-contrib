/*
 * Machine Generated source file for message "Send Logbook".
 * NOTE: The functions here must be completed by hand.
 * Generated on 08-November-1998 at 16:23:49
 * Generated from file EVENTS.XLS by PeterW
 */

#include "MsgInc\SendLogbook.h"
#include "mesg.h"
#include "ui\include\uicomms.h"

UI_SendLogbook::UI_SendLogbook(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendLogbook, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

UI_SendLogbook::UI_SendLogbook(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendLogbook, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

UI_SendLogbook::~UI_SendLogbook(void)
{
   // Your Code Goes Here
}

int UI_SendLogbook::Process(uchar autodisp)
{
	if(gCommsMgr)
		gCommsMgr->ReceiveLogbook(dataBlock.fromID,&dataBlock.Pilot);
   return 0;
   autodisp;
}

