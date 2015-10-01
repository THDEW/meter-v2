#ifndef __CONF_HANDLER__
#define __CONF_HANDLER__

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>
#include "MeterUtils.h"
#include "StringList.h"
#include "OtherDef.h"

#define ADDR_F CONF_DIR "/NETADDR.CNF"
#define VOLT_CF_F CONF_DIR "/VOLTCF.CNF"
#define CURRENT_CF_F CONF_DIR "/CURRCF.CNF"
#define SAMPLING_FREQ_F CONF_DIR "/SAMPFREQ.CNF"
#define TIME_SERVER_F CONF_DIR "/TIMESRV.CNF"
#define MQTT_SERVER_F CONF_DIR "/MQTTSRV.CNF"
#define DOUBLE_MAX_DIGITS 10

#define NOT_DEFINED 100

struct MeterAddress
{
	uint8_t ip[4]; //IPv4
	uint8_t mac[6]; //MAC Address
};

struct CurrentCF
{
	uint8_t channel;
	double cf;
};

class CurrentCFList {
        private:
		CurrentCF* currentCFlist;
		int size;
	public:	
		CurrentCFList(void);
		~CurrentCFList(void);
		void addCurrentCF(CurrentCF item);  //copy
		void setCurrentCF(CurrentCF item, int index); //copy
		void removeCurrentCF(int index);
		void emptyList();
		int getSize();
                CurrentCF operator[](size_t idx); //copy
};

class ConfigHandler
{
	private:
                double voltDividorFactorVT;
	public:
		MeterAddress addr;
		double voltCF;
		CurrentCF currCFs[CT_CHNLS];
		uint8_t sampFreq;
		StringList timeServers;
		uint8_t mqttAddr[4]; //IP address of MQTT server

		ConfigHandler();
		~ConfigHandler();
		void readAddress();
		void readVoltageCF();
		void readCurrentCF();
		void readSamplingFrequency();
		void readTimeServer();
		void readMQTTServer();

		void writeAddress(MeterAddress &addr);
		void writeVoltageCF(double voltCF);
                void writeCurrentCFs(CurrentCF* currCFs);
                void writeSamplingFrequency(uint8_t sampFreq);
                void writeTimeServers(StringList timeServers);
};
#endif
