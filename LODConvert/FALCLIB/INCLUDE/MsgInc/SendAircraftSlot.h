/*
 * Machine Generated include file for message "Send aircraft Slot".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 03-November-1997 at 20:09:30
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _SENDAIRCRAFTSLOT_H
#define _SENDAIRCRAFTSLOT_H

/*
 * Required Include Files
 */

#include "F4vu.h"
#include "mission.h"
#pragma pack (1)

#define REQUEST_RESULT_SUCCESS		1
#define REQUEST_RESULT_DENIED		2

/*
 * Message Type Send aircraft Slot
 */

class UI_SendAircraftSlot : public FalconEvent
{
   public:
      UI_SendAircraftSlot(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      UI_SendAircraftSlot(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~UI_SendAircraftSlot(void);
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

            VU_ID requesting_session;
			VU_ID game_id;
			VU_ID host_id;
			uchar result;
            uchar game_type;
            uchar team;
            uchar got_slot;
			uchar got_pilot_slot;
			uchar got_pilot_skill;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
