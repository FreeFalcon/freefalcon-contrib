/*
 * Machine Generated source file for message "Request Logbook".
 * NOTE: The functions here must be completed by hand.
 * Generated on 08-November-1998 at 16:23:49
 * Generated from file EVENTS.XLS by PeterW
 */

#include "MsgInc\RequestLogbook.h"
#include "mesg.h"
#include "ui\include\uicomms.h"

UI_RequestLogbook::UI_RequestLogbook(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (RequestLogbook, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

UI_RequestLogbook::UI_RequestLogbook(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (RequestLogbook, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

UI_RequestLogbook::~UI_RequestLogbook(void)
{
   // Your Code Goes Here
}

int UI_RequestLogbook::Process(uchar autodisp)
{
	if(gCommsMgr && dataBlock.fromID != FalconLocalSessionId)
		gCommsMgr->SendLogbook(dataBlock.fromID);
   return 0;
   autodisp;
}

