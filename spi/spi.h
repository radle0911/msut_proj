#ifndef SPI_H
#define SPI_H
//-----------------------------------------
#include "stm32f4xx.h"

// konstante:
// za accelerometer
#define SPI1_CS_HIGH    GPIOE->ODR |= GPIO_ODR_ODR_3;
#define SPI1_CS_LOW     GPIOE->ODR &= ~GPIO_ODR_ODR_3;



// baudrate prescaler
// SPI1 fclk = 84MHz/psc
#define SPI_BaudRate_Prescaler_2        ((uint16_t)0x0000)
#define SPI_BaudRate_Prescaler_4        ((uint16_t)0x0008)
#define SPI_BaudRate_Prescaler_8        ((uint16_t)0x0010)
#define SPI_BaudRate_Prescaler_16       ((uint16_t)0x0018)
#define SPI_BaudRate_Prescaler_32       ((uint16_t)0x0020)
#define SPI_BaudRate_Prescaler_64       ((uint16_t)0x0028)
#define SPI_BaudRate_Prescaler_128      ((uint16_t)0x0030)
#define SPI_BaudRate_Prescaler_256      ((uint16_t)0x0038)




// potrebne funkcije PIN je Pnesto

void initSPI1_accelerometer(uint16_t prescaler);    // PA6=>prijemnik, PA7=>predajnik
uint8_t rxByteSPI1(void);
uint8_t txByteSPI1(uint8_t data);


uint8_t testSPI1_loopback(void);
void provjera_loopback_test();


// --------------------------------------------- NRF24L01---------------------------------------------------------------


// konstante:
// za nRF
#define SPI2_CS_HIGH    GPIOB->ODR |= GPIO_ODR_ODR_12;
#define SPI2_CS_LOW     GPIOB->ODR &= ~GPIO_ODR_ODR_12;



// za SPI nRF modul
void initSPI2_nRF24lO1_carrier(uint16_t prescaler);

uint8_t rxByteSPI2_nRF_carrier(void);
uint8_t txByteSPI2_nRF_carrier(uint8_t data);

void txSPI2_nRF_carrier(uint8_t * data, uint16_t size);
void rxSPI2_nRF_carrier(uint8_t * data, uint16_t size);

//-----------------------------------------
#endif // !SPI_H

