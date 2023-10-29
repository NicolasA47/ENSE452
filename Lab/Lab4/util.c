
 #include "stm32f10x.h"
 #include "util.h"
 #include "stm32f10x_usart.h"
 

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
void serial_open(void) {
    // Enable the Port A and USART 2 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
		led_IO_init();

    // Configure PA2 for alternate function output Push-pull mode, max speed 50 MHz.
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= (GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1);

    // Configure PA3 for Input with pull-up / pull-down.
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_CNF3_0;

    // Enable the USART Tx and Rx in the USART Control register.
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // Configure USART 2 for 115200 bps, 8-bits-no parity, 1 stop bit. 
    USART2->BRR = (36000000 + 57500) / 115200;
    USART2->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); // 8 data bits, no parity
    USART2->CR2 &= ~USART_CR2_STOP;  // 1 stop bit

    // Enable USART2
    USART2->CR1 |= USART_CR1_UE;
}
	
void serial_close(void) {
    // Reset USART2 Configuration
    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->BRR = 0;

    // Disable the USART2 clock
    RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

int sendbyte(uint8_t b, uint16_t Timeout) {
    timer2_init(Timeout);  // Initialize and start Timer 2 with the desired timeout
    
    while (!(USART2->SR & USART_SR_TXE)) {
        if (Timer2_Expired()) {  // Check if Timer 2 has reached its limit
            timer2_stop();       // Stop Timer 2
            return -1;           // Timeout error
        }
    }
    timer2_stop();

    // Check for error flags
    if (USART2->SR & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE)){
        USART2->SR &= ~(USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE); // Clear the flags
        return -2; // other error
    }

    // Send byte
    USART2->DR = b;
    
    return 0; // Successfully sent the byte
}


uint8_t getbyte(int dr) {
    while (!dr);  // Wait until data is received
    dr = 0;   // Reset the flag
    return (uint8_t)USART2->DR;
}


int USART2_IRQHandler(int dr) {
	int dataRecieved = dr;
    if (USART2->SR & USART_SR_RXNE) {  // RXNE: Read data register not empty
       dataRecieved = 1;
    }
	return dataRecieved;
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


