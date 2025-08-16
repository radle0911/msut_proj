#include "nRF24L01.h"
#include "../spi/spi.h"
#include "../delay/delay.h"
#include <stdint.h>
#include "../usart/usart.h"

volatile uint8_t g_nrf24l01_node_type;
const char c_nrf_master_addr[6] = "MDR01";
const char c_nrf_slave_addr[6] = "SDR00";
volatile uint8_t* g_node_address;
volatile uint8_t nrf_mode;



// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW simple tx-rx WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

void initnRF24L01(uint8_t node_type)
{
  // Inicijalizacija na PB11 - PB15
  //wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
  // PB11 - nRF24L01 CE
  // PB12 - nRF24L01 CS
  // PB13 - nRF24L01 CLK
  // PB14 - nRF24L01 MISO
  // PB15 - nRF24L01 MOSI
  //---------------------------------------------------------------------	


  //---------------------------------------------------------------------------------------------
  // Potrebno nam je da odredimo da li zelimo da inicijaliziramo nRF modul
  // kao Tx ili Rx zbog dodjeljivanja adresa.

  g_nrf24l01_node_type = node_type;
  if (g_nrf24l01_node_type == NRF24L01_NODE_TYPE_RX) {
    // inicijaliziramo adresu za prijemnik
    g_node_address = (uint8_t*)c_nrf_slave_addr;    // ovo se koristi za uporedjivanje 2 nRF modula pri komunikaciji izmedju (moraju biti isti na oba)
  }else {                                           // kako bi komunikacija izmedju njih funkcionisala 
    // predajnik
    g_node_address = (uint8_t*)c_nrf_master_addr;
  }

  // RX ≠ uvijek "slave", TX ≠ uvijek "master"
  // Kod nRF24L01 nema pravog “master/slave” kao kod I2C-a.
  // Termin "master/slave" koristi logički:
  // Master = onaj koji inicira komunikaciju (prvi šalje podatke)
  // Slave = onaj koji uglavnom samo sluša i odgovara kad primi podatke

  initSPI2_nRF24lO1_carrier(SPI_BaudRate_Prescaler_32);
  // CS, CLK, MISO, MOSI su podeseni u funkciji iznad. Potrebno je podesiti
  // jos CE (chip enable); 
  //
  // Ovaj pin kontroliše radio modul, ne SPI.
  //
  // U RX modu: CE = HIGH → čip aktivno sluša kanale i prima podatke.
  // U TX modu: CE se drži LOW dok pripremiš podatke u SPI registrima, pa se nakratko digne HIGH da započne slanje.
  // CE = LOW → čip je u standby-u ili konfiguraciji.
  // Ukratko: CE = “počni raditi” prekidač za bežični dio čipa.

  { // podesavanje pina PB11 kao CE (chip enable)
    GPIOB->MODER &= ~GPIO_MODER_MODER11;          // reset moder
    GPIOB->MODER |= GPIO_MODER_MODER11_0;         // output mode
    GPIOB->OTYPER &= ~(0x00C00000);		  // bio podesio : GPIO_OTYPER_OT_11 i nije radilo
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11_1;  // fast speed
    //GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR11;          // 
  }
  //---------------------------------------------------------------------------------------------
  NRF_CE_LOW;     // CE = LOW = “ne radi ništa, čekaj da ti sve podestim”. 
  // Ako je CE HIGH dok podešavaš registre, čip može biti usred prijema/slanja
  // i ignorisati ili pogrešno primiti SPI komande.
  // Zato se uvijek drži CE = LOW dok traje inicijalizacija, a digneš ga tek 
  // kad si sve podesio i spreman za rad.
  //
  // 💡 Ukratko:
  //            CE = LOW = “ne radi ništa, čekaj da ti sve podestim”.
  //---------------------------------------------------------------------------------------------
  //  CS (ili CSN) = chip select. Aktivno stanje mu je LOW (negativna logika).
  SPI2_CS_HIGH;   //  CSN = HIGH = “ne pričam s tobom dok ne budem spreman”.
  //---------------------------------------------------------------------------------------------
  delay_ms(100);  // Razlog:
  // 
  // Nakon napajanja ili resetovanja, nRF24L01-u treba Tpd2stby vrijeme iz datasheet-a 
  // da bi prešao u Standby-I režim (spreman da primi SPI komande).
  // To vrijeme je tipično 100 ms da bude sigurno (iako datasheet kaže da je manje).
  // Ako pokušaš slati SPI komande prerano, čip ih može ignorisati ili raditi nestabilno.
  // 💡 Ukratko:
  //            Delay = “čekam da se čip probudi prije nego što mu počnem pričati”.
  //---------------------------------------------------------------------------------------------

  // sljedece sto nam je potrebno da inicijaliziramo to sve u nRF modul

  //prvo je sto radimo da podesavamo Rx adresu od nRF modula


  setRxAddrNRF24L01((uint8_t *)g_node_address,NRF24L01_RX_ADDR_P1);		        // set Rx address 
  /*
    Postavljanjem adrese u RX_ADDR_P1, omogućavaš modulu da prima podatke koji su poslati na tu adresu.
    Pri komunikaciji, pošiljalac šalje paket na tu adresu, a prijemnik (sa ovom postavljenom adresom u RX_ADDR_P1) ga prihvata.
   */

  conRegNRF24L01(NRF24L01_RF_CH, NRF24L01_ACTIVE_CHANNEL);        			// set active channel
  /*
      Frekvencija = 2400 MHz + (vrijednost * 1 MHz)
      1 – vrijednost koju upisuješ
      Ovdje znači da postavljaš kanal na 1 MHz iznad 2.400 GHz → 2401 MHz.
      Ako bi stavio 40, dobio bi 2.440 GHz, itd.
  */


  conRegNRF24L01(NRF24L01_RX_PW_P0, NRF24L01_PIPE_LENGTH);			// set length of pipe 0
  conRegNRF24L01(NRF24L01_RX_PW_P1, NRF24L01_PIPE_LENGTH);			// set length of pipe 1

  
	

  
  /*
     Zašto se podešava RX_PW_P0 i RX_PW_P1?

        Kod prijema (RX) modul mora znati unaprijed koliko bajtova očekuje u paketu, jer u suprotnom ne zna kad je paket kompletan.
        To se postavlja u RX_PW_Pn registarskim poljima za svaki “pipe” (kanal prijema).
        P0 i P1 su dva najčešće korištena pipe-a jer:
        P0 se automatski koristi za acknowledgment reply adresu kod Auto-ACK-a.
        P1 se obično koristi kao glavna RX adresa za aplikaciju (kao što si ti napravio master/slave adresiranje).
        Ostali pipe-ovi (P2–P5) dijele većinu adrese s P1 i rijetko se koriste u osnovnim primjerima.
*/
  setRxModeNRF24L01();    // podesi nRF da radi kao prijemnik i ukljuci ga da aktivno slusa. || (uključi modul u RX režim)
  flushRxNRF24L01();      // cistimo prijemni FIFO bafer nRF-a.  || (ocisti sve što je bilo primljeno ranije )
  

//uint8_t who = getRegNRF24L01(NRF24L01_CONFIG);
//printUSART2("TEST READ CONFIG = 0x%x\n", who);

}



