/*
 * Machine Generated include file for message "Send Image".
 * NOTE: This file is read only. DO NOT ATTEMPT TO MODIFY IT BY HAND.
 * Generated on 08-November-1998 at 16:52:27
 * Generated from file EVENTS.XLS by PeterW
 */

#ifndef _SENDIMAGE_H
#define _SENDIMAGE_H

/*
 * Required Include Files
 */
#include "F4vu.h"
#include "FalcMesg.h"
#include "mission.h"
#pragma pack (1)

/*
 * Message Type Send Image
 */
class UI_SendImage : public FalconEvent
{
   public:
      UI_SendImage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback=TRUE);
      UI_SendImage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target);
      ~UI_SendImage(void);
      int Size (void)
		{
			int size=FalconEvent::Size();
			size += sizeof(VU_ID) +
					sizeof(uchar) +
					sizeof(short) +
					sizeof(short) +
					sizeof(long) +
					sizeof(long) +
					dataBlock.blockSize;
			return(size);
		}

      int Decode (VU_BYTE **buf, int length)
         {
         int size;

            size = FalconEvent::Decode (buf, length);
            memcpy (&dataBlock.fromID, *buf, sizeof (VU_ID));	*buf += sizeof (VU_ID);		size += sizeof (VU_ID);
            memcpy (&dataBlock.typeID, *buf, sizeof (uchar));	*buf += sizeof (uchar);		size += sizeof (uchar);
            memcpy (&dataBlock.blockNo, *buf, sizeof (short));	*buf += sizeof (short);		size += sizeof (short);
            memcpy (&dataBlock.blockSize, *buf, sizeof (short));*buf += sizeof (short);		size += sizeof (short);
            memcpy (&dataBlock.offset, *buf, sizeof (long));	*buf += sizeof (long);		size += sizeof (long);
            memcpy (&dataBlock.size, *buf, sizeof (long));		*buf += sizeof (long);		size += sizeof (long);
			dataBlock.data=new uchar[dataBlock.blockSize];
            memcpy (dataBlock.data, *buf, dataBlock.blockSize);	*buf += dataBlock.blockSize;size += dataBlock.blockSize;
            return size;
         };
      int Encode (VU_BYTE **buf)
         {
         int size;

            size = FalconEvent::Encode (buf);
            memcpy (*buf, &dataBlock.fromID, sizeof (VU_ID));	*buf += sizeof (VU_ID);		size += sizeof (VU_ID);
            memcpy (*buf, &dataBlock.typeID, sizeof (uchar));	*buf += sizeof (uchar);		size += sizeof (uchar);
            memcpy (*buf, &dataBlock.blockNo, sizeof (short));	*buf += sizeof (short);		size += sizeof (short);
            memcpy (*buf, &dataBlock.blockSize, sizeof (short));*buf += sizeof (short);		size += sizeof (short);
            memcpy (*buf, &dataBlock.offset, sizeof (long));	*buf += sizeof (long);		size += sizeof (long);
            memcpy (*buf, &dataBlock.size, sizeof (long));		*buf += sizeof (long);		size += sizeof (long);
            memcpy (*buf, dataBlock.data, dataBlock.blockSize);	*buf += dataBlock.blockSize;size += dataBlock.blockSize;
            return size;
         };
      class DATA_BLOCK
      {
         public:
			VU_ID fromID;
            uchar typeID;
            short blockNo;
			short blockSize;
			long  offset;
            long  size;
			uchar *data;
      } dataBlock;

   protected:
      int Process(uchar autodisp);
};
#pragma pack ()

#endif
