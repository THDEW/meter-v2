#ifndef __OTR_DEF__
#define __OTR_DEF__

//#define DEBUG

/* Chip select/Slave select (CS/SS) pins
 */
#define ETH_SS PB4 //Slave select for Ethernet (digital pin 10)
#define SD_SS PG5 //Slave select for SD card (digital pin 4)
#define HW_SS PB0 //SPI hardware SS pin
#define VTCT_SS PF6 //Slave select for VT's and CT's ADCs (analog pin 6)

#define ETH_SS_PORT PORTB
#define ETH_SS_DDR DDRB
#define SD_SS_PORT PORTG
#define SD_SS_DDR DDRG

#define HW_SS_DDR DDRB

#define VTCT_SS_PORT PORTF
#define VTCT_SS_DDR DDRF

#define SD_SS_PIN 4

#define ADC_MAX 4096 // 12-bit ADC
#define WAVE_NUM_CROSS 24 //2 * number of cycles to sample for calculating Vrms, Irms, P (averaged)

#define CONF_DIR "CONF"
#define DATA_DIR "DATA"

#define F_LAST_CHAR '@'

#define DIV_R1 100000 //R1 value in the voltage dividor of voltage transformer
#define DIV_R2 10000 //R2 value in the voltage dividor of voltage transformer

#define SAMPLE_INT 0
#define SECONDS_TO_PUSH 60

#define CT_CHNLS 15

#define ENTRIES_SENT 5 //the number of data entries being dispatched to the server at a time

#endif
