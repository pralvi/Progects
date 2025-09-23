#include "usart.h"

/*
//USART1
APB2
TX - A9
RX - A10

//USART2
APB1
TX - A2
RX - A3

//USART3
APB1
TX - B10
RX - B11
*/

#define USARTBAUD(baud) (uint32_t)((uint32_t)((F_CPU) / (baud) / 16) * 16 + (((F_CPU) / (baud)) % 16))

static USART_HANDLER handlers[3] = {0};

uint8_t init_usart(USART_TypeDef* usart, uint32_t baud, USART_HANDLER handler) {
    if (usart && baud && handler) {
        uint_fast8_t irqn = USART1_IRQn;

        if (usart == USART1) {
            handlers[0] = handler;
            RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
            CFG_PIN(A, 9, ALTERNATE, PUSHPULL | SLOPE_2MHZ); //TX
            CFG_PIN(A, 10, INPUT, PULLUP); //RX
        } else {
            baud <<= 1; // APB1 is 2 times slower than APB2

            uint8_t isUART3 = usart == USART3;
            handlers[isUART3 + 1] = handler;

            RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | ((isUART3) ? RCC_APB2ENR_IOPBEN : RCC_APB2ENR_IOPAEN);
            RCC->APB1ENR |= (isUART3) ? RCC_APB1ENR_USART3EN : RCC_APB1ENR_USART2EN;

            if (isUART3) {
                CFG_PIN(B, 10, ALTERNATE, PUSHPULL | SLOPE_2MHZ);
                irqn = USART3_IRQn;
            } else {
                CFG_PIN(A, 2, ALTERNATE, PUSHPULL | SLOPE_2MHZ);
                irqn = USART2_IRQn;
            }
        }

        usart->BRR = USARTBAUD(baud);
        usart->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE; // USART ON, TX ON, RX ON, RXNE Interrupt enabled

        NVIC_EnableIRQ(irqn);
        NVIC_SetPriority(irqn, 4);

        return 1;
    }
    return 0;
}

void usart_transmit(USART_TypeDef* usart) {
    if (!(usart->CR1 & USART_CR1_TXEIE))
        usart->CR1 |= USART_CR1_TXEIE; // Enable TXE Interrupt to start transmission
}


static void USART_CommonHandler(USART_TypeDef* usart, USART_HANDLER handler) {
    uint8_t data;
    uint8_t event = 0;

    if (usart->SR & USART_SR_RXNE) { //Receive character
        data = usart->DR;
        event = USART_RX_EVENT;
    }

    //It's anticipated that missing handlers error situation, so it will be maximized
    if (usart->CR1 & USART_CR1_TXEIE && usart->SR & USART_SR_TXE) { // Transmit character
        if (handler(usart, event | USART_TX_EVENT, &data))
            usart->DR = data;
        else
            usart->CR1 &= ~USART_CR1_TXEIE; //Disable TXE interrupt

    } else if (event)
        handler(usart, event, &data);
}

void USART1_IRQHandler(void) {
    USART_CommonHandler(USART1, handlers[0]);
}

void USART2_IRQHandler(void) {
    USART_CommonHandler(USART2, handlers[1]);
}

void USART3_IRQHandler(void) {
    USART_CommonHandler(USART3, handlers[2]);
}


