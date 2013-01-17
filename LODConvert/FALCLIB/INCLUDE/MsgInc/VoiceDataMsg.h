/*
 * Machine Generated include file for message "Voice Data Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 17-November-1997 at 21:35:38
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _VOICEDATAMSG_H
#define _VOICEDATAMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Voice Data Message
 */
class FalconVoiceDataMessage : public FalconEvent
{
   public:
      FalconVoiceDataMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconVoiceDataMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconVoiceDataMessage(void);
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

            ushort id;
            ushort size;
            uchar* data;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
