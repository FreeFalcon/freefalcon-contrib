/*
 * Machine Generated include file for message "Sim Data Toggle".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 01-April-1997 at 18:57:03
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#ifndef _SIMDATATOGGLE_H
#define _SIMDATATOGGLE_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Sim Data Toggle
 */
class FalconSimDataToggle : public FalconEvent
{
   public:
      FalconSimDataToggle(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconSimDataToggle(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconSimDataToggle(void);
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

            VU_ID entityID;
            int flag ;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
