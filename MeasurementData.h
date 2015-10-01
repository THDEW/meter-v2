#ifndef __MEAS_DATA__
#define __MEAS_DATA__

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Time.h"

#define DATA_F DATA_DIR "/MEASDAT.DAT"

struct DataEntry
{
	uint8_t channel; //1 byte
	tmElements_t timestamp; //7 bytes
	double vrms; //4 bytes
	double irms; //4 bytes
	double realPower; //4 bytes
}; //20 bytes

class MeasurementData
{
	private:
		File dataF;
	public:
		DataEntry data;	

		void openForRead();
		void openForWrite();
		bool readEntry(DataEntry& data); //Returns boolean indicating EOF.
		void appendEntry(DataEntry& data);

		void clearData(); //Delete all data in the measurement data file

		void closeDataFile();
};

#endif
