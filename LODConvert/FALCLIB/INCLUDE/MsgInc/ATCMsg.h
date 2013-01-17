/*
 * Machine Generated include file for message "ATCMsg".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 13-October-1998 at 20:35:59
 * Generated from file EVENTS.XLS by Microprose
 */

#ifndef _ATCMSG_H
#define _ATCMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#include "aircrft.h" //TJL 08/16/04 //Cobra 10/31/04 TJL
#pragma pack (1)

#define STRENGTH_PCT_FOR_EMERG_LDG	0.4f

/*
 * Message Type ATCMsg
 */
class FalconATCMessage : public FalconEvent
{
   public:
      enum ATCMsgCode {
         RequestClearance,
         RequestEmerClearance,
         ContactApproach,
         RequestTakeoff,
         RequestTaxi,
         UpdateStatus,
		 AbortApproach,
		 TrafficInSight,
		 RequestHotpitRefuel};	// M.N.	
								//TrafficInSight RAS-22Jan04-Acknowledge Traffic calls
								//Cobra 10/31/04 TJL

      FalconATCMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconATCMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconATCMessage(void);
      int Size (void) { return sizeof(dataBlock) + FalconEvent::Size();};
      int Decode (VU_BYTE **buf, int length)
         {
         int size;

            size = FalconEvent::Decode (buf, length);
            memcpy (&dataBlock, *buf, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            size += sizeof (dataBlock);
            return size;
         };
      int Encode (VU_BYTE **buf)
         {
         int size;

            size = FalconEvent::Encode (buf);
            memcpy (*buf, &dataBlock, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            size += sizeof (dataBlock);
            return size;
         };
      class DATA_BLOCK
      {
         public:

            VU_ID from;
            unsigned int type;
            short status;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
