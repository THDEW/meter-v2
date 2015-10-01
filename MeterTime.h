#ifndef __METER_TIME__
#define __METER_TIME__

#include <Arduino.h>
#include <Ethernet.h>
#include "MCP7940RTC.h"
#include "StringList.h"

#define NTP_PORT 123

class MeterTime
{
	private:
		MCP7940RTC *rtc;
	public:
		MeterTime(); //Setup the time of the RTC chip, including NTP sync.
		void setNewTimeRTC(int yr, int mo, int dy, int hr, int mn, int sec); 
		void setNewTimeRTC(time_t newTime);
		time_t getNTPUnixTime(StringList& ntpServers);
		tmElements_t getTimeStamp();
		void syncWithNTPTime(StringList& ntpServers);
};
#endif
