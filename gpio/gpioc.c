#include "gpioc.h"
#include "stm32f4xx.h"
#include "../nRF24L01/nRF24L01.h"
#include <stdint.h>



void initGPIOC6_GND(void){
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  GPIOC->MODER &= ~GPIO_MODER_MODER6;           // input mode
//  GPIOC->OTYPER &= ~GPIO_OTYPER_OT_6;
//  GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;   // fast
  GPIOC->PUPDR |= GPIO_PUPDR_PUPDR6_0;          // pull-up uvijek je LOG 1 kad se spoji na GND onda je LOG 0 (pull up je 1 kada nije spojen(lebdi))
}



void chkConnection_GPIOC6_GND(uint8_t* node_type){
  if ((GPIOC->IDR & GPIO_IDR_IDR_6) == GPIOC6_CONNECTED_GND) {  // kada je 0 onda je conektovan, a kada je 1 onda nije jer je pull_up (kad lebdi onda nije connected)
    *node_type = NRF24L01_NODE_TYPE_TX;
  }
}
