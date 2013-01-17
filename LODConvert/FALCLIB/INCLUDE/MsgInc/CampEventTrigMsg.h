/*
 * Machine Generated include file for message "CampEvent Triggered".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 18-February-1997 at 15:12:33
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#ifndef _CAMPEVENTTRIGMSG_H
#define _CAMPEVENTTRIGMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type CampEvent Triggered
 */
class FalconCampEventTrigMessage : public FalconEvent
{
   public:
      FalconCampEventTrigMessage(VU_ID entityId, VU_SESSION_ID dest);
      FalconCampEventTrigMessage(VU_ID entityId, VU_BYTE routing);
      ~FalconCampEventTrigMessage(void);
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
            ushort event;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
