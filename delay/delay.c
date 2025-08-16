#include "delay.h"
#include "stm32f4xx.h"
#include <stdint.h>

volatile uint32_t g_tim7_val = 0; // globalna varijabla

// TIM12 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void delay_ms(uint32_t ms){
    RCC->APB1ENR |= RCC_APB1ENR_TIM12EN; // 2x42MHz = 84MHz
    TIM12->PSC = 0x0054 - 0x0001;        // (84+1)-1
    TIM12->ARR = 0x03E8;                 // 1000        
    // f_event = f_clk/[(PSC+1)-1] / ARR = 1000
    // T_event = 1/f_event = 0.001s = 1ms

    TIM12->CR1 |= TIM_CR1_ARPE| TIM_CR1_URS;
   // TIM12->CR1 &= ~TIM_CR1_CMS; // 0 -> 1 -> 2 -> 3.....-> ARR -> 0 -> 1 ->2 .... 
    TIM12->CR1 &= ~TIM_CR1_DIR; // upcounting

    TIM12->EGR |= TIM_EGR_UG;
    TIM12->CR1 |= TIM_CR1_CEN; // start counting;

    while(ms){
        while (!(TIM12->SR & TIM_SR_UIF));
        TIM12->SR &= ~TIM_SR_UIF;
        --ms;
    }

    TIM12->CR1 &= ~TIM_CR1_CEN;
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM12EN;
}
void delay_us(uint32_t us){
    RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
    TIM12->PSC = 0x0004 - 0x0001;
    TIM12->ARR = 0x0015;

    TIM12->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
   // TIM12->CR1 &= ~TIM_CR1_CMS;
    TIM12->CR1 &= ~TIM_CR1_DIR;
    
    TIM12->EGR |= TIM_EGR_UG;
    TIM12->CR1 |= TIM_CR1_CEN;

    while (us) {
        while (!(TIM12->SR & TIM_SR_UIF));
        TIM12->SR &= ~TIM_SR_UIF;
        --us;
    }
    TIM12->CR1 &= ~TIM_CR1_CEN;
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM12EN;


}
// TIM12 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




// TIM5 ####################################################################################
void initSTOPWATCH(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    TIM5->PSC = 0x0054 - 0x001; // ovo je 84 i ovdje je 1us jer psc odredjuje tikove 
    TIM5->ARR = 0xFFFFFFFF;     // ARR dokle ce da broji te tikove (impulse) 32bitni je 
                                // bas dugo broji 70+ min. Al CNT broji tikove koji su 1us.
    TIM5->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS ;
    TIM5->CR1 &= ~(TIM_CR1_CMS | TIM_CR1_DIR);
    TIM5->CR2 = 0x0000;
    TIM5->CNT = 0x00000000;
}
void startSTOPWATCH(void){
    TIM5->EGR |= TIM_EGR_UG;
    TIM5->CR1 |= TIM_CR1_CEN;
}
uint32_t stopSTOPWATCH(void){
    uint32_t time = 0;
    time = TIM5->CNT;
    TIM5->CR1 &= ~TIM_CR1_CEN;
   // RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;
    return time;
}
// TIM5 ####################################################################################




// TIM2 --------------------------------------------------------------------------------------
void initSYSTIMER(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // 84MHz
    TIM2->PSC = 0x0054 - 0x0001; // 1us takt
    TIM2->ARR = 0xFFFFFFFF; // TIM2 je 32bitni broji do max

    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
    TIM2->CR1 &= ~(TIM_CR1_CMS|TIM_CR1_DIR);


    TIM2->CR2 = 0x0000; // why is this ??? ne mora da bude ali eto da bude kao default vrijednost
    TIM2->CNT = 0x00000000;

    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;

}
uint32_t getSYSTIMER(void){
    return TIM2->CNT;
}
uint8_t chk4TimeoutSYSTIMER(uint32_t btime, uint32_t period){
    uint32_t time = getSYSTIMER();
    if (time >= btime) {
        if (time >= (btime+period)) { // da li je sadasnje vrijeme preslo ciljano vrijeme
            return SYSTIMER_TIMEOUT;
        }else {
            return SYSTIMER_KEEP_ALIVE;
        }
    }else {
        uint32_t tmp32 = 0xFFFFFFFF - btime;
        if ((time + tmp32)>=period) { // koliko je ukupno proslo od btime
            return SYSTIMER_TIMEOUT;
        }else {
            return SYSTIMER_KEEP_ALIVE;
        }
    }

}

// TIM2 --------------------------------------------------------------------------------------


// TIM7 --------------------------------------------------------------------------------------
void initSYSTIMER_TIM7(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 0x0054 - 0x0001;
    TIM7->ARR = 0x03E8;         // ovdje je podeseno da bude u ms, a ne kao prethodno da se broji (ovo je 1000)

    TIM7->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    TIM7->DIER |= TIM_DIER_UIE;

    TIM7->CR2 = 0x0000;
    TIM7->CNT = 0x0000;

    TIM7->EGR |= TIM_EGR_UG;

    NVIC_SetPriority(TIM7_IRQn, 0);
    NVIC_EnableIRQ(TIM7_IRQn);

    TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM7_IRQHandler(void){
    if ((TIM7->SR & TIM_SR_UIF) == TIM_SR_UIF) { // Al msm da moze da radi i bez ovog ==
        ++g_tim7_val;                           // znaci svaki put kada se desi interupt povecaj globalnu varijablu za 1 (svake ms)
        TIM7->SR &= ~TIM_SR_UIF;
    }
}


uint32_t getSYSTIMER_TIM7(){
    return g_tim7_val;
}


uint8_t chk4TimeoutSYSTIMER_TIM7(uint32_t btime, uint32_t period){
    uint32_t time = g_tim7_val; // ili getSYSTIMER_TIM7();
    if (time >= btime ) {
        if (time >= btime + period ) { // znaci da je preslo sto se ne smije desiti
            return SYSTIMER_TIMEOUT;
        } else { 
            return SYSTIMER_KEEP_ALIVE;
        }
    }else {
        uint32_t tmp32 = 0xFFFFFFFF - btime; 
        if ((time + tmp32) >= period) {
            return SYSTIMER_TIMEOUT;
        } else {
            return SYSTIMER_KEEP_ALIVE;
        }
    }
}

// TIM7 --------------------------------------------------------------------------------------



