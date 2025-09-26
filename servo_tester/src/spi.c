#include "spi.h"
#include "init.h"
#include "gpio.h"

static inline void InitDMA();

void InitSPI(uint8_t useDMA) {
    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_AFIOEN_Msk | RCC_APB2ENR_IOPAEN_Msk | RCC_APB2ENR_SPI1EN_Msk,
                     RCC_APB2ENR_AFIOEN    // Alternate function IO clock enable
                     | RCC_APB2ENR_IOPAEN  // IO port A clock enable
                     | RCC_APB2ENR_SPI1EN  // SPI1 clock enable
                     );

    _BST(SPI1->CR1,
         SPI_CR1_SSM                     //Software slave management enabled
       //| SPI_CR1_BR_0 | SPI_CR1_BR_1    //Baud rate control = Fpclk/16
       | SPI_CR1_BR_2                   //Baud rate control = Fpclk/32
       //| SPI_CR1_BR_0 | SPI_CR1_BR_2    //Baud rate control = Fpclk/64
       | SPI_CR1_MSTR | SPI_CR1_SSI     //Master configuration
       | SPI_CR1_CPHA                   //CPOL=0, CPHA=1
           );

    CFG_PIN(SPI_CS_PIN, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(SPI_SCK_PIN, ALTERNATE, PUSHPULL | SLOPE_50MHZ);
    CFG_PIN(SPI_MISO_PIN, ALTERNATE, PUSHPULL | SLOPE_50MHZ);
    CFG_PIN(SPI_MOSI_PIN, ALTERNATE, PUSHPULL | SLOPE_50MHZ);

    if (useDMA) {
        InitDMA();
        _BMD(SPI1->CR2, SPI_CR2_RXDMAEN_Msk | SPI_CR2_TXDMAEN_Msk, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN); // Enable SPI DMA
    }

    _BMD(SPI1->CR1, SPI_CR1_SPE_Msk, SPI_CR1_SPE); //Enable SPI
}

#define SPI_DMA_RX_NO 2
#define SPI_DMA_TX_NO 3

#define SPI_DMA_RX PERIF(DMA1_Channel, SPI_DMA_RX_NO)
#define SPI_DMA_TX PERIF(DMA1_Channel, SPI_DMA_TX_NO)

static inline void InitDMA() {
    _BMD(RCC->AHBENR, RCC_AHBENR_DMA1EN_Msk, RCC_AHBENR_DMA1EN); // Enable DMA1 clock

    //RX Channel
    SPI_DMA_RX->CCR = DMA_CCR_MINC | DMA_CCR_TCIE;      //Memory increment enabled and transmission completed interrupt enabled
    SPI_DMA_RX->CPAR = (uint32_t)&SPI1->DR;             //Set Peripheral address
    NVIC_EnableIRQ(PERIF(DMA1_Channel, PERIF(SPI_DMA_RX_NO, _IRQn))); //Enable IRQ

    //TX Channel
    SPI_DMA_TX->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE; //Memory increment enabled, read from memory direction and transmission completed interrupt enabled
    SPI_DMA_TX->CPAR = (uint32_t)&SPI1->DR;             //Set Peripheral address
    NVIC_EnableIRQ(PERIF(DMA1_Channel, PERIF(SPI_DMA_TX_NO, _IRQn))); //Enable IRQ
}

__WEAK void SPI_DMA_RXC(void){}
__WEAK void SPI_DMA_TXC(void){}

void DMA1_Channel2_IRQHandler() {
    _BCL(SPI_DMA_RX->CCR, DMA_CCR_EN); // Disable RX DMA channel
    if (!(SPI_DMA_TX->CCR & DMA_CCR_EN)) { //Ensure transmission completed
        DMA1->IFCR = PERIF(DMA_ISR_GIF, SPI_DMA_RX_NO); // Clear all interrupt flags
        SPI_DMA_RXC();
    }
}

void DMA1_Channel3_IRQHandler() {
    _BCL(SPI_DMA_TX->CCR, DMA_CCR_EN); // Disable TX DMA channel
    DMA1->IFCR = PERIF(DMA_ISR_GIF, SPI_DMA_TX_NO); // Clear all interrupt flags
    SPI_DMA_TXC();
}

uint8_t SPI_IsDMAEnabled() {
    return (SPI_DMA_RX->CCR | SPI_DMA_TX->CCR) & DMA_CCR_EN;
}

static inline void InitDMAChannel(DMA_Channel_TypeDef *channel, void* data, uint32_t size) {
    while(channel->CCR & DMA_CCR_EN)  __WFI(); // Wait while DMA Busy
    channel->CNDTR = size;
    channel->CMAR = (int32_t)data;
    _BMD(channel->CCR, DMA_CCR_EN_Msk, DMA_CCR_EN); // Enable DMA channel
}

void SPI_SendDMA(void* tx_data, uint32_t size) {
    InitDMAChannel(SPI_DMA_TX, tx_data, size);
}

void SPI_ReceiveDMA(void* rx_buf, uint32_t size) {
    InitDMAChannel(SPI_DMA_RX, rx_buf, size);
}

void SPI_SendReceiveDMA(void* tx_data, void* rx_buf, uint32_t size) {
    SPI_ReceiveDMA(rx_buf, size);
    SPI_SendDMA(tx_data, size);
}

void SPI_Send(uint8_t data) {
    while(SPI1->SR & SPI_SR_BSY || !(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
}

uint8_t SPI_Receive() {
    while(SPI1->SR & SPI_SR_BSY || !(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR;
}

uint8_t SPI_SendReceive(uint8_t data) {
    SPI_Send(data);
    return SPI_Receive();
}

void SPI_Test() {
//    uint8_t data_out[4] = {1, 2, 3, 0xFF};
//    uint8_t data_in[4] = {0, 0, 0, 0};
//    SPI_SendReciveDMA(data_out, data_in, sizeof(data_in));

//    volatile uint8_t dt = SPI_SendReceive(0xAA);
}
