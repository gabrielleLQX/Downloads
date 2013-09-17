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
 *  Implement simple system tick usage.
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include <board.h>
#include <cmsis/core_cm3.h>
#include "timetick.h"

/*----------------------------------------------------------------------------
 *         Local variables
 *----------------------------------------------------------------------------*/

/** Tick Counter united by ms */
static volatile uint32_t tickCount = 0;

/*----------------------------------------------------------------------------
 *         Exported Functions
 *----------------------------------------------------------------------------*/

/**
 *  \brief Handler for Sytem Tick interrupt.
 */
void SysTick_Handler(void)
{
    tickCount ++;
}

/**
 *  \brief Configures the SAM3 SysTick & reset tickCount.
 *  Systick interrupt handler will generates 1ms interrupt and increase a
 *  tickCount.
 *  \param new_mck  Current master clock.
 */
void TimeTick_Configure(uint32_t new_mck)
{
    SysTick_Config(new_mck/1000);
    tickCount = 0;
}

/**
 *  \brief Get current Tick Count, in ms.
 */
uint32_t GetTickCount(void)
{
    return tickCount;
}

/**
 *  \brief Sync Wait for several ms
 */
void Wait(volatile uint32_t ms)
{
    uint32_t start;
    start = tickCount;
    while(tickCount - start < ms);
}

/**
 *  \brief Sync Sleep for several ms
 */
void Sleep(volatile uint32_t ms)
{
    uint32_t start;
    start = tickCount;
    while(tickCount - start < ms) {
        __WFI();
    }
}

