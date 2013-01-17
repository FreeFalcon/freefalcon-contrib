/*
 * Machine Generated include file for message "Graphics Text Display".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 03-April-1997 at 17:25:17
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#ifndef _GRAPHICSTEXTDISPLAYMSG_H
#define _GRAPHICSTEXTDISPLAYMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type Graphics Text Display
 */
class GraphicsTextDisplay : public FalconEvent
{
   public:
      GraphicsTextDisplay(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      GraphicsTextDisplay(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~GraphicsTextDisplay(void);
      int Size (void) { return sizeof(dataBlock) + dataBlock.len + 1 + FalconEvent::Size();};
      int Decode (VU_BYTE **buf, int length)
         {
         int size;

            size = FalconEvent::Decode (buf, length);
            memcpy (&dataBlock, *buf, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            size += sizeof (dataBlock);
            dataBlock.msg = (char*)(*buf);
            *buf += dataBlock.len;
            size += dataBlock.len;
            *buf += 1;
            size += 1;
            return size;
         };
      int Encode (VU_BYTE **buf)
         {
         int size;

            size = FalconEvent::Encode (buf);
            memcpy (*buf, &dataBlock, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            size += sizeof (dataBlock);
            memcpy (*buf, dataBlock.msg, dataBlock.len);
            *buf += dataBlock.len;
            size += dataBlock.len;
            **buf = 0;
            *buf += 1;
            size += 1;
            return size;
         };
      class DATA_BLOCK
      {
         public:
            VU_ID target;
            int len;
            char* msg;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
