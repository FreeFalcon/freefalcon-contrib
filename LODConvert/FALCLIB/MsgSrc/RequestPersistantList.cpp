/*
 * Machine Generated source file for message "Request Persistant List".
 * NOTE: The functions here must be completed by hand.
 * Generated on 29-July-1997 at 18:41:02
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#include "MsgInc\RequestPersistantList.h"
#include "mesg.h"

FalconRequestPersistantList::FalconRequestPersistantList(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (RequestPersistantList, FalconEvent::CampaignThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

FalconRequestPersistantList::FalconRequestPersistantList(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (RequestPersistantList, FalconEvent::CampaignThread, senderid, target)
{
   // Your Code Goes Here
}

FalconRequestPersistantList::~FalconRequestPersistantList(void)
{
   // Your Code Goes Here
}

int FalconRequestPersistantList::Process(void)
{
   // Your Code Goes Here
   return 0;
}

