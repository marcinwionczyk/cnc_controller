#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "Config.h"
#include "usart_fifo.h"
#include "Settings.h"


#define MAX_BUFFER_SIZE     128


char buf[512] = {0};
uint16_t buf_idx = 0;

int Printf(const char *str, ...)
{
	char buffer[MAX_BUFFER_SIZE];
	uint8_t idx = 0;
	va_list vl;
	va_start(vl, str);
	int i = vsnprintf(buffer, MAX_BUFFER_SIZE, str, vl);
	if(i > MAX_BUFFER_SIZE)
    {
        i = MAX_BUFFER_SIZE;
    }
    while(i) {
        if (usart_tx_fifo_ovf_flag){
            LL_USART_EnableIT_TXE(USART1);
            usart_tx_fifo_ovf_flag = 0;
        }
        while(usart_tx_fifo_full_flag);
        usart_send_byte(buffer[idx++]);
        i--;
    }
    va_end(vl);
    LL_USART_EnableIT_TXE(USART1);
    while(usart_tx_fifo_not_empty_flag){};
    // Return number of sent bytes
	return idx;
}


int8_t Getc(char *c)
{
	if(usart_rx_fifo_not_empty_flag == 0) {
		return -1;
	}
    *c = usart_get_byte();
	return 0;
}


int Putc(const char c)
{
    if(usart_tx_fifo_ovf_flag){
        LL_USART_EnableIT_TXE(USART1);
        usart_tx_fifo_ovf_flag = 0;
    }
    while (usart_tx_fifo_full_flag);
    usart_send_byte(c);
    LL_USART_EnableIT_TXE(USART1);
    while(usart_tx_fifo_not_empty_flag);
	return 0;
}


// Convert float to string by immediately converting to a long integer, which contains
// more digits than a float. Number of decimal places, which are tracked by a counter,
// may be set by the user. The integer is then efficiently converted to a string.
// NOTE: AVR '%' and '/' integer operations are very efficient. Bitshifting speed-up
// techniques are actually just slightly slower. Found this out the hard way.
void PrintFloat(float n, uint8_t decimal_places)
{
	if(n < 0) {
		Putc('-');
		n = -n;
	}

	uint8_t decimals = decimal_places;

	while(decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}

	if(decimals) {
		n *= 10;
	}
	n += 0.5f; // Add rounding factor. Ensures carryover through entire value.

	// Generate digits backwards and store in string.
	unsigned char buffer[13];
	uint8_t i = 0;
	uint32_t a = (long)n;

	while(a > 0) {
        buffer[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}

	while(i < decimal_places) {
        buffer[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}

	if(i == decimal_places) { // Fill in leading zero, if needed.
		buffer[i++] = '0';
	}

	// Print the generated string.
	for(; i > 0; i--) {
		if(i == decimal_places) {
			Putc('.');
		} // Insert decimal point in right place.
		Putc(buffer[i - 1]);
	}
}


// Floating value printing handlers for special variables types used in Grbl and are defined
// in the config.h.
//  - CoordValue: Handles all position or coordinate values in inches or mm reporting.
//  - RateValue: Handles feed rate and current velocity in inches or mm reporting.
void PrintFloat_CoordValue(float n)
{
	if(BIT_IS_TRUE(settings.flags, BITFLAG_REPORT_INCHES)) {
		PrintFloat(n*INCH_PER_MM,N_DECIMAL_COORDVALUE_INCH);
	}
	else {
		PrintFloat(n, N_DECIMAL_COORDVALUE_MM);
	}
}


void PrintFloat_RateValue(float n)
{
	if(BIT_IS_TRUE(settings.flags, BITFLAG_REPORT_INCHES)) {
		PrintFloat(n*INCH_PER_MM,N_DECIMAL_RATEVALUE_INCH);
	}
	else {
		PrintFloat(n, N_DECIMAL_RATEVALUE_MM);
	}
}
