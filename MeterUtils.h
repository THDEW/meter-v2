#ifndef __METER_UTILS__
#define __METER_UTILS__

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "StringList.h"
#include "OtherDef.h"

class MeterUtils
{
	public:
		static void split(String &str, char delimiter, StringList &results);
		static char getLine(File &f, String &line);
		static void enableEthernet();
		static void enableSDCard();
		static void enableVTCT();
		static void disableSDCard();
};

#endif
