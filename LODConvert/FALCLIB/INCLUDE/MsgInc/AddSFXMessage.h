/*
 * Machine Generated include file for message "Add SFX Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 29-July-1997 at 17:49:00
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#ifndef _ADDSFXMESSAGE_H
#define _ADDSFXMESSAGE_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "falcmesg.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type Add SFX Message
 */
class FalconAddSFXMessage : public FalconEvent
{
   public:
      FalconAddSFXMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconAddSFXMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconAddSFXMessage(void);
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

            short type;
            short visType;
            short flags;
            CampaignTime time;
            float xLoc;
            float yLoc;
            float zLoc;
            float xVel;
            float yVel;
            float zVel;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