void setRxAddrNRF24L01(uint8_t * addr, uint8_t reg)
{
  NRF_CE_LOW;
  setRegNRF24L01(reg, addr, 5);
  NRF_CE_HIGH;
}


void conRegNRF24L01(uint8_t reg, uint8_t reg_val)
{
  setRegNRF24L01(reg, &reg_val, 1);
}




void setRxModeNRF24L01(void)
{/// Rx mode, power up the nRF TRx, CRC One byte!
	nrf_mode = NRF_MODE_RX;
		
	conRegNRF24L01(NRF24L01_CONFIG,((1<<NRF24L01_CONFIG_EN_CRC)|(0<<NRF24L01_CONFIG_CRCO))|((1<<NRF24L01_CONFIG_POWER_UP)|(1<<NRF24L01_CONFIG_PRIM_RX)));
	NRF_CE_HIGH;
	delay_us(130);	
	conRegNRF24L01(NRF24L01_STATUS, (1<<NRF24L01_STATUS_TX_DS)|(1<<NRF24L01_STATUS_MAX_RT));
}





void flushRxNRF24L01(void)
{/// flush Rx pipe
	SPI2_CS_LOW;
	txByteSPI2_nRF_carrier(NRF_FLUSH_RX);
	SPI2_CS_HIGH;
}

//---------------------------- ove se funk. pozivaju u main.c file za ovaj zadatak run master i runSlave:


void setTxAddrNRF24L01(uint8_t *daddr)
{ // mora biti isto u primarno P0 i TX radi ACK
  setRegNRF24L01(NRF24L01_RX_ADDR_P0, daddr, 5);
  setRegNRF24L01(NRF24L01_TX_ADDR, daddr, 5);
}


