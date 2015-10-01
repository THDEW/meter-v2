#include "MeasurementData.h"
#include "MeterUtils.h"
#include "OtherDef.h"

void MeasurementData::openForRead()
{
	SD.begin(SD_SS_PIN);
	this->dataF = SD.open(DATA_F, FILE_READ);
}

void MeasurementData::openForWrite()
{
	SD.begin(SD_SS_PIN);
	this->dataF = SD.open(DATA_F, FILE_WRITE);
}

bool MeasurementData::readEntry(DataEntry& data)
{
	size_t dataSize = sizeof(DataEntry);
	uint8_t *dataBuf = ((uint8_t*)(&data));
        size_t fileSize = this->dataF.size();
	uint8_t count, buf;
        
	//Assume there are 'dataSize' more bytes available in dataF.
	for(count=0; count < dataSize && this->dataF.position() < fileSize; count++)
	{
		/**dataBuf <<= count*8;
		*dataBuf |= this->dataF.read();*/
                buf = (uint8_t)this->dataF.read();
                if(this->dataF.position() < fileSize)
                        dataBuf[count] = buf;
                //Serial.println("size: "+String(fileSize));
                //Serial.println("position: "+String(this->dataF.position()));
                //Serial.println("dataBuf["+String(count)+"]: " + String(dataBuf[count],HEX));
	}
        if(this->dataF.position() >= fileSize)
                return false;
        //Serial.println("888888888888======");
        return true;
}

void MeasurementData::appendEntry(DataEntry& data)
{
        size_t s = sizeof(DataEntry);
        uint8_t *dat = ((uint8_t*)(&data));
        /*uint8_t i;
        for(i=0; i<s; i++)
        {
                this->dataF.write((dat+i), 1);
                Serial.println("dat["+String(i)+"]: "+String(dat[i],HEX));
        }*/
        this->dataF.write(dat, sizeof(DataEntry));
        //Serial.println("AESecond: "+String(data.timestamp.Second));
        //Serial.println("AEMinute: "+String(data.timestamp.Minute));
        //Serial.println("AEHour: "+String(data.timestamp.Hour));
}

void MeasurementData::clearData()
{
	SD.remove(DATA_F);
}

void MeasurementData::closeDataFile()
{
	this->dataF.close();
}
