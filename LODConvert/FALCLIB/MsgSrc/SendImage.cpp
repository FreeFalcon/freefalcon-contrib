/*
 * Machine Generated source file for message "Send Image".
 * NOTE: The functions here must be completed by hand.
 * Generated on 08-November-1998 at 16:52:27
 * Generated from file EVENTS.XLS by PeterW
 */

#include "MsgInc\SendImage.h"
#include "mesg.h"

UI_SendImage::UI_SendImage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendImage, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

UI_SendImage::UI_SendImage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendImage, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

UI_SendImage::~UI_SendImage(void)
{
   // Your Code Goes Here
	delete dataBlock.data;
}

int UI_SendImage::Process(uchar autodisp)
{
   // Your Code Goes Here
   return 0;
   autodisp;
}

