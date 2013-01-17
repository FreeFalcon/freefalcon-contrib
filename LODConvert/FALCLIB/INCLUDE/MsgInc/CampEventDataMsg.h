/*
 * Machine Generated include file for message "Camp Event Data".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 25-November-1998 at 11:57:47
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _CAMPEVENTDATAMSG_H
#define _CAMPEVENTDATAMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Camp Event Data
 */
class CampEventDataMessage : public FalconEvent
{
   public:
      enum eventMsgType {
         eventMessage,
         victoryConditionMessage,
		 playMovie };

      CampEventDataMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      CampEventDataMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~CampEventDataMessage(void);
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
            unsigned int message;
            short event;
            uchar status;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
