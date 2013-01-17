/*
 * Machine Generated include file for message "AWACSMsg".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 13-October-1998 at 19:15:45
 * Generated from file EVENTS.XLS by Microprose
 */

#ifndef _AWACSMSG_H
#define _AWACSMSG_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type AWACSMsg
 */
class FalconAWACSMessage : public FalconEvent
{
   public:
      enum AWACSMsgCode {
         Wilco,
         Unable,
         Judy,
         RequestPicture,
         RequestHelp,
         RequestRelief,
         RequestDivert,
         RequestSAR,
         OnStation,
         OffStation,
         VectorHome,
         VectorToAltAirfield,
         VectorToTanker,
         VectorToPackage,
         VectorToThreat,
         VectorToTarget,
         GivePicture,
         CampAbortMission,
         CampAirCoverSend,
         CampAWACSOnStation,
         CampAWACSOffStation,
         CampRelievedStation,
         CampDivertCover,
         CampDivertIntercept,
         CampDivertCAS,
         CampDivertOther,
         CampDivertDenied,
         CampEndDivert,
         CampAircraftLaunch,
         CampInterceptCourse,
         CampFriendliesEngaged,
         DeclareAircraft,
	  	 VectorToCarrier}; // M.N.

      FalconAWACSMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      FalconAWACSMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~FalconAWACSMessage(void);
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
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
