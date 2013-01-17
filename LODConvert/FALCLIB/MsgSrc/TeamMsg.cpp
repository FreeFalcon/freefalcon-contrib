/*
 * Machine Generated source file for message "Team Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 23-October-1996 at 13:34:00
 * Generated from file EVENTS.XLS by KEVINK
 */

#include "MsgInc\TeamMsg.h"
#include "mesg.h"
#include "Team.h"

FalconTeamMessage::FalconTeamMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback) : FalconEvent (TeamMsg, FalconEvent::CampaignThread, entityId, target, loopback)
	{
	}

FalconTeamMessage::FalconTeamMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target) : FalconEvent (TeamMsg, FalconEvent::CampaignThread, senderid, target)
	{
	type;
	}

FalconTeamMessage::~FalconTeamMessage(void)
	{
	}

int FalconTeamMessage::Process(uchar autodisp)
	{
	if (autodisp)
		return 0;

	switch (dataBlock.messageType)
		{
		case teamRelationsChange:
		  	TeamInfo[dataBlock.team]->stance[dataBlock.actor] = dataBlock.value;
			TheCampaign.MakeCampMap(MAP_OWNERSHIP);
			PostMessage(FalconDisplay.appWin,FM_REFRESH_CAMPMAP,0,0);
			break;
		case teamNewMember:
			if (dataBlock.value)			// quit
				TeamInfo[dataBlock.team]->member[dataBlock.actor] = 0;
			else
				TeamInfo[dataBlock.team]->member[dataBlock.actor] = 1;
			TheCampaign.MakeCampMap(MAP_OWNERSHIP);
			PostMessage(FalconDisplay.appWin,FM_REFRESH_CAMPMAP,0,0);
			break;
		default:
			break;
		}
	return 0;
	}

