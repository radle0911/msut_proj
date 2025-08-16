#ifndef DELAY_H
#define DELAY_H
// ------------------------
#include <stdint.h>

#define SYSTIMER_TIMEOUT       0
#define SYSTIMER_KEEP_ALIVE    1

void delay_ms(uint32_t);
void delay_us(uint32_t);

void initSTOPWATCH(void);
void startSTOPWATCH(void);
uint32_t stopSTOPWATCH(void);

//TIM2
void initSYSTIMER(void);
uint32_t getSYSTIMER(void);
uint8_t chk4TimeoutSYSTIMER(uint32_t btime, uint32_t period);


//TIM7
void initSYSTIMER_TIM7();
void TIM7_IRQHandler(void);
uint32_t getSYSTIMER_TIM7();
uint8_t chk4TimeoutSYSTIMER_TIM7(uint32_t btime, uint32_t period);

extern volatile uint32_t g_tim7_val; // globalna varijabla

// ------------------------
#endif // !DELAY_H
