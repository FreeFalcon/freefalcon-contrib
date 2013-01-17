/*
 * Machine Generated source file for message "Update Settings".
 * NOTE: The functions here must be completed by hand.
 * Generated on 25-July-1997 at 11:34:34
 * Generated from file EVENTS.XLS by MIS
 */

#include "MsgInc\UpdateSettings.h"
#include "mesg.h"
#include "uicomms.h"

/*
UI_UpdateSettings::UI_UpdateSettings(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (UpdateSettings, FalconEvent::UIThread, entityId, target, loopback)
{
	RequestOutOfBandTransmit();
}

UI_UpdateSettings::UI_UpdateSettings(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (UpdateSettings, FalconEvent::UIThread, senderid, target)
{
   // Your Code Goes Here
}

UI_UpdateSettings::~UI_UpdateSettings(void)
{
   // Your Code Goes Here
}

int UI_UpdateSettings::Process(uchar autodisp)
{
	if(dataBlock.from != vuLocalSessionEntity->Id())
	{
		if(gCommsMgr == NULL)
			return(0);

		gCommsMgr->UpdateSetting(dataBlock.setting,dataBlock.value);
	}
	return(0);
}
*/
