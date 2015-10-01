#include "MeterTime.h"

MeterTime::MeterTime(void)
{
	rtc = new MCP7940RTC();
	rtc->setBattEn();
        rtc->setSquareWaveOutput();
}

void MeterTime::setNewTimeRTC(time_t newTime)
{
	this->rtc->set(newTime);
}

void MeterTime::setNewTimeRTC(int yr, int mo, int dy, int hr, int mn, int sec) {
	tmElements_t tm1;
	tm1.Year     =tmYearToY2k(CalendarYrToTm(yr));
	tm1.Month     =mo;
	tm1.Day       =dy;
	tm1.Hour      =hr;
	tm1.Minute    =mn;
	tm1.Second    =sec;
	time_t t = makeTime(tm1);
	this->rtc->set(t);
}

void MeterTime::syncWithNTPTime(StringList& timeServers)
{
	time_t serverTime = this->getNTPUnixTime(timeServers);
	setNewTimeRTC(serverTime);
}

tmElements_t MeterTime::getTimeStamp()
{
	tmElements_t tm;
	this->rtc->read(tm);
	return tm;
}

time_t MeterTime::getNTPUnixTime(StringList& timeServers)
	//Some parts of the code come from http://playground.arduino.cc/Code/NTPclient.
{
	uint8_t i;
	time_t time = 0;// NTP time
	EthernetUDP udp;
	int udpInited = udp.begin(NTP_PORT);
        char* timeServer;

	// Only the first four bytes of an outgoing NTP packet need to be set
	// appropriately, the rest can be whatever.
	const uint32_t ntpFirstFourBytes = 0xEC0600E3; // NTP request header

	if(udpInited)
	{
		// Clear received data from possible stray received packets
		udp.flush();

		i = 0;
                timeServer = (char*)malloc((timeServers[i].length()+1)*sizeof(char));
                timeServers[i].toCharArray(timeServer, timeServers[i].length()+1);
		while(! (udp.beginPacket(timeServer, NTP_PORT)
					&& udp.write((byte *)&ntpFirstFourBytes, 48) == 48
					&& udp.endPacket()) && (i < timeServers.getSize()))
		{
			//Try next server if the current one was unsuccessful
			i++;
                        free(timeServer);
                        timeServer = (char*)malloc((timeServers[i].length()+1)*sizeof(char));
                        timeServers[i].toCharArray(timeServer, timeServers[i].length()+1);
		}
                free(timeServer);

		//If a time server was connected successfully
		if(i < timeServers.getSize())
		{
			// Wait for response; check every pollIntv ms up to maxPoll times
			const int pollIntv = 150;		// poll every this many ms
			const byte maxPoll = 15;		// poll up to this many times
			int pktLen;				// received packet length
			for (i=0; i<maxPoll; i++) {
				if ((pktLen = udp.parsePacket()) == 48)
					break;
				delay(pollIntv);
			}

			if(pktLen == 48)
			{
				// Read and discard the first useless bytes
				// Set useless to 32 for speed; set to 40 for accuracy.
				const byte useless = 40;
				for (i = 0; i < useless; ++i)
					udp.read();

				// Read the integer part of sending time
				time = udp.read();
				for (i = 1; i < 4; i++)
					time = time << 8 | udp.read();

				// Round to the nearest second if we want accuracy
				// The fractionary part is the next byte divided by 256: if it is
				// greater than 500ms we round to the next second; we also account
				// for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
				// additionally, we account for how much we delayed reading the packet
				// since its arrival, which we assume on average to be pollIntv/2.
				time += (udp.read() > 115 - pollIntv/8);

				// Discard the rest of the packet
				udp.flush();
				time -= 2208988800ul; // convert NTP time to Unix time
			}
		}
	}
	return time;		
}
