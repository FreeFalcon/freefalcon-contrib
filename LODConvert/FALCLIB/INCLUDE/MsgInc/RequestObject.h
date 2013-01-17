/*
 * Machine Generated include file for message "Request Object".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 25-May-1998 at 00:48:26
 * Generated from file EVENTS.XLS by Microprose
 */

#ifndef _REQUESTOBJECT_H
#define _REQUESTOBJECT_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Request Object
 */
class FalconRequestObject : public FalconEvent
{
   public:
      FalconRequestObject(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconRequestObject(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconRequestObject(void);
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

            VU_ID fromID;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
