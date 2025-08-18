#include "spi.h"
#include "stm32f4xx.h"
#include "../usart/usart.h"



void initSPI1_accelerometer(uint16_t prescaler)
{
  // Prov inicijaliziramo pinove sa kojima je povezan SPI protokol
  // na STM32 ploci
  // PA5->SPI1_SCK; PA6->SPI1_MISO; PA7->SPI1_MOSI
  // PA6=>prijemnik, PA7=>predajnik
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_1 |GPIO_MODER_MODER6_1 |GPIO_MODER_MODER7_1; // Alternate function mode
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_5|GPIO_OTYPER_OT_6|GPIO_OTYPER_OT_7);
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_1 |GPIO_OSPEEDER_OSPEEDR6_1 |GPIO_OSPEEDER_OSPEEDR7_1; // fast speed
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5|GPIO_PUPDR_PUPDR6|GPIO_PUPDR_PUPDR7);

    GPIOA->AFR[0] |= 0x55550000;
  }

  // Potreban nam je i jedan neovisam gpio pin npr PE3 za high i low CS (chip select)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    GPIOE->MODER |= (GPIO_MODER_MODER3_0);  // general output mode
    GPIOE->OTYPER &= ~(GPIO_OTYPER_OT_3);
    GPIOE->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3_1);   // prof je inicijalizirao moder2 // fast speed
    GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPDR3);
    // Takodjer u .h file je definisano za high i low 
    //
    // profa je to ovako odradio : 
    //
    //	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    //	GPIOE->MODER |= 0x00000040; 
    //	GPIOE->OTYPER &= ~(0x00000008);
    //	GPIOE->OSPEEDR |= 0x00000080;	
    //  i onda je pozvao odr ovako :
    SPI1_CS_HIGH;
  }

  // inicijalizacija SPI1
  {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 |= SPI_CR1_MSTR;    // master configuration
    SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
    SPI1->CR1 |= prescaler;       // podesavamo BR[2:0] vrijednosti

    SPI1->CR1 |=SPI_CR1_SPE;      // SPI enable
  }
}



uint8_t rxByteSPI1(void)
{
  uint8_t data;

  SPI1->DR = 0x00;    // ovo nisam bio upisao i nije radilo
  /*
✅ Zašto se u SPI šalje "dummy bajt" pri čitanju?
    SPI je full-duplex protokol – svaki put kad master pošalje bajt, on istovremeno prima bajt nazad od slave uređaja.
    Dakle:
    Da bi master mogao da primi bajt → mora poslati nešto!
    Taj bajt koji master pošalje ne mora imati značenje – zovemo ga "dummy byte".
   */

  while (!(SPI1->SR & SPI_SR_TXE));   // ✅ čekaj dok TX buffer nije PRAZAN (spreman za slanje novog bajta)
  while (!(SPI1->SR & SPI_SR_RXNE));  // ✅ čekaj dok RX buffer NIJE PUN (bajt još nije primljen)
  while (SPI1->SR & SPI_SR_BSY);      // ✅ čekaj dok SPI još radi (prenosi), tj. dok nije završen ceo prenos
  data = SPI1->DR;                    // ✅ pročitaj primljeni bajt iz DR registra

  return data;
}


uint8_t txByteSPI1(uint8_t data)
{
  uint8_t tmp;

  SPI1->DR = data;    // saljemo podatke

  while (!(SPI1->SR & SPI_SR_TXE));   // ✅ čekaj dok TX buffer nije PRAZAN (spreman za slanje novog bajta)
  while (!(SPI1->SR & SPI_SR_RXNE));  // ✅ čekaj dok RX buffer NIJE PUN (bajt još nije primljen)
  while (SPI1->SR & SPI_SR_BSY);      // ✅ čekaj dok SPI još radi (prenosi), tj. dok nije završen ceo prenos
  tmp = SPI1->DR;

  return tmp;
}



uint8_t testSPI1_loopback(void) 
{
    uint8_t sendByte = 0xA5;      // proizvoljan test bajt
    uint8_t receivedByte;

    receivedByte = txByteSPI1(sendByte);
    
    if(receivedByte == sendByte) {
        // test prosao
        return 1;
    } else {
        // test fail
        return 0;
    }
}


// da bi ovo radilo mora se fizicki spojiti PA6 i PA7 (logicno)
void provjera_loopback_test() // ovo je za provjeru samo bilo, mozes izbrisati
{
  if(testSPI1_loopback()) {
    printUSART2("primljeno\n");
  } else {
    printUSART2("nije primljeno\n");
  }
}







// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW NRF24L01 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW




