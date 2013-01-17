/*
 * Machine Generated include file for message "Send Dogfight Info".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 03-April-1997 at 02:36:21
 * Generated from file EVENTS.XLS by PeterW
 */

#ifndef _SENDDOGFIGHTINFO_H
#define _SENDDOGFIGHTINFO_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "falcmesg.h"
#include "mission.h"
#include "dogfight.h"
#pragma pack (1)

/*
 * Message Type Send Dogfight Info
 */
class UI_SendDogfightInfo : public FalconEvent
{
   public:
      UI_SendDogfightInfo(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      UI_SendDogfightInfo(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~UI_SendDogfightInfo(void);
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

            VU_ID			from;
			VU_ID			game;
            DogfightClass	settings;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
