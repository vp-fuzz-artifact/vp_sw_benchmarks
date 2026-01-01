/*
 * Copyright (C) 2025 ICE RWTH-Aachen
 *
 * This file is part of AFL++ VP-Mode.
 *
 * AFL++ VP-Mode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * AFL++ VP-Mode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AFL++ VP-Mode. If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * Copyright (C) 2025 ICE RWTH-Aachen
 * GPLv2-or-later
 */

#include <stdint.h>

#define UART0_BASE   0x10009000      // PL011 UART base address

#define UART_DR      (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART_FR      (*(volatile uint32_t *)(UART0_BASE + 0x18))

// Flag register bits
#define UART_FR_RXFE (1 << 4)        // Receive FIFO empty
#define UART_FR_TXFF (1 << 5)        // Transmit FIFO full
// Define LCR_H (Line Control Register) at Offset 0x2C
#define UART_LCR_H   (*(volatile uint32_t *)(UART0_BASE + 0x2C))

// PL011 abstractions
static inline char uart_getc() {
    // Wait until the RX FIFO has data
    while (UART_FR & UART_FR_RXFE);
    return (char)(UART_DR & 0xFF);
}

static inline void uart_putc(char c) {
    // Wait until TX FIFO has space
    while (UART_FR & UART_FR_TXFF);
    UART_DR = (uint32_t)c;
}

#define MAX 10

extern "C" void exit(int exit_code) {
    *((volatile int*)0x10008000) = 1; // stop sim
    while (1) {}
}

int my_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

extern "C" void main() {
    UART_LCR_H = (1 << 4);
    int i = 0;
    char expected[] = "eassword";
    char input[MAX];
    char c;
    do {
        c = uart_getc();
        input[i++] = c;
    } while (c != '\n' && c != '\0' && i < MAX);

    input[i - 1] = 0;

    if (my_strcmp(expected, input) == 0) {
        exit(1);
    } else {
        exit(0);
    }
}