void initSPI2_nRF24lO1_carrier(uint16_t prescaler)
{
  // Prov inicijaliziramo pinove sa kojima je povezan SPI protokol
  // na STM32 ploci
  // -------------------------------------------------------------
  //
  // PB11 - nRF24L01 CE     no
  //
  // PB12 - nRF24L01 CS     yes
  //
  // PB13 - nRF24L01 CLK    yes
  // PB14 - nRF24L01 MISO   yes
  // PB15 - nRF24L01 MOSI   yes
  // -------------------------------------------------------------
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~((GPIO_MODER_MODER13)|(GPIO_MODER_MODER14)|(GPIO_MODER_MODER15));
    GPIOB->MODER |= GPIO_MODER_MODER13_1 |GPIO_MODER_MODER14_1 |GPIO_MODER_MODER15_1; // Alternate function mode
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_13|GPIO_OTYPER_OT_14|GPIO_OTYPER_OT_15);
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_1 |GPIO_OSPEEDER_OSPEEDR14_1 |GPIO_OSPEEDER_OSPEEDR15_1; // fast speed
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13|GPIO_PUPDR_PUPDR14|GPIO_PUPDR_PUPDR15);

    GPIOB->AFR[1] |= 0x55500000;
  }

  // Potreban nam je i jedan neovisam gpio pin npr PB12 za high i low CS (chip select)
  {
    GPIOB->MODER &= ~(GPIO_MODER_MODER12);  // moder reset
    GPIOB->MODER |= (GPIO_MODER_MODER12_0);  // general output mode
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_12|GPIO_OTYPER_OT_13);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_1);   // prof je inicijalizirao moder2 // fast speed
    /*prof je to ovako uradio : 
     * meni ovo nema smisla 
     	GPIOB->OTYPER &= ~(0x03000000);
	GPIOB->OSPEEDR |= 0x02000000;										// 
     */
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR12);
    // zatim odma podesavamo CS u HIGH tj ne koristi se 
    SPI2_CS_HIGH;
  }

  // inicijalizacija SPI2
  {
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    SPI2->CR1 = SPI_CR1_MSTR;    // master configuration      // ovako je prof. uradio
    //SPI2->CR1 |= SPI_CR1_MSTR;    // master configuration   // chatGPT kaze ovo je bolje
    SPI2->CR1 |= (SPI_CR1_SSI | SPI_CR1_SSM);
    SPI2->CR1 |= prescaler;       // podesavamo BR[2:0] vrijednosti

    SPI2->CR1 |=SPI_CR1_SPE;      // SPI enable
  }
}



uint8_t rxByteSPI2_nRF_carrier(void)
{
  uint8_t data;

  SPI2->DR = 0x00;    // ovo nisam bio upisao i nije radilo
  /*
✅ Zašto se u SPI šalje "dummy bajt" pri čitanju?
    SPI je full-duplex protokol – svaki put kad master pošalje bajt, on istovremeno prima bajt nazad od slave uređaja.
    Dakle:
    Da bi master mogao da primi bajt → mora poslati nešto!
    Taj bajt koji master pošalje ne mora imati značenje – zovemo ga "dummy byte".
   */

  while (!(SPI2->SR & SPI_SR_TXE));   // ✅ čekaj dok TX buffer nije PRAZAN (spreman za slanje novog bajta)
  while (!(SPI2->SR & SPI_SR_RXNE));  // ✅ čekaj dok RX buffer NIJE PUN (bajt još nije primljen)
  while (SPI2->SR & SPI_SR_BSY);      // ✅ čekaj dok SPI još radi (prenosi), tj. dok nije završen ceo prenos
  data = SPI2->DR;                    // ✅ pročitaj primljeni bajt iz DR registra

  return data;
}


uint8_t txByteSPI2_nRF_carrier(uint8_t data)
{
  uint8_t tmp;

  SPI2->DR = data;    // saljemo podatke

  while (!(SPI2->SR & SPI_SR_TXE));   // ✅ čekaj dok TX buffer nije PRAZAN (spreman za slanje novog bajta)
  while (!(SPI2->SR & SPI_SR_RXNE));  // ✅ čekaj dok RX buffer NIJE PUN (bajt još nije primljen)
  while (SPI2->SR & SPI_SR_BSY);      // ✅ čekaj dok SPI još radi (prenosi), tj. dok nije završen ceo prenos
  tmp = SPI2->DR;   // myb samo spi2->dr

  return tmp;
}


void txSPI2_nRF_carrier(uint8_t * data, uint16_t size)  // saljemo bit po bit
{
  uint16_t k;
  for(k=0;k<size;k++)
  {
    txByteSPI2_nRF_carrier(data[k]);
  }
}


void rxSPI2_nRF_carrier(uint8_t * data, uint16_t size)  // primamo bit po bit
{
  uint16_t k;
  for (k=0; k<size; k++) {
    data[k] = rxByteSPI2_nRF_carrier();
  }
}
