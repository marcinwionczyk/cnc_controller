//
// Created by marci on 23.02.2020.
//

#ifndef CNC_CONTROLLER_V7_USART_FIFO_H
#define CNC_CONTROLLER_V7_USART_FIFO_H
#include <main.h>
#define FIFO_BUFFER_SIZE 512

void usart_char_reception_callback(void);
void usart_tx_empty_callback(void);
void usart_error_callback(void);
uint16_t available_in_tx_fifo(void);
uint16_t available_in_rx_fifo(void);

// UART data transmit function
//  - checks if there's room in the transmit sw buffer
//  - if there's room, it transfers data byte to sw buffer
//  - automatically handles "uart_tx_buffer_full_flag"
//  - sets the overflow flag upon software buffer overflow (doesn't overwrite existing data)
//  - if this is the first data byte in the buffer, it enables the "hw buffer empty" interrupt
void usart_send_byte(uint8_t byte);
// UART data receive function
//  - checks if data exists in the receive sw buffer
//  - if data exists, it returns the oldest element contained in the buffer
//  - automatically handles "uart_rx_buffer_full_flag"
//  - if no data exists, it clears the uart_rx_flag
uint8_t usart_get_byte(void);


extern volatile uint8_t usart_rx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer
extern volatile uint8_t usart_rx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
extern volatile uint8_t usart_rx_fifo_ovf_flag;       // this flag is not automatically cleared by the software buffer
extern volatile uint8_t usart_tx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
extern volatile uint8_t usart_tx_fifo_ovf_flag;       // this flag is not automatically cleared by the software buffer
extern volatile uint8_t usart_tx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer

#endif //CNC_CONTROLLER_V7_USART_FIFO_H
