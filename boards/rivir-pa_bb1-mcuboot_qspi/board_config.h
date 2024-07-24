#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "RIVIR"
#define PRODUCT_NAME "bloader PetroAlpha_BB-1"
#define VOLUME_LABEL "RIVIRBOOT"
#define INDEX_URL "http://rivir.space"
#define BOARD_ID "SAME51J20A-RIVIR-PA-BB1"

#define USB_VID 0x239A
#define USB_PID 0x00CD

#define LED_PIN PIN_PA19

#define BOOT_USART_MODULE                 SERCOM5
#define BOOT_USART_MASK                   APBDMASK
#define BOOT_USART_BUS_CLOCK_INDEX        MCLK_APBDMASK_SERCOM5
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_UNUSED
#define BOOT_USART_PAD2                   PINMUX_UNUSED
#define BOOT_USART_PAD1                   PINMUX_PB30D_SERCOM5_PAD1
#define BOOT_USART_PAD0                   PINMUX_PB31D_SERCOM5_PAD0
#define BOOT_GCLK_ID_CORE                 SERCOM5_GCLK_ID_CORE
#define BOOT_GCLK_ID_SLOW                 SERCOM5_GCLK_ID_SLOW

#endif
