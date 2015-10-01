#include <Arduino.h>
#include "Measurement.h"

DataAcquisition::DataAcquisition()
{

	/*Configure VT channel
	 */
	VT_STCP_DDR |= (1 << VT_STCP); //output
	VT_STCP_PORT &= ~(1 << VT_STCP); //clear bit (set the output of the channel as LOW)
	VT_DDR &= ~((1 << VT_Q0) | (1 << VT_Q1) | (1 << VT_Q2) | (1 << VT_Q3) | (1 << VT_Q4) | (1 << VT_Q5) | (1 << VT_Q6) | (1 << VT_Q7)); //set all these channels as inputs
	VT_PORT |= (1 << VT_Q0) | (1 << VT_Q1) | (1 << VT_Q2) | (1 << VT_Q3) | (1 << VT_Q4) | (1 << VT_Q5) | (1 << VT_Q6) | (1 << VT_Q7); //pull up

	/*Configure CT selection ports (CTS), and the presence detection ports (CTD)
	 */
	CTS_DDR |= (1 << CTS_A0) | (1 << CTS_A1) | (1 << CTS_A2) | (1 << CTS_A3); //outputs
	CTS_PORT &= ~((1 << CTS_A0) | (1 << CTS_A1) | (1 << CTS_A2) | (1 << CTS_A3)); //clear bits

	/* 15 CT channels presence detection (inputs) */
	CTD_DDR1 &= ~((1 << CTD_0) | (1 << CTD_1) | (1 << CTD_2) | (1 << CTD_3) | (1 << CTD_4) | (1 << CTD_5) | (1 << CTD_6) | (1 << CTD_7)); //inputs
	CTD_PORT1 |= (1 << CTD_0) | (1 << CTD_1) | (1 << CTD_2) | (1 << CTD_3) | (1 << CTD_4) | (1 << CTD_5) | (1 << CTD_6) | (1 << CTD_7); //pull up
	CTD_DDR2 &= ~((1 << CTD_8) | (1 << CTD_9) | (1 << CTD_10) | (1 << CTD_11) | (1 << CTD_12) | (1 << CTD_13) | (1 << CTD_14)); //inputs
	CTD_PORT2 |= (1 << CTD_8) | (1 << CTD_9) | (1 << CTD_10) | (1 << CTD_11) | (1 << CTD_12) | (1 << CTD_13) | (1 << CTD_14); //pull up

        this->voltSamples = 0;
        this->currentSamples = 0;
	this->sumSquareVolt = 0;
	this->sumSquareCurrent = 0;
	this->sumRealPower = 0;
	this->powerNumSamples = 0;
	this->voltZeroCross = 0;
	this->currentZeroCross = 0;
	this->lastSampledUnbiasVolt = 0;
	this->lastSampledUnbiasCurrent = 0;
	this->sampledUnbiasVolt = 0;
	this->sampledUnbiasCurrent = 0;
	this->detectCTConnection();
}
void DataAcquisition::sampleBiasVoltage() //sample the reference bias voltage and store in 'biasVolt'
{
	uint32_t vcc; //in mV
	uint16_t readValue; //a level out of 1024 levels [0, 1023]
	this->readVcc(vcc);
	readValue = analogRead(BIAS_VOLT_APIN);
	this->biasVolt.level12bit = readValue * 4; //convert from 10-bit scale to 12-bit scale: (readValue/1024)*4096
	this->biasVolt.voltage = readValue*(vcc/1024000.0);
}
void DataAcquisition::detectCTConnection()
{
	this->ctConnected = 0;
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_14)) << (14-CTD_14);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_13)) << (13-CTD_13);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_12)) << (12-CTD_12);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_11)) << (11-CTD_11);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_10)) << (10-CTD_10);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_9)) << (9-CTD_9);
	this->ctConnected |= (CTD_PIN2 & (1 << CTD_8)) << (8-CTD_8);

	this->ctConnected |= CTD_PIN1; //lower byte

	this->ctConnected = ~this->ctConnected; //The set bit indicates connected CT; otherwise, disconnected.
}
double DataAcquisition::getPowerNumSamples()
{
        return this->lastPowerNumSamples;
}
bool DataAcquisition::isConnected(uint8_t ctChannel) //ctChannel in [0,14]
{
	//Assume detectCTConnection must be called first.
	return (this->ctConnected & (1 << ctChannel)) != 0;
}
void DataAcquisition::findStartingPoint(uint16_t& valVoltCh, uint16_t& valCurrCh)
{
	this->sampleVoltageCurrent(valVoltCh, valCurrCh);
        //Serial.println("valVoltCh: "+String(valVoltCh)+", valCurrCh: "+String(valCurrCh));
	sampledUnbiasVolt = (valVoltCh * ADC_LSB) + ADD_VOLT - biasVolt.voltage;
        //Serial.println("sampledUnbiasVolt: "+String(sampledUnbiasVolt));
	do{ //Keep sampling if zero crossing point has not yet been detected.
		lastSampledUnbiasVolt = sampledUnbiasVolt;
		this->sampleVoltageCurrent(valVoltCh, valCurrCh);
                //Serial.println("valVoltCh: "+String(valVoltCh)+", valCurrCh: "+String(valCurrCh)+", ADC_LSB: "+String(ADC_LSB,6)+", biasVolt.voltage: "+String(biasVolt.voltage,4));
		sampledUnbiasVolt = (valVoltCh * ADC_LSB) + ADD_VOLT - biasVolt.voltage;
                //Serial.println("lastSampledUnbiasVolt: "+String(lastSampledUnbiasVolt,6)+", sampledUnbiasVolt: "+String(sampledUnbiasVolt,6));
	}while(!(lastSampledUnbiasVolt <= 0 && sampledUnbiasVolt >=0) && !(lastSampledUnbiasVolt >= 0 && sampledUnbiasVolt <=0));
	//Use the last 'valVoltCh' and 'valCurrCh' immediately after the zero crossing point has been detected.
	lastSampledUnbiasVolt = sampledUnbiasVolt; //To prevent the zero crossing detection from identifying this first sample as the first zero cross.
}
void DataAcquisition::configDataAcquisitionSPI()
{
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
	SPI.setBitOrder(MSBFIRST);
}
void DataAcquisition::resetSPIConfig()
{
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);
}
void DataAcquisition::selectChannel(uint8_t ctChannel) //ctChannel in [0,14]
{
	CTS_PORT = 0x0F & ctChannel;
}
void DataAcquisition::sampleVoltageCurrent(uint16_t& valVoltCh, uint16_t& valCurrCh)
{
	uint8_t lbyteVolt, lbyteCurr;

	MeterUtils::enableVTCT(); //Enable its slave select and disable other SPI slaves
	//VTCT_SS_PORT &= ~(1 << VTCT_SS); //clear bit (enable the slaves; start sampling on both ADCs)
	this->configDataAcquisitionSPI();

	this->sampleBiasVoltage();
        cSampleTime2 = cSampleTime1;
        cSampleTime1 = micros();
	valCurrCh = SPI.transfer(0); //High byte for current value
        vSampleTime2 = vSampleTime1;
        vSampleTime1 = micros();
	VT_STCP_PORT |= (1 << VT_STCP); //dump the high-byte sampled data to storage register
	valVoltCh = VT_PIN; //High byte for voltage value
	VT_STCP_PORT &= ~(1 << VT_STCP); //close the storage register
	lbyteCurr = SPI.transfer(0); //Low byte
	VT_STCP_PORT |= (1 << VT_STCP);
	lbyteVolt = VT_PIN; //Low byte
	VT_STCP_PORT &= ~(1 << VT_STCP); //close the storage register

	VTCT_SS_PORT |= (1 << VTCT_SS); //set bit (disable the slaves; stop sampling on both ADCs)

	valVoltCh = (valVoltCh << 7) | (lbyteVolt >> 1); //According to Figure 6-2 in page 15 of the  MCP7940 datasheet
	valVoltCh &= 0x0FFF; //Only 12 bits as the result

	valCurrCh = (valCurrCh << 7) | (lbyteCurr >> 1);
	valCurrCh &= 0x0FFF; //Only 12 bits as the result
}
void DataAcquisition::accumulateSumSquare(uint8_t type, uint16_t& sampledValue)
{
	if(type == VOLT_TYPE)
	{
                vSampleTimes[voltSamples] = vSampleTime2 - vSampleTime1;
                voltSamples++;
		sampledUnbiasVolt = (sampledValue * ADC_LSB) + ADD_VOLT;
		sampledUnbiasVolt -= biasVolt.voltage; //Get rid off the bias voltage from the sampled voltage
		sumSquareVolt += sampledUnbiasVolt * sampledUnbiasVolt; //Accumulate the square of voltage's voltage
		if((lastSampledUnbiasVolt <= 0 && sampledUnbiasVolt >=0) || (lastSampledUnbiasVolt >= 0 && sampledUnbiasVolt <=0))
			voltZeroCross++;
	}
	else if(type == CURRENT_TYPE)
	{
                cSampleTimes[currentSamples] = cSampleTime2 - cSampleTime1;
                currentSamples++;
		sampledUnbiasCurrent = (sampledValue * ADC_LSB) + ADD_CURR;
		sampledUnbiasCurrent -= biasVolt.voltage; //Get rid off the bias voltage from the sampled voltage
                //sampledUnbiasCurrent *= CURR_FACTOR;
                //Serial.println("sampledUnbiasCurrent: "+String(sampledUnbiasCurrent, 6));
		sumSquareCurrent += sampledUnbiasCurrent * sampledUnbiasCurrent; //Accumulate the square of current's voltage
                //Serial.println("sumSquareCurrent: "+String(sumSquareCurrent, 6));
		if((lastSampledUnbiasCurrent <= 0 && sampledUnbiasCurrent >=0) || (lastSampledUnbiasCurrent >= 0 && sampledUnbiasCurrent <=0))
			currentZeroCross++;
	}
}
void DataAcquisition::calculateRMSVoltage(double& rmsVolt, double& voltCF)
{
	rmsVolt = sqrt(this->sumSquareVolt/this->voltSamples) * voltCF; //Multiply by voltage calibration factor
        this->latestVoltSamples = this->voltSamples;
        this->voltSamples = 0;
	this->sumSquareVolt = 0; //Reset the sum square
	this->voltZeroCross = 0;
	this->lastSampledUnbiasVolt = 0;
	this->sampledUnbiasVolt = 0;
}
void DataAcquisition::calculateRMSCurrent(double& rmsCurrent, double& currentCF)
{
	rmsCurrent = sqrt(this->sumSquareCurrent/this->currentSamples) * currentCF; //Multiply by current calibration factor
        //Serial.println("this->currentSamples: "+String(this->currentSamples)+", sqrt(this->sumSquareCurrent/this->currentSamples): "+String(sqrt(this->sumSquareCurrent/this->currentSamples), 6)+", currentCF: "+String(currentCF));
        //Serial.println("rmsCurrent: "+ String(rmsCurrent, 8));
        this->latestCurrentSamples = this->currentSamples;
        this->currentSamples = 0;
	this->sumSquareCurrent = 0; //Reset the sum square
	this->currentZeroCross = 0;
	this->lastSampledUnbiasCurrent = 0;
	this->sampledUnbiasCurrent = 0;
}
void DataAcquisition::accumulateRealPower(uint16_t& sampledVoltValue, uint16_t& sampledCurrentValue, double& voltCF, double& currentCF) //Must be called after 'accumulateSumSquare' and before 'calculateRMSxxx'
{
	double sampledVoltage, sampledCurrent;
	sampledVoltage = sampledUnbiasVolt * voltCF;
	sampledCurrent = sampledUnbiasCurrent * currentCF;
	this->sumRealPower += sampledVoltage * sampledCurrent;
	lastSampledUnbiasVolt = sampledUnbiasVolt;
	lastSampledUnbiasCurrent = sampledUnbiasCurrent;
	this->powerNumSamples++;
}
void DataAcquisition::averageRealPower(double& avgP)
{
	avgP = this->sumRealPower / this->powerNumSamples;
	this->sumRealPower = 0;
        this->lastPowerNumSamples = this->powerNumSamples;
	this->powerNumSamples = 0;
}
uint8_t DataAcquisition::getCurrentZeroCrosses()
{
	return this->currentZeroCross;
}
uint8_t DataAcquisition::getVoltZeroCrosses()
{
	return this->voltZeroCross;
}
void DataAcquisition::readVcc(uint32_t& result) //result in mV
{
	//not used on emonTx V3 - as Vcc is always 3.3V - eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/
	#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  
	#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	ADCSRB &= ~_BV(MUX5);   // Without this the function always returns -1 on the ATmega2560 http://openenergymonitor.org/emon/node/2253#comment-11432
	#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
	#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);

	#endif

	#if defined(__AVR__) 
	delay(2);                                        // Wait for Vref to settle
	ADCSRA |= _BV(ADSC);                             // Convert
	while (bit_is_set(ADCSRA,ADSC));
	result = ADCL;
	result |= ADCH<<8;
	result = 1126400L / result;                     //1100mV*1024 ADC steps [REF] http://openenergymonitor.org/emon/node/1186
	
	#elif defined(__arm__)
	result = (3300);                                  //Arduino Due
	#else 
	result = (3300);                                  //Guess that other un-supported architectures will be running a 3.3V!
	#endif
}
uint16_t DataAcquisition::getLatestVoltSamples()
{
        return latestVoltSamples;
}
uint16_t DataAcquisition::getLatestCurrentSamples()
{
        return latestCurrentSamples;
}
long DataAcquisition::getVSampleTimes(uint16_t idx)
{
        return vSampleTimes[idx];
}
long DataAcquisition::getCSampleTimes(uint16_t idx)
{
        return cSampleTimes[idx];
}
