#ifndef __MEAS_H__
#define __MEAS_H__

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include "MeterUtils.h"

#define BIAS_VOLT_APIN 4
#define VOLT_TYPE 0
#define CURRENT_TYPE 1
#define ADC_LSB 0.001220703 // 5/4096 (Ref.Volt: 5V, Levels: 4096; 12 bits)
#define ADD_VOLT 0 //Added voltage to compensate loss from multiplexer
#define ADD_CURR 0.03 //Added current's voltage to compensate loss from multiplexer
//#define CURR_FACTOR 1 //Preliminary current factor not to make current too small before calibration


/* Data from VT's ADC (8-bit)
 */
#define VT_STCP PF7 //Storage register (analog pin 7)
#define VT_Q0 PK0 //analog pin 8
#define VT_Q1 PK1 //analog pin 9
#define VT_Q2 PK2 //analog pin 10
#define VT_Q3 PK3 //analog pin 11
#define VT_Q4 PK4 //analog pin 12
#define VT_Q5 PK5 //analog pin 13
#define VT_Q6 PK6 //analog pin 14
#define VT_Q7 PK7 //analog pin 15

#define VT_STCP_PORT PORTF
#define VT_STCP_DDR DDRF
#define VT_PORT PORTK
#define VT_DDR DDRK
#define VT_PIN PINK

/* CT Selector (Channel 0 - 14)
 */
#define CTS_A0 PF0 //analog pin 0
#define CTS_A1 PF1 //analog pin 1
#define CTS_A2 PF2 //analog pin 2
#define CTS_A3 PF3 //analog pin 3

#define CTS_PORT PORTF
#define CTS_DDR DDRF

/* CT Connection Detector (Check if a CT channel is connected or not)
   They are active low.
 */
#define CTD_0 PA0 //digital pin 22
#define CTD_1 PA1 //digital pin 23
#define CTD_2 PA2 //digital pin 24
#define CTD_3 PA3 //digital pin 25
#define CTD_4 PA4 //digital pin 26
#define CTD_5 PA5 //digital pin 27
#define CTD_6 PA6 //digital pin 28
#define CTD_7 PA7 //digital pin 29
#define CTD_8 PC6 //digital pin 31
#define CTD_9 PC5 //digital pin 32
#define CTD_10 PC4 //digital pin 33
#define CTD_11 PC3 //digital pin 34
#define CTD_12 PC2 //digital pin 35
#define CTD_13 PC1 //digital pin 36
#define CTD_14 PC0 //digital pin 37

#define CTD_PORT1 PORTA
#define CTD_DDR1 DDRA
#define CTD_PIN1 PINA
#define CTD_PORT2 PORTC
#define CTD_DDR2 DDRC
#define CTD_PIN2 PINC


struct BiasVoltage
{
	//Reference bias voltage
	uint16_t level12bit; //Corresponding level in 12-bit scale
	double voltage; //in Volt [V]
};

class DataAcquisition
{
	private:
		BiasVoltage biasVolt;
		uint16_t ctConnected; //Each bit determines if a CT channel is connected (1) or not (0); <14,0> are used (15 bits for 15 channels).
		double sumSquareVolt;
		double sumSquareCurrent;
		double sumRealPower;
		double powerNumSamples;
		double lastSampledUnbiasVolt;
		double lastSampledUnbiasCurrent;
		double sampledUnbiasVolt;
		double sampledUnbiasCurrent;
                uint16_t voltSamples;
                uint16_t currentSamples;
		uint8_t voltZeroCross;
		uint8_t currentZeroCross;
                double lastPowerNumSamples;
                
                long vSampleTime1, vSampleTime2, cSampleTime1, cSampleTime2;
                long vSampleTimes[256], cSampleTimes[256];
                uint16_t latestVoltSamples, latestCurrentSamples;

		void readVcc(uint32_t& result); //Read Arduino's actual current Vcc for calculating LSB of the internal ADC; 'result' in mV
		void sampleBiasVoltage(); //sample the reference bias voltage and store in 'biasVolt'
		void detectCTConnection();
		void configDataAcquisitionSPI();
	public:
		DataAcquisition();
		bool isConnected(uint8_t ctChannel); //ctChannel in [0,14]
		void findStartingPoint(uint16_t& valVoltCh, uint16_t& valCurrCh); //To get the starting point near the zero crossing point
		void selectChannel(uint8_t ctChannel); //ctChannel in [0,14]
		void sampleVoltageCurrent(uint16_t& valVoltCh, uint16_t& valCurrCh);
		void accumulateSumSquare(uint8_t type, uint16_t& sampledValue);
		void calculateRMSVoltage(double& rmsVolt, double& voltCF);
		void calculateRMSCurrent(double& rmsCurrent, double& currentCF);
		void accumulateRealPower(uint16_t& sampledVoltValue, uint16_t& sampledCurrentValue, double& voltCF, double& currentCF);
		void averageRealPower(double& avgP);
		void resetSPIConfig();
                double getPowerNumSamples();
		uint8_t getCurrentZeroCrosses();
		uint8_t  getVoltZeroCrosses();

                uint16_t getLatestVoltSamples();
                uint16_t getLatestCurrentSamples();
                long getVSampleTimes(uint16_t idx);
                long getCSampleTimes(uint16_t idx);
};

#endif