uint8_t getRegNRF24L01(uint8_t reg)
{
  uint8_t rx;
  reg &= NRF_REGISTER_MASK;   // cistimo bite [7:5]
  //printUSART2("x%x\n",reg);
 // printUSART2("0b%b\n",reg);

  SPI2_CS_LOW;
  txSPI2_nRF_carrier(&reg, 1);
  rxSPI2_nRF_carrier(&rx, 1);
  SPI2_CS_HIGH;
  return rx;
}




void txPacketNRF24L01(uint8_t * data)
{
	uint8_t status, k, tmp;
	uint32_t timer = getSYSTIMER();
	
	//printUSART2("Ulazi al je nrf_mode = %d treba 0 \n",nrf_mode);
	while(nrf_mode == NRF_MODE_TX)
	{
		status = getRegNRF24L01(NRF24L01_STATUS);
		if(status & ((1 << (NRF24L01_STATUS_TX_DS))|(1<<(NRF24L01_STATUS_MAX_RT))))
		{
			nrf_mode = NRF_MODE_RX; 
			break;
		}
		
		if(chk4TimeoutSYSTIMER(timer, 1000) == (SYSTIMER_TIMEOUT))
		{
			printUSART2("TIMEOUT\n");
			delay_ms(1000);
			return;
		}
	}
	
	NRF_CE_LOW;															// clear CE bit
	setTxModeNRF24L01();												// set Tx mode
	
	flushTxNRF24L01();
	
	SPI2_CS_LOW;														// clear SS bit
	tmp = NRF_W_TX_PAYLOAD;
	txSPI2_nRF_carrier(&tmp , 1);													// write cmd to write payload
	txSPI2_nRF_carrier(data, (NRF24L01_PIPE_LENGTH));
	SPI2_CS_HIGH;
	NRF_CE_HIGH;
	delay_us(130);														// delay >10us
  
	//printUSART2("kraj funkcije nrf_mode = %d treba 1 \n",nrf_mode);	 // ovo izbrisati kasnije   
	//delay_ms(1000);							 // ovo izbrisati kasnije 
	
}




//
//void txPacketNRF24L01(uint8_t* data){
//  uint32_t tajmer = getSYSTIMER();
//  uint8_t status,tmp;
//
//  while (nrf_mode == NRF_MODE_TX) {             // ovo je NRF_MODE_TX =  1
//    
//    status = getRegNRF24L01(NRF24L01_STATUS);   // Trebamo provjeriti da li je promjenjen nrf_mode
//    if (status & ((1 << NRF24L01_STATUS_TX_DS) | (1 << NRF24L01_STATUS_MAX_RT))) { // ove biti se ciste upisom 1 bit-a
//      nrf_mode = NRF_MODE_RX;
//      break;
//    }
//
//    // pravimo delay od 1ms i vrsimo ispis nakon 1ms
//    if (chk4TimeoutSYSTIMER(tajmer, 1000) == SYSTIMER_TIMEOUT) {  // 1ms
//      printUSART2("TIMEOUT\n");
//      return;
//    }
//  }
//
//    NRF_CE_LOW;	  // ------------------
//    setTxModeNRF24L01();												// set Tx mode
//    flushTxNRF24L01();
//    SPI2_CS_LOW;
//
//    tmp = NRF_W_TX_PAYLOAD; // SPI commands
//    txSPI2_nRF_carrier(&tmp, 1);    // select w_tx_payload, clear ss bit 
//    txSPI2_nRF_carrier(data, NRF24L01_PIPE_LENGTH);
//
//
//    SPI2_CS_HIGH;
//    NRF_CE_HIGH;  // ------------------
//
//    delay_us(130);
//    // ima i bolji nacin da se ova funk. zapise chatGPT
//}
//


uint8_t	txOverNRF24L01(void)
{
	uint8_t status;
	if(nrf_mode == (NRF_MODE_TX))
	{
		status = getRegNRF24L01(NRF24L01_STATUS);
		if(status & ((1 << (NRF24L01_STATUS_TX_DS))|(1<<(NRF24L01_STATUS_MAX_RT))))
		{
			setRxModeNRF24L01();
			status = (NRF_TX_FINISHED); 
		}
		else
		{
			status = (NRF_TX_IN_PROGRESS); 
		}	
	}
	else
	{
		status = (NRF_TX_FINISHED); 
	}
	
	return status;
}




