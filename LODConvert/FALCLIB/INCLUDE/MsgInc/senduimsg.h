/*
 * Machine Generated include file for message "Send UI Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 29-November-1998 at 12:42:57
 * Generated from file EVENTS.XLS by Pepe
 */

#ifndef _SENDUIMSG_H
#define _SENDUIMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Send UI Message
 */
class UISendMsg : public FalconEvent
{
   public:

		enum
		{
			UnknownType=0,
			VC_Update,
			VC_GameOver,
			UpdateIter,
		};

      UISendMsg(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      UISendMsg(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~UISendMsg(void);
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
            uchar msgType;
            long number;
            long value;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
