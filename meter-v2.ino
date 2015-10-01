#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Ethernet.h>
#include "StringList.h"
#include "StartUp.h"

#define SIZE 600

File file;
uint16_t rounds, temp;
StringList sl;
DataEntry de, de1;
MeasurementData mData;
bool neof;

void setup()
{
        uint8_t i;
        
	Serial.begin(9600);
	while(!Serial);
        digitalWrite(53, HIGH);
        //Serial.println("HW_SS, HW_SS_DDR");
        //Serial.println(HW_SS);
        //Serial.println(HW_SS_DDR, HEX);
        //Serial.println("HW_SS, HW_SS_DDR END");
	/*if(!SD.begin(4)){
          Serial.println("failed to initialize.");
          return;
        }*/
        
	startUp();
        //MeterUtils::enableSDCard();
        //Serial.println("success to initialize.");
	rounds = 0;
        //if(!SD.exists("test"))
        //      SD.mkdir("test");
        SD.remove("test/test.dat");
        Serial.println("Meter Address");
        //Serial.println("aaa");
        Serial.println(String(conf->addr.ip[0])+"."+String(conf->addr.ip[1])+"."+String(conf->addr.ip[2])+"."+String(conf->addr.ip[3]));
        Serial.println(String(conf->addr.mac[0],HEX)+":"+String(conf->addr.mac[1],HEX)+":"+String(conf->addr.mac[2],HEX)+":"+String(conf->addr.mac[3],HEX)+":"+String(conf->addr.mac[4],HEX)+":"+String(conf->addr.mac[5],HEX));
        Serial.println("VoltCF");
        Serial.println(String(conf->voltCF, 3));
        Serial.println("CurrCFs");
        for(i=0; i<CT_CHNLS; i++)
        {
                if(conf->currCFs[i].channel != NOT_DEFINED)
                {
                        Serial.println("currCFs["+String(i)+"].channel: " + String(conf->currCFs[i].channel));
                        Serial.println("currCFs["+String(i)+"].cf: " + String(conf->currCFs[i].cf));
                }
        }
        Serial.println("Sampling Frequency: "+String(SAMPLING_FREQ_F));
        Serial.println(conf->sampFreq);
        Serial.println("Time Server");
        for(i=0; i<conf->timeServers.getSize(); i++)
        {
              Serial.println(conf->timeServers[i]);
        }
        Serial.println("MQTT Server");
        Serial.println(String(conf->mqttAddr[0])+"."+String(conf->mqttAddr[1])+"."+String(conf->mqttAddr[2])+"."+String(conf->mqttAddr[3]));
        
        
        mData.clearData();
        /*mData.openForWrite();
        de.channel = 0;
        de.timestamp = mTime->getTimeStamp();
        de.vrms = 220.1;
        de.irms = 1.32;
        de.realPower = 252.4;
        mData.appendEntry(de);
        
        delay(3000);
        
        de.channel = 11;
        de.timestamp = mTime->getTimeStamp();
        de.vrms = 218.34;
        de.irms = 0.254;
        de.realPower = 54.83;
        mData.appendEntry(de);
        mData.closeDataFile();
        Serial.println("After appending entries-------------");*/
        
        for(i=0; i<20; i++)
        {
                sampleISR();
                if(i % 10 == 0)
                        Serial.print("\n");
                Serial.print(String(i+1)+",");
                //delay((conf->sampFreq * 1000));
        }
        
        Serial.println("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
        
        neof = true;
        
        //pinMode(19, INPUT);
        
}

void loop()
{        
        /*Serial.print(String(test.Hour)+":");
        Serial.print(String(test.Minute)+":");
        Serial.print(String(test.Second)+" ; ");
        de.timestamp = mTime->getTimeStamp();
        Serial.print(String(de.timestamp.Second)+", ");
        Serial.println(String(digitalRead(19))+",,"+String(a));
        attachInterrupt(0, testISR, RISING);*/
        if(neof)
        {
        Serial.println("channel, hour, minute, seccond, vrms, irms, realPower, numSamples");
        mData.openForRead();
        while(neof)
        {
                if(neof)
                {
                        neof = mData.readEntry(de1);
                        Serial.print(String(de1.channel)+",");
                        Serial.print(String(de1.timestamp.Hour)+",");
                        Serial.print(String(de1.timestamp.Minute)+",");
                        Serial.print(String(de1.timestamp.Second)+",");
                        Serial.print(String(de1.vrms, 6)+",");
                        Serial.print(String(de1.irms, 6)+",");
                        Serial.print(String(de1.realPower, 6)+",");
                        Serial.println(String(daq->getPowerNumSamples()));
                }
        }
        
        mData.closeDataFile();
        
        delete conf;
        delete mTime;
        delete daq;
        }
        
        /*char* temp = NULL;
        String r;
        
        for(rounds=0; rounds < 10; rounds++)
        {
                //if(temp != NULL)
                //        free(temp);
                r = String(rounds);
                temp = (char*)malloc((r.length()+1)*sizeof(char));
                r.toCharArray(temp, r.length()+1);
	        sl.addString(temp);
        }
        //if(temp != NULL)
        //                free(temp);

        for(rounds=0; rounds < 10; rounds++)
	        Serial.println("sl["+String(rounds)+"]"+sl[rounds]);*/
}
