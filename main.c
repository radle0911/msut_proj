#include "./usart/usart.h"
#include "./delay/delay.h"
#include "./nRF24L01/nRF24L01.h"
#include "./gpio/gpioc.h"
#include <stdint.h>

void runMasterNodeSYS();
void runSlaveNodeSYS();


int main(void)
{
  uint8_t node_type = NRF24L01_NODE_TYPE_RX;

  // INICIJALIZACIJA START -----------------------------------------------
  {
    initUSART2(USART2_BAUDRATE_921600);
    initSYSTIMER();

    // TX or RX check
    initGPIOC6_GND();
    delay_ms(20);
    chkConnection_GPIOC6_GND(&node_type); // 0->RX->(spojen na GND) | 1->TX->(lebdi)

    initnRF24L01(node_type);
  }
  // INICIJALIZACIJA END -------------------------------------------------

  printUSART2("\n\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");
  printUSART2("w                   nRF24L01 Tx[1]-Rx[0] => [%d]               w\n",node_type);
  printUSART2("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww\n");


  if (node_type == NRF24L01_NODE_TYPE_TX) {
    runMasterNodeSYS();
  }else {
    runSlaveNodeSYS();
  }

  return 0;
}



void runMasterNodeSYS(){
  uint8_t cnt = 0;
  char nrf_data[NRF24L01_PIPE_LENGTH] = "nRF24L01 Tx 0";
  uint8_t* p_msg = (uint8_t*)nrf_data;
  while (1) {
    p_msg[12] = 0x30 + cnt;
    txDataNRF24L01((uint8_t*)c_nrf_slave_addr, p_msg);
	//printUSART2("-> SYS: tx-ing msg [%d]\n",cnt);
    ++cnt;
    if (cnt > 9) {
      cnt = 0;
    }
  }
}


void runSlaveNodeSYS(){
  uint8_t res;
  uint8_t nrf_data[NRF24L01_PIPE_LENGTH];
  setTxAddrNRF24L01((uint8_t*)c_nrf_master_addr);
  while (1) {
    res = dataReadyNRF24L01();
    if (res == NRF_DATA_READY) {
      rxDataNRF24L01(nrf_data);
      printUSART2("%s\n",nrf_data);
    }
  }
}

