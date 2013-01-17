/*
 * Machine Generated include file for message "FAC Message".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 26-August-1997 at 10:01:21
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */

#ifndef _FACMSG_H
#define _FACMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "falcmesg.h"
#include "mission.h"

#pragma pack (1)

/*
 * Message Type FAC Message
 */
class FalconFACMessage : public FalconEvent
{
   public:
      enum FACMsgCode {
         CheckIn,
         Wilco,
         Unable,
         Ready,
         In,
         Out,
         RequestMark,
         RequestTarget,
         RequestBDA,
         RequestLocation,
         RequestTACAN,
         HoldAtCP,
         FacSit,
         Mark,
         AttackBrief,
         NoTargets,
         GroundTargetBr,
         BDA,
         NoBDA,
         FriendlyFire,
         ReattackQuery,
         HartsTarget,
         HartsOpen,
         ScudLaunch,
         SanitizeLZ,
         AttackMyTarget,
         SendChoppers};

      FalconFACMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconFACMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconFACMessage(void);
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
            VU_ID caller;
            unsigned int type;
            VU_ID target;
            float data1;
            float data2;
            float data3;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};

#pragma pack ()

#endif
