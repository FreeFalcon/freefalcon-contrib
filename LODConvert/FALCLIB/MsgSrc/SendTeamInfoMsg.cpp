/*
 * Machine Generated source file for message "Send Team Info".
 * NOTE: The functions here must be completed by hand.
 * Generated on 05-November-1996 at 17:39:12
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#include "MsgInc\SendTeamInfoMsg.h"
#include "mesg.h"

FalconSendTeamInfoMessage::FalconSendTeamInfoMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendTeamInfoMsg, FalconEvent::CampaignThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

FalconSendTeamInfoMessage::FalconSendTeamInfoMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendTeamInfoMsg, FalconEvent::CampaignThread, senderid, target)
{
   // Your Code Goes Here
}

FalconSendTeamInfoMessage::~FalconSendTeamInfoMessage(void)
{
   // Your Code Goes Here
}

int FalconSendTeamInfoMessage::Process(void)
{
   // Your Code Goes Here
   return 0;
}

