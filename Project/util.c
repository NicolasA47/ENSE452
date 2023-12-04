
 #include "stm32f10x.h"
 #include "util.h"
 #include "CLI.h"
 #include "stm32f10x_usart.h"
 #include "FreeRTOS.h"
 #include "queue.h"
 #include "task.h"
 
volatile uint32_t ms_counter = 0;
extern QueueHandle_t xQueue;
//**************************** Utility ************************************************************
// delay = 1800 is approximately 1 ms @ SYSCLK = 24 MHz (ymmv)

void delay(uint32_t delay)
{
  	 while (delay--)
	{
		}
}

//**************************** Clock Configuration ************************************************************
void clockInit(void)
{
  
//* enable HSI and wait for it to be ready

		RCC->CR |= RCC_CR_HSION;
    while (((RCC->CR) & (RCC_CR_HSION | RCC_CR_HSIRDY)) == 0);
			
//* enable HSE with Bypass and wait for it to be ready

		RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
    while (((RCC->CR) & (RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_HSERDY)) == 0);
				
    
////SET HSI as SYSCLK and wait for it to be recognized

//		RCC->CFGR = RCC_CFGR_SW_HSI;
//    while (((RCC->CFGR) & (RCC_CFGR_SW_HSI | RCC_CFGR_SWS_HSI)) != 0);
			
//SET HSE as SYSCLK and wait for it to be recognized

		RCC->CFGR = RCC_CFGR_SW_HSE;
    while (((RCC->CFGR) & (RCC_CFGR_SW_HSE | RCC_CFGR_SWS_HSE)) == 0);

//****************To Use PLL as SYSCLK	
// Disable PLL. 
		
		RCC->CR &= ~RCC_CR_PLLON;
			
//Change PLL source and set the PLL multiplier
			
//These are the SYSCLK values when using the PLL with HSI/2 as the input. The max value is 64 MHz
		//RCC->CFGR = 0x00000000;// 8MHz
		//RCC->CFGR = 0x00040000;// 12 MHz
		//RCC->CFGR = 0x00080000;// 16 MHz
		//RCC->CFGR = 0x000c0000;// 20 MHz			
		//RCC->CFGR = 0x00100000;// 24 MHz
		//RCC->CFGR = 0x00140000;//	28 MHz
		//RCC->CFGR = 0x00180000;// 32 MHz
		RCC->CFGR = 0x001C0000;// 36 MHz			
		//RCC->CFGR = 0x00200000;// 40 MHz
		//RCC->CFGR = 0x00240000;//	44 MHz
		//RCC->CFGR = 0x00280000;// 48 MHz
		//RCC->CFGR = 0x002C0000;// 52 MHz
		//RCC->CFGR = 0x003C0000;// 64 MHz			

//These are the SYSCLK values when using the PLL with HSE/Bypass as the input. The max value is 72 MHz
		//RCC->CFGR = 0x00010000;// 16 MHz
		//RCC->CFGR = 0x00050000;// 24 MHz 
		//RCC->CFGR = 0x00090000;// 32 MHz
		//RCC->CFGR = 0x000d0000;// 40 MHz			
		//RCC->CFGR = 0x00110000;// 48 MHz
		//RCC->CFGR = 0x00150000;//	56 MHz
		//RCC->CFGR = 0x00190000;// 64 MHz
		//RCC->CFGR = 0x001d0000;// 72 MHz		


//ENABLE PLL and wait for it to be ready

    RCC->CR |= RCC_CR_PLLON;
    while (((RCC->CR) & (RCC_CR_PLLON | RCC_CR_PLLRDY)) == 0);
   
// Set PLL as SYSCLK and wait for it to be ready
			
    RCC->CFGR |= RCC_CFGR_SW_PLL;// 0x00000002;
    while (((RCC->CFGR) & (RCC_CFGR_SW_PLL | RCC_CFGR_SWS_PLL)) == 0);
			


// Enable clock visibity on PA8 and select desired source (By default, the output will be the PLL frequency divided by 2)
			
		//RCC->CFGR |= RCC_CFGR_MCO_HSI;
		//RCC->CFGR |= RCC_CFGR_MCO_HSE;
		//RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
		RCC->CFGR |= RCC_CFGR_MCO_PLL;

	
//Enable Port A and AFIO clocks
			
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

// Configure PA8 as AFIO ( Write 1011b into the configuration and mode bits )
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ;
    GPIOA->CRH &= ~GPIO_CRH_CNF8_0 ;

}

void led_IO_init (void)
{
    //Enable Port A and Port C clocks
    
    RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN ;
	
		
	//****************NUCLEO-64*******************************************************************
//    //Set the config and mode bits for Port A bit 5 so it will be a push-pull output (up to 50 MHz)
    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
	//****************NUCLEO-64*******************************************************************
	
}	
	
