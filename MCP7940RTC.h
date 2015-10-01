/*
 * MCP7940RTC.h - library for MCP7940N RTC
 * This library is intended to be uses with Arduino Time.h library functions
 * Modified March 27, 2015 by Nattachart Tamkittikhun
 */

#ifndef MCP7940RTC_h
#define MCP7940RTC_h

//#include <Arduino.h>
#include "Time.h"
#include <Wire.h>

#define SEC_MSK 0
#define MIN_MSK 1
#define HOUR_MSK 2
#define WKD_MSK 3
#define DAY_MSK 4
#define ALL_MSK 7

typedef uint8_t byte;

// library interface description
class MCP7940RTC
{
	// user-accessible "public" interface
	public:
		MCP7940RTC();
		static time_t get();
		//static time_t getAlarm(int alarm);

		static void set(time_t t);
		static void setAlarm0(time_t t, uint8_t alarmMask);
		static void clearAlarm0();
		static void setSquareWaveOutput();
		static void clearSquareWaveOutput();
		static void setBattEn();
		static void clearBattEn();
		//static void setAlarm(int alarm, time_t t);
		//static void setAlarmCtl(int alarm, int params);

		static void read(tmElements_t &tm);
		static void write(tmElements_t &tm);

		static uint8_t getSecond();
		static uint8_t getMinute();
		static uint8_t getHour();
		static uint8_t getDay();
		static uint8_t getDayOfWeek();
		static uint8_t getMonth();
		static uint8_t getYear();

	private:
		static uint8_t dec2bcd(uint8_t num);
		static uint8_t bcd2dec(uint8_t num);
		static void clearControlBits();
};

#endif


