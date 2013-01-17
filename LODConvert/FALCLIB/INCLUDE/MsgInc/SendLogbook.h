/*
 * Machine Generated include file for message "Send Logbook".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 08-November-1998 at 16:52:27
 * Generated from file EVENTS.XLS by PeterW
 */

#ifndef _SENDLOGBOOK_H
#define _SENDLOGBOOK_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Send Logbook
 */
class UI_SendLogbook : public FalconEvent
{
   public:
      UI_SendLogbook(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      UI_SendLogbook(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~UI_SendLogbook(void);
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
            LB_PILOT Pilot;
            long Flags;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
