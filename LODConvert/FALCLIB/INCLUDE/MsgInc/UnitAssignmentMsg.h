/*
 * Machine Generated include file for message "Unit Assignment".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 28-August-1997 at 17:24:21
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _UNITASSIGNMENTMSG_H
#define _UNITASSIGNMENTMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type Unit Assignment
 */
class FalconUnitAssignmentMessage : public FalconEvent
{
   public:
      FalconUnitAssignmentMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconUnitAssignmentMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconUnitAssignmentMessage(void);
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
            short orders;
            VU_ID poid;
            VU_ID soid;
            VU_ID roid;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
