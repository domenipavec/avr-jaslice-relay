/* File: main.cpp
 * Contains base main function and usually all the other stuff that avr does...
 */
/* Copyright (c) 2012-2013 Domen Ipavec (domen.ipavec@z-v.si)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/eeprom.h> 

#include <stdint.h>

#include "bitop.h"
#include "io.h"

using namespace avr_cpp_lib;

static const uint8_t ADDRESS = 0x51;

OutputPin * relays[7];

int main() {
	// init
	
	// i2c
	TWCR = 0b01000101;
	TWAR = ADDRESS<<1;

	OutputPin r0(&DDRD, &PORTD, PD6);
	OutputPin r1(&DDRD, &PORTD, PD5);
	OutputPin r2(&DDRD, &PORTD, PD4);
	OutputPin r3(&DDRD, &PORTD, PD3);
	OutputPin r4(&DDRD, &PORTD, PD2);
	OutputPin r5(&DDRD, &PORTD, PD1);
	OutputPin r6(&DDRD, &PORTD, PD0);
	
	relays[0] = &r0;
	relays[1] = &r1;
	relays[2] = &r2;
	relays[3] = &r3;
	relays[4] = &r4;
	relays[5] = &r5;
	relays[6] = &r6;
	
	// enable interrupts
	sei();

	for (;;);
}

ISR(TWI_vect) {
	static uint8_t state = 0;
	static uint8_t command = 0;
	switch (TWSR) {
		case 0x60: // SLA+W
			state = 0;
			break;
		case 0x80: // SLA+W + DATA
			switch (state) {
				case 0:
					command = TWDR;
					break;
				case 1:
					uint8_t i = TWDR;
					if (i < 7) {
						switch (command) {
							case 0:
								relays[i]->clear();
								break;
							case 1:
								relays[i]->set();
								break;
						}
					}	
					break;
			}
			state++;
			break;
	}
	SETBIT(TWCR, TWINT);
}
