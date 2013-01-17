/*
 * Machine Generated include file for message "Naval Tasking Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 28-August-1997 at 17:24:21
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _NAVALTASKINGMSG_H
#define _NAVALTASKINGMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Naval Tasking Message
 */
class FalconNavalTaskingMessage : public FalconEvent
{
   public:
      enum NTMMsgType {
         ntmSupportRequest};

      FalconNavalTaskingMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconNavalTaskingMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconNavalTaskingMessage(void);
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
            VU_ID to;
            uchar team;
            unsigned int messageType;
            short data1;
            short data2;
            VU_ID enemy;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