void led_on(void){
   GPIOA->BSRR = NUC_GREEN_ON;
}

void led_off(void){
	 GPIOA->BSRR = NUC_GREEN_OFF;
}

int led_get_state(void){
    if (GPIOA->IDR & (1 << 5)) {  
        return 1;
    } else {
        return 0;
    }
}
	
void usart_init(void) {
	NVIC_EnableIRQ(USART2_IRQn);
	USART2->CR1 |= USART_CR1_RXNEIE;

	
}


void timer2_init(uint16_t timeout_ms) {
    // Enable Timer 2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Reset Timer 2 configuration (just to be sure)
    TIM2->CR1 = 0;

    // Set the timer prescaler value. 
    // Assuming a clock of 32 MHz
    TIM2->PSC = 32000 - 1;

    // Set the auto-reload value for the desired timeout.
    // This will set the timer duration to the specified number of milliseconds.
    TIM2->ARR = timeout_ms;

    // Reset the counter value
    TIM2->CNT = 0;

    // Start Timer 2
    TIM2->CR1 |= TIM_CR1_CEN;
}

int Timer2_Expired(void) {
    // Check if the update event flag is set, indicating the timer has overflowed (expired)
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;  // Clear the update event flag
        return 1;
    }
    return 0;
}

void timer2_stop(void) {
    // Disable Timer 2
    TIM2->CR1 &= ~TIM_CR1_CEN;
}

void enableUserLED(void) {
	// Configure PA5 as a push-pull Output
	GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
	GPIOA->CRL |= GPIO_CRL_MODE5_0;
}
void toggleUserLED(void) {
	GPIOA->ODR ^= GPIO_ODR_ODR5;
}

uint8_t* queryUserLED(void) {
	if (GPIOA->IDR & GPIO_IDR_IDR5) {
			return (uint8_t *)"LED is on";
	} else {
			return (uint8_t *)"LED is off";
	}
}

void enable_TIM2(void) {
	// Enable TIM2 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 
	// Prescaler for 1 ms resolution (72 MHz / 7200), ARR (0-9 -> 1 ms)
	TIM2->PSC = 7199; 
	TIM2->ARR = 9; 
	TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
	NVIC_EnableIRQ(TIM2_IRQn); // Enable TIM2 IRQ
	TIM2->CR1 |= TIM_CR1_CEN; // Start TIM2
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		ms_counter += 1;
		// Clear update interrupt bit
		TIM2->SR &= ~TIM_SR_UIF;
  }
}


/** Configure and enable the device. */
void serial_open(void) {
		// Enable the USART2 and GPIOA clocks
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

		// Configure PA2 (alternate function, push-pull, max speed 50 MHz)
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2;

		// Configure PA3 (input with pull-up/pull-down)
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_CNF3_1;
		
		// Configure USART2 for 115200 bps, 8 data bits, no parity, 1 stop bit
		// 36 000 000 / (16 * 115200) = 19.53
		// (Mantissa << 4) | (Fraction * 16 & 0xF)
    USART2->BRR = (19 << 4) | (9 & 0xF); 
		// Activate transmit, recieve, & USART2
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
	
		// Enable USART2 receive interrupts
		USART2->CR1 |= USART_CR1_RXNEIE; 
		
		// Enable the USART2 interrupt
		NVIC_EnableIRQ(USART2_IRQn); 

}

/**
	Send an 8-bit byte to the serial port, using the configured bit-rate, # of bits, etc.
	Returns 0 on success and non-zero on failure.
	@param b the 8-bit quantity to be sent.
	@param Timeout the timeout in ms before transmission is cancelled.
	@pre must have already called serial_open()
*/
int sendByte(uint8_t b, uint32_t Timeout) {
		uint32_t initial_time = ms_counter;

    while (!(USART2->SR & USART_SR_TXE)){
			if (ms_counter - initial_time > Timeout){
				return -1; // Error code
			}
		}

    // Send the byte
    USART2->DR = b;

		initial_time = ms_counter;
    while (!(USART2->SR & USART_SR_TC)){
			if (ms_counter - initial_time > Timeout){
				return -1; // Error code
			}
		}

    return 0;
}


void button_init (void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
    GPIOC->CRH &= ~GPIO_CRH_MODE13;  
    GPIOC->CRH &= ~GPIO_CRH_CNF13;   
    GPIOC->CRH |= GPIO_CRH_CNF13_1;  

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI13_PC;
    EXTI->IMR |= EXTI_IMR_MR13;
    EXTI->FTSR |= EXTI_FTSR_TR13;
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

