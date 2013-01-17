/*
 * Machine Generated source file for message "Weather Message".
 * NOTE: The functions here must be completed by hand.
 * Generated on 06-February-1997 at 12:20:23
 * Generated from file EVENTS.XLS by Leon Rosenshein
 */
//JAM 20Nov03

#include "MsgInc\WeatherMsg.h"
#include "mesg.h"
#include "Weather.h"
#include "CmpClass.h"
#include "Weather.h"

FalconWeatherMessage::FalconWeatherMessage(VU_ID entityId, VuTargetEntity *target, VU_BOOL loopback)
 : FalconEvent(WeatherMsg, FalconEvent::CampaignThread, entityId, target, loopback)
{
	dataBlock.weatherCondition = SUNNY;
}

FalconWeatherMessage::FalconWeatherMessage(VU_MSG_TYPE type, VU_ID senderid, VU_ID target)
 : FalconEvent(WeatherMsg, FalconEvent::CampaignThread, senderid, target)
{
	dataBlock.weatherCondition = SUNNY;
}

FalconWeatherMessage::~FalconWeatherMessage(void)
{
}

int FalconWeatherMessage::Size(void) 
{
	return sizeof(dataBlock) + FalconEvent::Size();
}

int FalconWeatherMessage::Decode(VU_BYTE **buf, int length)
{
	int size = 0;
	long start = (long)*buf;

	memcpy(&dataBlock,*buf,sizeof(dataBlock));
	*buf += sizeof(dataBlock);
	size += sizeof(dataBlock);
	size += FalconEvent::Decode(buf,length);

	ShiAssert(size == (long)*buf - start);

	return size;
}

int FalconWeatherMessage::Encode(VU_BYTE **buf)
{
	int size = 0;

	memcpy(*buf,&dataBlock,sizeof(dataBlock));
	*buf += sizeof(dataBlock);
	size += sizeof(dataBlock);
	size += FalconEvent::Encode(buf);

	ShiAssert(size == Size());

	return size;
}

int FalconWeatherMessage::Process(uchar autodisp)
{
	if(autodisp || !TheCampaign.IsPreLoaded()) return -1;

	((WeatherClass *)realWeather)->ReceiveWeather(this);
	
	return 0;
}
