/*
 * Machine Generated include file for message "Timing Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 23-September-1997 at 17:52:26
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _TIMINGMSG_H
#define _TIMINGMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Timing Message
 */
class FalconTimingMessage : public FalconEvent
{
   public:
      FalconTimingMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
	  FalconTimingMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconTimingMessage(void);
      int Size (void) { return sizeof(dataBlock) + FalconEvent::Size();};
      int Decode (VU_BYTE **buf, int length);
      int Encode (VU_BYTE **buf);

	  class DATA_BLOCK
      {
         public:
#ifdef FALCON_TRACK_LATENCY
            ulong masterPostTime;
            ulong slaveReceiveTime;
            ulong slavePostTime;
            VU_ID from;
            ulong latency;
#endif
			ulong targetTime;
			char  compressionRatio;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
