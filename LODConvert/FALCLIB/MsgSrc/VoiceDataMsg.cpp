/*
 * Machine Generated source file for message "Voice Data Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 17-November-1997 at 21:35:38
 * Generated from file EVENTS.XLS by MicroProse
 */

#include "MsgInc\VoiceDataMsg.h"
#include "mesg.h"

FalconVoiceDataMessage::FalconVoiceDataMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (VoiceDataMsg, FalconEvent::VuThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

FalconVoiceDataMessage::FalconVoiceDataMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (VoiceDataMsg, FalconEvent::VuThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

FalconVoiceDataMessage::~FalconVoiceDataMessage(void)
{
   // Your Code Goes Here
}

int FalconVoiceDataMessage::Process(uchar autodisp)
{
   // Your Code Goes Here
   return 0;
   autodisp;
}

