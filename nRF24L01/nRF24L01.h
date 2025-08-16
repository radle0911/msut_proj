#ifndef NRF24L01_H
#define NRF24L01_H
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
#include <stdint.h>


//------------------------SPI COMANDS START-------------------------
// OBJASNJENJE:
//
// W_REGISTER je 001A AAAA to znaci :
//
//  0010 0000 je vrijednost W_REGISTER pa je 
//  0010 0000 = 0x20;

#define NRF_R_REGISTER              0x00
#define NRF_REGISTER_MASK	        0x1F
#define NRF_W_REGISTER              0x20
#define NRF_R_RX_PAYLOAD            0x61
#define NRF_W_TX_PAYLOAD            0xA0
#define NRF_FLUSH_TX                0xE1
#define NRF_FLUSH_RX                0xE2
#define NRF_REUSE_TX_PL             0xE3
#define NRF_ACTIVATE                0x50
#define NRF_R_RX_PL_WID             0x60
#define NRF_NOP                     0xFF

//------------------------SPI COMANDS END-------------------------
//
/// nRF24L01P Memory map
///
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_CONFIG									0x00
//----------------------------------------------------------------------
#define NRF24L01_CONFIG_MASK_RX_DR 						6
#define NRF24L01_CONFIG_MASK_TX_DS 						5
#define NRF24L01_CONFIG_MASK_MAX_RT						4
#define NRF24L01_CONFIG_EN_CRC							3
#define NRF24L01_CONFIG_CRCO							2				// 1 - 2byte CRC, 0 - 1byte CRC
#define NRF24L01_CONFIG_POWER_UP						1				// 1 - on, 0 - off, NRF
#define NRF24L01_CONFIG_PRIM_RX							0				// 1 - Rx Mode, 0 - Tx Mode
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_EN_AA									0x01
//----------------------------------------------------------------------
#define NRF24L01_EN_AA_ENAA_P0							0 
#define NRF24L01_EN_AA_ENAA_P1							1 
#define NRF24L01_EN_AA_ENAA_P2							2 
#define NRF24L01_EN_AA_ENAA_P3							3 
#define NRF24L01_EN_AA_ENAA_P4							4 
#define NRF24L01_EN_AA_ENAA_P5							5 
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_EN_RXADDR								0x02 
//----------------------------------------------------------------------
#define NRF24L01_EN_RXADDR_ERX_P0						0 
#define NRF24L01_EN_RXADDR_ERX_P1						1 
#define NRF24L01_EN_RXADDR_ERX_P2						2 
#define NRF24L01_EN_RXADDR_ERX_P3						3 
#define NRF24L01_EN_RXADDR_ERX_P4						4 
#define NRF24L01_EN_RXADDR_ERX_P5						5 
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_SETUP_AW    							0x03
//----------------------------------------------------------------------
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_SETUP_RETR  							0x04
//----------------------------------------------------------------------
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_RF_CH									0x05
//----------------------------------------------------------------------
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_RF_SETUP								0x06
//----------------------------------------------------------------------
#define NRF24L01_RF_SETUP_RF_CONT_WAVE					7
#define NRF24L01_RF_SETUP_RF_DR_LOW						5
#define NRF24L01_RF_SETUP_RF_PLL_LOCK					4
#define NRF24L01_RF_SETUP_RF_DR_HIGH					3			
#define NRF24L01_RF_SETUP_RF_PWR1						2
#define NRF24L01_RF_SETUP_RF_PWR0						1	
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_STATUS									0x07
//----------------------------------------------------------------------	
#define NRF24L01_STATUS_RX_DR							6	
#define NRF24L01_STATUS_TX_DS							5	
#define NRF24L01_STATUS_MAX_RT							4
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_OBSERVE_TX								0x08
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_RPD									0x09
//----------------------------------------------------------------------
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_RX_ADDR_P0  							0x0A
#define NRF24L01_RX_ADDR_P1    							0x0B
#define NRF24L01_RX_ADDR_P2    							0x0C
#define NRF24L01_RX_ADDR_P3    							0x0D
#define NRF24L01_RX_ADDR_P4    							0x0E
#define NRF24L01_RX_ADDR_P5    							0x0F
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_TX_ADDR       							0x10
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_RX_PW_P0  								0x11
#define NRF24L01_RX_PW_P1  							    0x12
#define NRF24L01_RX_PW_P2  								0x13
#define NRF24L01_RX_PW_P3  								0x14
#define NRF24L01_RX_PW_P4  								0x15
#define NRF24L01_RX_PW_P5     							0x16
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_FIFO_STATUS 							0x17
//----------------------------------------------------------------------
#define NRF24L01_FIFO_STATUS_RX_EMPTY					0
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#define NRF24L01_DYNPD									0x1C

