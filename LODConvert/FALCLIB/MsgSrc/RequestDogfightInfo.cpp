/*
 * Machine Generated source file for message "Request Dogfight Info".
 * NOTE: The functions here must be completed by hand.
 * Generated on 03-April-1997 at 02:36:20
 * Generated from file EVENTS.XLS by PeterW
 */

#include "MsgInc\RequestDogfightInfo.h"
#include "mesg.h"
#include "dogfight.h"

UI_RequestDogfightInfo::UI_RequestDogfightInfo(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (RequestDogfightInfo, FalconEvent::SimThread, entityId, target, loopback)
{
	RequestOutOfBandTransmit();
}

UI_RequestDogfightInfo::UI_RequestDogfightInfo(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (RequestDogfightInfo, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

UI_RequestDogfightInfo::~UI_RequestDogfightInfo(void)
{
   // Your Code Goes Here
}

int UI_RequestDogfightInfo::Process(uchar autodisp)
	{
	if (autodisp)
		return 0;

	if (FalconLocalGame && FalconLocalGame->IsLocal() && FalconLocalGame->GetGameType() == game_Dogfight)
		{
		FalconSessionEntity* requester = (FalconSessionEntity*)vuDatabase->Find(dataBlock.requester_id);
		if (!requester)
			return FALSE;

		SimDogfight.SendSettings(requester);
		}
	return TRUE;
	}