//
//uint8_t txStatusNRF(){
//  uint8_t status;
//  if (nrf_mode == NRF_MODE_TX) {  // prvo provjeravamo da li je mode uopste podesen za TX
//    status = getRegNRF24L01(NRF24L01_STATUS); // adresa statusa, uzimamo sve inf. iz nje ovako
//    if (status & ((1 << (NRF24L01_STATUS_TX_DS)) | (1 << (NRF24L01_STATUS_MAX_RT)))) 
//    {
//      setRxModeNRF24L01(); // zasto se prebacuje u Rx mode ??? --> AUTO-ACK
//      /*Zašto se nakon slanja prebacuje u RX mode?
//	Kod nRF24L01, kad pošalješ paket u TX modu:
//
//	1. AKO KORISTIŠ AUTO-ACK  (ODGOVOR)
//	      PRIMALAC AUTOMATSKI ŠALJE ACK PAKET NAZAD.
//	      DA BI TVOJ MODUL UOPŠTE PRIMIO TAJ ACK, MORA DA BUDE U RX MODU ODMAH NAKON SLANJA.
//	      AKO OSTANE U TX MODU, ACK SE NEĆE PRIMITI, A STATUS_MAX_RT (MAX RETRIES) BI SE MOGAO AKTIVIRATI IAKO JE PAKET STIGAO.
//
//	2. Čekanje na odgovor
//	      U mnogim aplikacijama nakon što pošalješ podatke, očekuješ odgovor ili naredbu od druge strane.
//	      Prebacivanjem u RX modul odmah “sluša” i može primiti sledeći paket bez ručne promjene moda u drugom dijelu koda.
//
//	3.Čišćenje i spremnost
//	      Kod nRF24L01, TX FIFO i RX FIFO su odvojeni, ali statusni flagovi (TX_DS, MAX_RT) ostaju postavljeni dok ih ne očistiš.
//	      Mnogi developeri kombinuju prebacivanje u RX + flush/clear sekvencu kako bi uređaj bio u poznatom stanju i spreman za novi ciklus.*/
//      status = NRF_TX_FINISHED;
//    }else {
//    status = NRF_TX_IN_PROGRESS;
//    }
//  }else {
//    status = NRF_TX_FINISHED;
//  }
//  return status;
//}
//


	volatile uint8_t global_cnt = 0;
uint8_t txDataNRF24L01(uint8_t * daddr, uint8_t * data)
{
	uint8_t res = (NRF24L01_TX_COMPLETED);
	uint32_t timer = getSYSTIMER();
	//uint32_t tim;

	setTxAddrNRF24L01(daddr);	
	txPacketNRF24L01(data);					
	//printUSART2("-> izlazi iz txPacket funk %d\n",global_cnt);
	++global_cnt;
	while(txOverNRF24L01() == (NRF_TX_IN_PROGRESS))
	{
		if(chk4TimeoutSYSTIMER(timer, NRF24L01_TX_WAIT_PERIOD) == (SYSTIMER_TIMEOUT))
		{
			res = (NRF24L01_TX_FAILED);
			break;
		}
	}
	
	
	//tim = getSYSTIMER();
	//printUSART2("-> timer = %d\n\n\n",tim-timer);
	return res;
}





/*
uint8_t txDataNRF24L01(uint8_t *daddr, uint8_t *data)    // data_address, data
{
  uint32_t tajmer = getSYSTIMER();
  uint8_t rezultat = NRF24L01_TX_COMPLETED;
  setTxAddrNRF24L01(daddr);   // podesavaomo u TX_addr da bude isto od rx ACK
  txPacketNRF24L01(data);     //ovo je ogromna funkcija

  while (txStatusNRF() == NRF_TX_IN_PROGRESS) {	// prof. je txOverNRF24L01() oznacio ovu funk.
    if (chk4TimeoutSYSTIMER(tajmer, NRF24L01_TX_WAIT_PERIOD) == SYSTIMER_TIMEOUT) {   // ako se ne zavrsi za 1ms fail je 
      rezultat = NRF24L01_TX_FAILED;
      break;
    }
  }
  return rezultat;
}
*/


uint8_t dataReadyNRF24L01(void)
{
  uint8_t status = getRegNRF24L01(NRF24L01_STATUS);
 // printUSART2("prvo, status je : 0x%x\n",status);
 // delay_ms(1000);
  if (status & (1 << (NRF24L01_STATUS_RX_DR))) {
  //  printUSART2("Ulazi u if blok prvi\n");
  //  delay_ms(1000);
    status = NRF_DATA_READY;
  }else {
    // uzimamo RX FIFO status
    status = getRegNRF24L01(NRF24L01_FIFO_STATUS);
    //status = status & (1 << NRF24L01_FIFO_STATUS_RX_EMPTY);
    status &=  (1 << NRF24L01_FIFO_STATUS_RX_EMPTY);
    if (status) {
      status = NRF_DATA_NOT_READY;
    }else {
      status = NRF_DATA_READY;
    }
  }
  return status;
}




