#include "OtherDef.h"
#include "ConfigHandler.h"
#include "MeterTime.h"
#include "MeasurementData.h"
#include "Measurement.h"

ConfigHandler *conf;
MeterTime *mTime;
DataAcquisition *daq;
volatile uint8_t samplingSecond;
tmElements_t test;
uint8_t a;

void sampleISR()
{
	samplingSecond++;
        //Serial.println("samplingSecond: "+String(samplingSecond));
        //Serial.println("conf->sampFreq: "+String(conf->sampFreq));
	if(!(samplingSecond % conf->sampFreq))
	{
		//Sample data and calculate Vrms, Irms, P, and associate them with a time stamp
		uint8_t i;
		uint16_t voltChValue, currentChValue;
		double rmsVolt, rmsCurrent, realPower;
		for(i=0; i<CT_CHNLS; i++)
		{
			//Continue the process if the current channel is connected; otherwise, try the next channel.
                        //Serial.println("daq->isConnected("+String(i)+"): "+String(daq->isConnected(i)));
			if(daq->isConnected(i))
			{
                                //Serial.println("1111");
				daq->selectChannel(i);
                                //Serial.println("2222");
				daq->findStartingPoint(voltChValue, currentChValue); //Find the starting point of the signal to calculate Vrms, Irms, and P.
                                //Serial.println("3333");	
				daq->accumulateSumSquare(VOLT_TYPE, voltChValue);
                                //Serial.println("4444");
				daq->accumulateSumSquare(CURRENT_TYPE, currentChValue);
                                //Serial.println("5555");
				daq->accumulateRealPower(voltChValue, currentChValue, conf->voltCF, conf->currCFs[i].cf);
                                //Serial.println("6666");
				while(daq->getVoltZeroCrosses() < WAVE_NUM_CROSS && daq->getCurrentZeroCrosses() < WAVE_NUM_CROSS)
				{
                                        //Serial.println("7777");
					daq->sampleVoltageCurrent(voltChValue, currentChValue);
                                        //Serial.println("8888");
					daq->accumulateSumSquare(VOLT_TYPE, voltChValue);
                                        //Serial.println("9999");
					daq->accumulateSumSquare(CURRENT_TYPE, currentChValue);
                                        //Serial.println("AAAA");
					daq->accumulateRealPower(voltChValue, currentChValue, conf->voltCF, conf->currCFs[i].cf);
                                        //Serial.println("BBBB");
				}
                                //Serial.println("CCCC, "+String(daq->getVoltZeroCrosses())+ ", "+String(daq->getCurrentZeroCrosses()));
				if(daq->getVoltZeroCrosses() == WAVE_NUM_CROSS)
				{
					while(daq->getCurrentZeroCrosses() < WAVE_NUM_CROSS)
					{
						daq->sampleVoltageCurrent(voltChValue, currentChValue);
						daq->accumulateSumSquare(CURRENT_TYPE, currentChValue);
                                                //Serial.println("voltChValue1: "+String(voltChValue)+"currentChValue1: "+String(currentChValue));
                                                //Serial.println("daq->getCurrentZeroCrosses()1: "+String(daq->getCurrentZeroCrosses()));
					}
				}
				else if(daq->getCurrentZeroCrosses() == WAVE_NUM_CROSS)
				{
					while(daq->getVoltZeroCrosses() < WAVE_NUM_CROSS)
					{
						daq->sampleVoltageCurrent(voltChValue, currentChValue);
						daq->accumulateSumSquare(VOLT_TYPE, voltChValue);
                                                //Serial.println("voltChValue2: "+String(voltChValue)+"currentChValue2: "+String(currentChValue));
                                                //Serial.println("daq->getVoltZeroCrosses()2: "+String(daq->getVoltZeroCrosses()));
					}
				}
                                //Serial.println("9999999999999999==============");
				DataEntry data;
				data.channel = i;
				data.timestamp = mTime->getTimeStamp();
				daq->calculateRMSVoltage(data.vrms, conf->voltCF);
                                //Serial.println("conf->currCFs["+String(i)+"].cf: "+String(conf->currCFs[i].cf,6));
				daq->calculateRMSCurrent(data.irms, conf->currCFs[i].cf);
				daq->averageRealPower(data.realPower);

                                /*Serial.print("CT Sample Times: ");
                                for(a=0; a<daq->getLatestCurrentSamples(); a++)
                                {
                                        Serial.print(String(a)+":"+String(daq->getCSampleTimes(a))+",");
                                }
                                Serial.print("\nVT Sample Times: ");
                                for(a=0; a<daq->getLatestVoltSamples(); a++)
                                {
                                        Serial.print(String(a)+":"+String(daq->getVSampleTimes(a))+",");
                                }
                                Serial.print("\n");*/

				//write data to SD
				MeasurementData mData;
				mData.openForWrite();
				mData.appendEntry(data);
				mData.closeDataFile();
			}
		}
		/*if(!(samplingSecond % SECONDS_TO_PUSH))
		{
			//Push data in SD to server and delete the file.
			MeasurementData mData;
			DataEntry dataList[ENTRIES_SENT];
			uint16_t i;
			mData.openForRead();
			i = 0;
			while(mData.readEntry(dataList[i]))
			{
				mData.readEntry(dataList[i]);
				i++;
				if(i == ENTRIES_SENT)
				//Dispatch the data to the server when the dataList is full.
				{
					i = 0;  //reset the counter
				}
			}
			samplingSecond = 0;
		}*/
	}
}

void testISR()
{
        detachInterrupt(0);
        uint32_t i = 0;
        a++;
        /*tmElements_t temp = mTime->getTimeStamp();
        test.Hour = temp.Hour;
        test.Minute = temp.Minute;
        test.Second = temp.Second;*/
        test.Hour = test.Hour + 1;
        test.Minute = test.Minute + 1;
        test.Second = test.Second + 1;
        /*while(i < 4000000)
                i++;*/
        //delayMicroseconds(3000000);
        //Serial.println("nnnnnnnnnnnnnnnn");
}

void startUp()
{
	HW_SS_DDR |= (1 << HW_SS); //SPI Hardware pin (digital 53) to be output
	//ETH_SS_DDR &= ~(1 << ETH_SS); //Ethernet slave select pin to be output
	//SD_SS_DDR &= ~(1 << SD_SS); //SD card slave select pin to be output
	//VTCT_SS_DDR &= ~(1 << VTCT_SS); //VTCT slave select pin to be output
	SD.begin(SD_SS_PIN);
	if(!SD.exists(CONF_DIR))
		SD.mkdir(CONF_DIR);
	
	if(!SD.exists(DATA_DIR))
		SD.mkdir(DATA_DIR);

	conf = new ConfigHandler();
        #ifdef DEBUG
        //Serial.println("Start readAddress-----------------------");
        #endif
	conf->readAddress();
	conf->readVoltageCF();
	conf->readCurrentCF();
        //Serial.println("========currCFs[0].cf: "+String(conf->currCFs[0].cf));
	conf->readSamplingFrequency();
        #ifdef DEBUG
        //Serial.println("+++++++++++++readTimeServer++++++++");
        #endif
	conf->readTimeServer();
        #ifdef DEBUG
        //Serial.println("+++++++++++++readMQTTServer++++++++");
        #endif
        conf->readMQTTServer();
	mTime = new MeterTime();
	//mTime->syncWithNTPTime(conf->timeServers);
	samplingSecond = 0;
	daq = new DataAcquisition();
	//attachInterrupt(SAMPLE_INT, sampleISR, RISING);
        //attachInterrupt(SAMPLE_INT, testISR, RISING);
}
