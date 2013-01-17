/*
 * Machine Generated include file for message "Regeneration Msg".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 08-July-1998 at 22:37:56
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _REGENERATIONMSG_H
#define _REGENERATIONMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (push, pack1, 1)

/*
 * Message Type Regeneration Msg
 */
class FalconRegenerationMessage : public FalconEvent
{
   public:
      FalconRegenerationMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconRegenerationMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconRegenerationMessage(void);
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

            float newx;
            float newy;
            float newz;
            float newyaw;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack (pop, pack1)

#endif
