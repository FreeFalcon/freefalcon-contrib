/*
 * Machine Generated source file for message "Send UI Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 29-November-1998 at 12:42:57
 * Generated from file EVENTS.XLS by Pepe
 */

#include "MsgInc\SendUIMsg.h"
#include "mesg.h"
#include "ui\include\uicomms.h"

extern void TacEngSetVCCompleted(long ID, int value);
extern void TacEngGameOver (void);

UISendMsg::UISendMsg(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (SendUIMsg, FalconEvent::SimThread, entityId, target, loopback)
{
   // Your Code Goes Here
}

UISendMsg::UISendMsg(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (SendUIMsg, FalconEvent::SimThread, senderid, target)
{
   // Your Code Goes Here
	type;
}

UISendMsg::~UISendMsg(void)
{
   // Your Code Goes Here
}

int UISendMsg::Process(uchar autodisp)
{
	switch(dataBlock.msgType)
	{
		case VC_Update:
		{
			if(dataBlock.from == FalconLocalSessionId)
			{
				break;
			}

			TacEngSetVCCompleted(dataBlock.number, dataBlock.value);
			break;
		}

		case VC_GameOver:
		{
			TacEngGameOver ();
			break;
		}

		case UpdateIter:
		{
			if(dataBlock.from ==  FalconLocalSessionId)
			{
				break;
			}

			if(gCommsMgr)
			{
				gCommsMgr->RemoteUpdateIter(dataBlock.value);
			}

			break;
		}
	}
	return 0;
	autodisp;
}

