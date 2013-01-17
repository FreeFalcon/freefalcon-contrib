/*
 * Machine Generated include file for message "Request Campaign data".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 01-April-1997 at 18:56:59
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#ifndef _REQUESTCAMPAIGNMSG_H
#define _REQUESTCAMPAIGNMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Request Campaign data
 */
class FalconRequestCampaign : public FalconEvent
{
   public:
      FalconRequestCampaign(VU_ID entityId, VU_ID dest, VU_BYTE routing);
      FalconRequestCampaign(VU_ID entityId, VU_BYTE routing);
      ~FalconRequestCampaign(void);
      int Size (void) { return sizeof(dataBlock) + FalconEvent::Size();};
      int Decode (VU_BYTE **buf, int length)
         {
         int size;

            size = FalconEvent::Decode (buf, length);
            memcpy (&dataBlock, *buf, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            return size;
         };
      int Encode (VU_BYTE **buf)
         {
         int size;

            size = FalconEvent::Encode (buf);
            memcpy (*buf, &dataBlock, sizeof (dataBlock));
            *buf += sizeof (dataBlock);
            return size;
         };
      class DATA_BLOCK
      {
         public:

            ulong campTime;
            VU_ID requester_id;
            VU_ID requester_session;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
