/*
 * Machine Generated include file for message "ATC Command".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 11-March-1999 at 15:34:45
 * Generated from file EVENTS.XLS by MicroProse
 */

#ifndef _ATCCMDMSG_H
#define _ATCCMDMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type ATC Command
 */
class FalconATCCmdMessage : public FalconEvent
{
   public:
      enum ATCCmdCode {
         TakePosition,
         EmergencyHold,
         Hold,
         Abort,
         ToFirstLeg,
         ToBase,
         ToFinal,
         OnFinal,
         ClearToLand,
         Landed,
         TaxiOff,
         EmerToBase,
         EmerToFinal,
         EmerOnFinal,
         EmergencyStop,
         TaxiBack,
         Taxi,
         Wait,
         HoldShort,
         PrepToTakeRunway,
         TakeRunway,
         Takeoff,
         Divert,
         Release,
         ExitSim};

      FalconATCCmdMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconATCCmdMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconATCCmdMessage(void);
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
            unsigned int type;
            long rwtime;
            short rwindex;
            VU_ID follow;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