//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
// user constants
//wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

#define NRF_MODE_TX							1		
#define NRF_MODE_RX							0	
#define NRF_DATA_READY						1
#define NRF_DATA_NOT_READY					0	
#define NRF_ADDR_EQUAL						1
#define NRF_ADDR_NOT_EQUAL					0
#define NRF24L01_IRQ_ON						1
#define NRF24L01_IRQ_OFF					0
#define NRF_TX_IN_PROGRESS					1
#define NRF_TX_FINISHED						0

#define NRF24L01_MAX_CHANNEL				127
#define NRF24L01_ACTIVE_CHANNEL				1
#define NRF24L01_PIPE_LENGTH				16 							// (1-32)

#define NRF24L01_TX_WAIT_PERIOD				100							// [x1ms]
		
#define NRF24L01_NODE_TYPE_RX				0x00
#define NRF24L01_NODE_TYPE_TX				0x01
#define NRF24L01_TX_COMPLETED				0x00
#define NRF24L01_TX_FAILED					0x01

// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
// Eldar

#define NRF24L01_NODE_TYPE_RX				0x00    // slave
#define NRF24L01_NODE_TYPE_TX				0x01    // master



































extern volatile uint8_t* g_node_address;
extern volatile uint8_t nrf_mode;

extern const char c_nrf_master_addr[6];
extern const char c_nrf_slave_addr[6];







#define NRF_CE_HIGH     GPIOB->ODR |= 0x0800; 
#define NRF_CE_LOW      GPIOB->ODR &= ~0x0800; 

// Funkcije koristene za zadatak CONTINUOUS CARRIER (prve funk)
// ---------------------------------------------------------------------
void setRegNRF24L01(uint8_t registar, uint8_t * reg_value, uint8_t n_value);
void txContCarrierNRF24L01(uint32_t period, uint16_t ch);   // GPIOB
// ---------------------------------------------------------------------



// Funkcije koristene za zadatak SIMPLE TX-RX MSG (prve funk)
// ---------------------------------------------------------------------
void initnRF24L01(uint8_t node_type); // GPIOB
void setRxAddrNRF24L01(uint8_t * addr, uint8_t reg);
void conRegNRF24L01(uint8_t reg, uint8_t reg_val);
void setRxModeNRF24L01(void);
void flushRxNRF24L01();
uint8_t getRegNRF24L01(uint8_t reg);

// ovo jos ubaciti potrebno zbog main runMaster i runSlave funkcija
uint8_t txDataNRF24L01(uint8_t* daddr, uint8_t* data);
void setTxAddrNRF24L01(uint8_t*addr);
void txPacketNRF24L01(uint8_t*);//ovo je ogromna funkcija
uint8_t txStatusNRF();
uint8_t dataReadyNRF24L01(void);
void rxDataNRF24L01(uint8_t * data);

void setTxModeNRF24L01(void);
void flushTxNRF24L01(void);
uint8_t	txOverNRF24L01(void);

// ---------------------------------------------------------------------
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
#endif // !NRF24L01_H
