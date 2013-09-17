/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 *  \file
 * 
 *  \section Purpose
 * 
 *  Interface for configuring and using Timer Counter (TC) peripherals.
 * 
 *  \section Usage
 *  -# Optionally, use TC_FindMckDivisor() to let the program find the best
 *     TCCLKS field value automatically.
 *  -# Configure a Timer Counter in the desired mode using TC_Configure().
 *  -# Start or stop the timer clock using TC_Start() and TC_Stop().
 */

#ifndef TC_H
#define TC_H

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/

/* These headers were introduced in C99 by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <board.h>

/*------------------------------------------------------------------------------
 *         Global functions
 *------------------------------------------------------------------------------*/

extern void TC_Configure(Tc *pTc, uint8_t ch, uint32_t mode);

extern void TC_Start(Tc *pTc, uint8_t ch);

extern void TC_Stop(Tc *pTc, uint8_t ch);

extern uint8_t TC_FindMckDivisor(
    uint32_t freq,
    uint32_t mck,
    uint32_t *div,
    uint32_t *tcclks);

#endif /* #ifndef TC_H */

