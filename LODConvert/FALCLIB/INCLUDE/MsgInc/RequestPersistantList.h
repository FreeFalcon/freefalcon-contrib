/*
 * Machine Generated include file for message "Request Persistant List".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 29-July-1997 at 18:41:01
 * Generated from file EVENTS.XLS by Kevin Klemmick
 */

#ifndef _REQUESTPERSISTANTLIST_H
#define _REQUESTPERSISTANTLIST_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Request Persistant List
 */
class FalconRequestPersistantList : public FalconEvent
{
   public:
      FalconRequestPersistantList(VU_ID entityId, VU_ID dest, VU_BYTE routing);
      FalconRequestPersistantList(VU_ID entityId, VU_BYTE routing);
      ~FalconRequestPersistantList(void);
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

            VU_ID who;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
