#ifndef GPIOC_H
#define GPIOC_H
//=================================================
#include "stm32f4xx.h"

#define GPIOC6_CONNECTED_GND                0x00000000
#define GPIOC6_ACTIVE                GPIO_IDR_IDR_6 // ili GPIO_IDR_IDR_6 OR 0x00000040

void initGPIOC6_GND(void);
void chkConnection_GPIOC6_GND(uint8_t*);



//=================================================
#endif // !GPIOC_H
