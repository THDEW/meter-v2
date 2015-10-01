#include "ConfigHandler.h"


ConfigHandler::ConfigHandler(void)
{
	voltDividorFactorVT = (DIV_R1 + DIV_R2) / (double)DIV_R2; //Multiplied to the voltage calibration factor to get the voltage at the primary of the VT
}

ConfigHandler::~ConfigHandler()
{
        //Serial.println("~ConfigHandler()");
	MeterUtils::disableSDCard();
}

void ConfigHandler::readAddress()
{
	File addrF;
	String ipStr;
	String macStr;
	StringList ipTokens;
	StringList macTokens;
	uint8_t i;
        char* tempToken = NULL;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	//Open for read
        if(SD.exists(ADDR_F))
        {
		addrF = SD.open(ADDR_F);

		MeterUtils::getLine(addrF, ipStr);
		MeterUtils::getLine(addrF, macStr);

		MeterUtils::split(ipStr, '.', ipTokens);
		MeterUtils::split(macStr, ':', macTokens);
                //Serial.println("ipStr: "+ipStr);
                //Serial.println("macStr: "+macStr);
		for(i=0; i<ipTokens.getSize(); i++)
		{
                        if(tempToken != NULL)
                        {
                                free(tempToken);
                                tempToken = NULL;
                        }
                        tempToken = (char*)malloc((ipTokens[i].length()+1)*sizeof(char));
                        ipTokens[i].toCharArray(tempToken, ipTokens[i].length()+1);
			this->addr.ip[i] = atoi(tempToken);
                        //Serial.println("this->addr.ip[i]: "+String(this->addr.ip[i]));
		}
                if(tempToken != NULL)
                {
                        free(tempToken);
                        tempToken = NULL;
                }

		for(i=0; i<macTokens.getSize(); i++)
		{
                        if(tempToken != NULL)
                        {
                                free(tempToken);
                                tempToken = NULL;
                        }
                        tempToken = (char*)malloc((macTokens[i].length()+1)*sizeof(char));
                        macTokens[i].toCharArray(tempToken, macTokens[i].length()+1);
			this->addr.mac[i] = atoi(tempToken);
                        //Serial.println("this->addr.mac[i]: "+String(this->addr.mac[i]));
		}
                if(tempToken != NULL)
                {
                        free(tempToken);
                        tempToken = NULL;
                }
		addrF.close();
                //Serial.println("End Read Address");
        }
        MeterUtils::disableSDCard();
        SPI.end();
        //Serial.println("End Read Address");
}