void rxDataNRF24L01(uint8_t * data)
{

  uint8_t tmp;

  SPI2_CS_LOW;

  tmp = (NRF_R_RX_PAYLOAD);
  txSPI2_nRF_carrier(&tmp, 1);	  // transmit command to read RX payload
  rxSPI2_nRF_carrier(data, (NRF24L01_PIPE_LENGTH));
  SPI2_CS_HIGH;
  
  conRegNRF24L01(NRF24L01_STATUS,(1<<(NRF24L01_STATUS_RX_DR)));
  //flushRxNRF24L01();

}


// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW continuous carrier WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//
// nemam nacina da provjerim da li je uspjesno...
// ovo su prve funkcije koje su se podesile:
void txContCarrierNRF24L01(uint32_t period, uint16_t ch)    // sta, koje, kako, za sta i zbog cega ima sve u initnRF24L01 funk. objasnjeno
{
  uint8_t data[4];

  initSPI2_nRF24lO1_carrier(SPI_BaudRate_Prescaler_32);

  { // podesavanje pina PB11 kao CE (chip enable)
    GPIOB->MODER &= ~GPIO_MODER_MODER11;          // reset moder
    GPIOB->MODER |= GPIO_MODER_MODER11_0;         // output mode
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_11;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11_1;  // fast speed
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR11;          
  }
  NRF_CE_LOW;
  SPI2_CS_HIGH;

  data[0] = 0x0A;                               // NRF24L01_CONFIG_POWER_UP | NRF24L01_CONFIG_EN_CRC
  setRegNRF24L01(NRF24L01_CONFIG, data, 1);
  delay_us(1500);

  NRF_CE_LOW;

  data[0] = 0x97;
  setRegNRF24L01(NRF24L01_RF_SETUP, data, 1);
  delay_us(10);

  // NRF_CE_LOW;
  //data[0] = 0x07;
  //setRegNRF24L01(NRF24L01_RF_SETUP, data, 1);
  //delay_us(10);

  NRF_CE_LOW;
  data[0] = ch;
  setRegNRF24L01(NRF24L01_RF_SETUP, data, ch);
  delay_us(10);

  NRF_CE_HIGH;
  delay_ms(period);
  NRF_CE_LOW;
}

// ova se funkcija ispod se koristi za sve 
void setRegNRF24L01(uint8_t registar, uint8_t *reg_value, uint8_t n_value)  // funk. za upisivanje podataka u nRF24L01 modul pomocu SPI2 
{ 
  // reg_id → broj registra u nRF24L01 koji želiš da promijeniš (npr. NRF24L01_RF_CH za frekvencijski kanal).
  // reg_value → pointer na podatke koje želiš da upišeš u taj registar.
  // n_value → koliko bajtova želiš da upišeš (veličina podataka). Neki registri su 1 bajt, neki su 5 bajtova (npr. RX/TX adrese).

  registar &= NRF_REGISTER_MASK; 
  registar |= NRF_W_REGISTER;

  SPI2_CS_LOW;
  txSPI2_nRF_carrier(&registar, 1);           // Saljemo prvi bajt komande (W_REGISTER + adresa).
  txSPI2_nRF_carrier(reg_value, n_value);     // Saljemo n_value bajtova podataka koje zelimo da upisati u taj registar.
  SPI2_CS_HIGH;                               // n_value => predstavlja velicinu adrese tipa RX_ADDR_P0 ima max 5Bayta 40bita [39:0]
                                              // ovi podaci se nalaze u nRF->manual
}



void flushTxNRF24L01(void)
{// flush Tx pipe
	SPI2_CS_LOW;
	txByteSPI2_nRF_carrier(NRF_FLUSH_TX);
	SPI2_CS_HIGH;
}




void setTxModeNRF24L01(void)
{/// Rx mode, power up the nRF TRx, CRC One byte!
	nrf_mode = (NRF_MODE_TX);
	conRegNRF24L01(NRF24L01_CONFIG,((1<<(NRF24L01_CONFIG_EN_CRC))|(0<<(NRF24L01_CONFIG_CRCO)))|((1<<(NRF24L01_CONFIG_POWER_UP))|(0<<(NRF24L01_CONFIG_PRIM_RX))));
}

