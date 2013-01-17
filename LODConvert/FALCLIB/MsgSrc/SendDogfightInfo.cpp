/*
 * Machine Generated source file for message "Send Dogfight Info".
 * NOTE: The functions here must be completed by hand.
 * Generated on 03-April-1997 at 02:36:21
 * Generated from file EVENTS.XLS by PeterW
 */

#include "MsgInc\SendDogfightInfo.h"
#include "mesg.h"
#include "Dispcfg.h"
#include "Dogfight.h"

UI_SendDogfightInfo::UI_SendDogfightInfo(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendDogfightInfo, FalconEvent::SimThread, entityId, target, loopback)
{
	RequestOutOfBandTransmit();
RequestReliableTransmit ();
}

UI_SendDogfightInfo::UI_SendDogfightInfo(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendDogfightInfo, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
RequestReliableTransmit ();
	type;
}

UI_SendDogfightInfo::~UI_SendDogfightInfo(void)
{
   // Your Code Goes Here
}

int UI_SendDogfightInfo::Process(uchar autodisp)
{
	// Abort if this is autodispatching (the UI is probably shutdown)
	if (autodisp)
		return 0;

	if(gCommsMgr != NULL)
	{
		VuGameEntity	*game = (VuGameEntity*) vuDatabase->Find(dataBlock.game);
		gCommsMgr->LookAtGame(game);

		if(dataBlock.from != vuLocalSessionEntity->Id())
			{
			SimDogfight.ReceiveSettings(&dataBlock.settings);
			PostMessage(FalconDisplay.appWin,FM_REFRESH_DOGFIGHT,0,0);
			}
	}
	return TRUE;
}