void ConfigHandler::readVoltageCF()
{
	File voltCFF;
	String voltCFStr;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	//Open for read
	if(SD.exists(VOLT_CF_F))
	{
		voltCFF = SD.open(VOLT_CF_F);

		MeterUtils::getLine(voltCFF, voltCFStr);

		this->voltCF = atof(voltCFStr.c_str())*voltDividorFactorVT;
		voltCFF.close();
	}
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::readCurrentCF()
{
	File currCFF;
	String currentCFStr;
	StringList currentCFTokens;
	CurrentCF tempCurr;
	uint32_t size;
        uint8_t i;
        char* tempToken = NULL;
        char eof;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
        #ifdef DEBUG
        Serial.println("CURRENT_CF_F"+String(CURRENT_CF_F));
        #endif
        for(i=0; i < CT_CHNLS; i++)
        {
	        currCFs[i].channel = NOT_DEFINED;
	}
	if(SD.exists(CURRENT_CF_F))
	{
		currCFF = SD.open(CURRENT_CF_F);
		size = currCFF.size();
                #ifdef DEBUG
                Serial.println("++++++++++++++++++++"+String(currCFF.size()));
                #endif
		while(eof != -1 && eof != F_LAST_CHAR)
		{
                        currentCFTokens = StringList();
                        #ifdef DEBUG
                        Serial.println("------"+String(currCFF.position()));
                        #endif
			eof = MeterUtils::getLine(currCFF, currentCFStr);
                        //Serial.println("eof: "+String(eof));
                        //if(c != -1 && c != F_LAST_CHAR)
                        //{
				MeterUtils::split(currentCFStr, ',', currentCFTokens);
				#ifdef DEBUG
				//Serial.println("------"+String(currCFF.position()));
				//Serial.println("currentCFStr: "+currentCFStr);
				#endif
				if(tempToken != NULL)
                                {
                                        free(tempToken);
                                        tempToken = NULL;
                                }
				tempToken = (char*)malloc((currentCFTokens[0].length()+1)*sizeof(char));
				currentCFTokens[0].toCharArray(tempToken, currentCFTokens[0].length()+1);
				#ifdef DEBUG
				Serial.println("tempToken1: "+String(tempToken));
				#endif
				tempCurr.channel = (uint8_t)atoi(tempToken);
				if(tempToken != NULL)
                                {
                                        free(tempToken);
                                        tempToken = NULL;
                                }
				tempToken = (char*)malloc((currentCFTokens[1].length()+1)*sizeof(char));
				currentCFTokens[1].toCharArray(tempToken, currentCFTokens[1].length()+1);
				#ifdef DEBUG
				Serial.println("tempToken2: "+String(tempToken));
				#endif
				tempCurr.cf = atof(tempToken);
				currCFs[tempCurr.channel] = tempCurr; //Assume the channel is in range [0,14]
                                //Serial.println("currCFs[0,"+String(tempCurr.channel)+"].cf: "+String(currCFs[0].cf));
		}
                if(tempToken != NULL)
                {
                        free(tempToken);
                        tempToken = NULL;
                }
		currCFF.close();
	}
	MeterUtils::disableSDCard();
        SPI.end();
        #ifdef DEBUG
        Serial.println("----------------End readCurrentCF----------");
        #endif
}

void ConfigHandler::readSamplingFrequency()
{
	File sampFreqF;
	String sampFreqStr;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	
        #ifdef DEBUG
        Serial.println(":::::::::::::::");
        #endif
	if(SD.exists(SAMPLING_FREQ_F))
	{
		sampFreqF = SD.open(SAMPLING_FREQ_F);

		MeterUtils::getLine(sampFreqF, sampFreqStr);
		this->sampFreq = (uint8_t)atoi(sampFreqStr.c_str());
                #ifdef DEBUG
                Serial.println("sampFreq: "+String(this->sampFreq));
                Serial.println("sampFreqStr: "+sampFreqStr);
                #endif
		sampFreqF.close();
	}
	MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::readTimeServer()
{
	File timeServerF;
	String timeServerStr;
	uint8_t i;
	uint32_t size;
        char* buf = NULL;
        char c;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	if(SD.exists(TIME_SERVER_F))
	{
		timeServerF = SD.open(TIME_SERVER_F);
		size = timeServerF.size();

		do
		{
			c = MeterUtils::getLine(timeServerF, timeServerStr);
                        if(buf != NULL)
                        {
                                free(buf);
                                buf = NULL;
                        }
                        //if(c != -1 && c != F_LAST_CHAR)
                        //{
			buf = (char*)malloc((timeServerStr.length()+1)*sizeof(char));
                        timeServerStr.toCharArray(buf, timeServerStr.length()+1);
			timeServers.addString(buf);
                        //}
		}while(c != -1 && c != F_LAST_CHAR);
                if(buf != NULL)
                {
                        free(buf);
                        buf = NULL;
                }
		timeServerF.close();
	}
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::readMQTTServer()
{
	File addrF;
	String ipStr;
	StringList ipTokens;
	uint8_t i;
        char* tempToken = NULL;

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	//Open for read
        if(SD.exists(MQTT_SERVER_F))
        {
		addrF = SD.open(MQTT_SERVER_F);

		MeterUtils::getLine(addrF, ipStr);

                #ifdef DEBUG
                        Serial.println("ipStr: "+ipStr);
                #endif

		MeterUtils::split(ipStr, '.', ipTokens);

                #ifdef DEBUG
                        Serial.println("ipTokens.getSize(): "+String(ipTokens.getSize()));
                #endif

		for(i=0; i<ipTokens.getSize(); i++)
		{
                        #ifdef DEBUG
                                Serial.println("ipTokens[i]: "+ipTokens[i]);
                                Serial.println("mqttAddr[i]: "+String(mqttAddr[i]));
                        #endif
                        if(tempToken != NULL)
                        {
                                free(tempToken);
                                tempToken = NULL;
                        }
                        tempToken = (char*)malloc((ipTokens[i].length()+1)*sizeof(char));
                        ipTokens[i].toCharArray(tempToken, ipTokens[i].length()+1);
                        mqttAddr[i] = atoi(tempToken);
		}
                if(tempToken != NULL)
                {
                        free(tempToken);
                        tempToken = NULL;
                }

		addrF.close();
        }
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::writeAddress(MeterAddress &addr)
{
	File addrF;
	String str(addr.ip[0]);

	str += "."+addr.ip[1];
	str += "."+addr.ip[2];
	str += "."+addr.ip[3];

	str += "\n";

	str += addr.mac[0];
	str += ":"+addr.mac[1];
	str += ":"+addr.mac[2];
	str += ":"+addr.mac[3];
	str += ":"+addr.mac[4];
	str += ":"+addr.mac[5];

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	addrF = SD.open(ADDR_F, FILE_WRITE);
	addrF.write(str.c_str(), str.length());
	addrF.close();
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::writeVoltageCF(double voltCF)
{
	File voltCFF;
	String voltCFStr(voltCF);

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	voltCFF = SD.open(VOLT_CF_F, FILE_WRITE);
	voltCFF.write(voltCFStr.c_str(), voltCFStr.length());
	voltCFF.close();
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::writeCurrentCFs(CurrentCF* currCFs)
{
	File currCFF;
	String str("");
	//size_t size = currCFs.getSize();
	size_t i;
	char buf[DOUBLE_MAX_DIGITS + 1];

	for(i=0; i < CT_CHNLS; i++)
	{
		if(currCFs[i].channel != NOT_DEFINED)
		{
			str += currCFs[i].channel;
			str += ",";
			dtostrf(currCFs[i].cf, DOUBLE_MAX_DIGITS, DOUBLE_MAX_DIGITS-3, buf);
			str += buf;
			str += "\n";
		}
	}

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	currCFF = SD.open(CURRENT_CF_F, FILE_WRITE);
	currCFF.write(str.c_str(), str.length());
	currCFF.close();
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::writeSamplingFrequency(uint8_t sampFreq)
{
	File sampFreqF;

        String sampFreqStr(sampFreq);

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	sampFreqF = SD.open(SAMPLING_FREQ_F, FILE_WRITE);
	sampFreqF.write(sampFreqStr.c_str(), sampFreqStr.length());
	sampFreqF.close();
        MeterUtils::disableSDCard();
        SPI.end();
}

void ConfigHandler::writeTimeServers(StringList timeServers)
{
	File timeServersF;
	String str("");
	size_t size = timeServersF.size();
	size_t i;

	for(i=0; i<size; i++)
	{
		str += timeServers[i];
		if(i<size-1)
			str += "\n";
	}

        MeterUtils::enableSDCard();
	SD.begin(SD_SS_PIN);
	timeServersF = SD.open(TIME_SERVER_F, FILE_WRITE);
	timeServersF.write(str.c_str(), str.length());
	timeServersF.close();
        MeterUtils::disableSDCard();
        SPI.end();
}

/*----------------CurrentCFList Class-------------------------*/

CurrentCFList::CurrentCFList(void)
{
        this->size = 0;
}
CurrentCFList::~CurrentCFList(void)
{
        this->emptyList();
}
void CurrentCFList::addCurrentCF(CurrentCF item)
{
        CurrentCF* neulist = (CurrentCF*)malloc((this->size+1)*sizeof(CurrentCF));
	for(size_t i=0; i<size; i++){
		neulist[i] = this->currentCFlist[i];
	}
	
	neulist[size] = item;
	if(size > 0)
		free(this->currentCFlist);
	this->currentCFlist = neulist;
	this->size++;
}
void CurrentCFList::setCurrentCF(CurrentCF item, int index)
{
        //free(this->currentCFlist[index]);
	this->currentCFlist[index] = item;
}
void CurrentCFList::removeCurrentCF(int index)
{
        size_t i;
        CurrentCF* neulist = (CurrentCF*)malloc((this->size-1)*sizeof(CurrentCF));
	//From Begining
	for(i=0; i<index; i++){
		neulist[i] = this->currentCFlist[i];
	}
	//From next Index
        //free(currentCFlist[index]);
	for(i=index; i<=this->size-1; i++){
		neulist[i] = this->currentCFlist[i+1];
	}

	free(this->currentCFlist);
	this->currentCFlist = neulist;
	this->size--;
}
void CurrentCFList::emptyList()
{
        /*size_t i;
        for(i=0; i < this->size; i++)
        {
                free(this->currentCFlist[i]);
        }*/
        if(this->size > 0)
              free(this->currentCFlist);
}
int CurrentCFList::getSize()
{
        return this->size;
}
CurrentCF CurrentCFList::operator[](size_t idx)
{
        return this->currentCFlist[idx];
}
