/*
 * Machine Generated include file for message "Data Link Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 18-April-1998 at 12:10:10
 * Generated from file EVENTS.XLS by Microprose
 */

#ifndef _DLINKMSG_H
#define _DLINKMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type Data Link Message
 */
class FalconDLinkMessage : public FalconEvent
{
   public:
      enum DLinkPointType {
         NODLINK,
         IP,
         TGT,
         EGR,
         CP};

      FalconDLinkMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconDLinkMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconDLinkMessage(void);
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

            uchar numPoints;
            ushort targetType;
            ushort threatType;
            unsigned int ptype[5];
            short px[5];
            short py[5];
            short pz[5];
            long arrivalTime[5];
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
