/*
 * Machine Generated include file for message "Eject Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 24-September-1997 at 14:37:55
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _EJECTMSG_H
#define _EJECTMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type Eject Message
 */
class FalconEjectMessage : public FalconEvent
{
   public:
      FalconEjectMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconEjectMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconEjectMessage(void);
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
			 
			 VU_ID	ePlaneID;
//			 VU_ID	eEjectID;
			 VU_ID   eFlightID;
			 ushort	eCampID;
//          ushort	eIndex;
			 uchar	ePilotID;
//			 uchar	eSide;
			 uchar	hadLastShooter;
     } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
