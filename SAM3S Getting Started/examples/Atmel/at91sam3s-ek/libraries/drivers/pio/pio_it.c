/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
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

/// \file

/// Disable traces for this file
#undef TRACE_LEVEL
#define TRACE_LEVEL 0

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "pio_it.h"
#include "pio.h"
#include <board.h>
#include <pmc/pmc.h>
#include <utility/assert.h>
#include <utility/trace.h>
#include <cmsis/core_cm3.h>

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

/// Maximum number of interrupt sources that can be defined. This
/// constant can be increased, but the current value is the smallest possible
/// that will be compatible with all existing projects.
#define MAX_INTERRUPT_SOURCES       7

//------------------------------------------------------------------------------
//         Local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Describes a PIO interrupt source, including the PIO instance triggering the
/// interrupt and the associated interrupt handler.
//------------------------------------------------------------------------------
typedef struct {

    /// Pointer to the source pin instance.
    const Pin *pPin;

    /// Interrupt handler.
    void (*handler)(const Pin *);

} InterruptSource;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// List of interrupt sources.
static InterruptSource pSources[MAX_INTERRUPT_SOURCES];

/// Number of currently defined interrupt sources.
static unsigned int numSources = 0;

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \brief Handles all interrupts on the given PIO controller.
/// \param id  PIO controller ID.
/// \param pPio  PIO controller base address.
//------------------------------------------------------------------------------
void PioInterruptHandler(unsigned int id, Pio *pPio)
{
    unsigned int status;
    unsigned int i;

    // Read PIO controller status
    status = pPio->PIO_ISR;
    status &= pPio->PIO_IMR;

    // Check pending events
    if (status != 0) {

        TRACE_DEBUG("PIO interrupt on PIO controller #%d\n\r", id);

        // Find triggering source
        i = 0;
        while (status != 0) {

            // There cannot be an unconfigured source enabled.
            SANITY_CHECK(i < numSources);

            // Source is configured on the same controller
            if (pSources[i].pPin->id == id) {

                // Source has PIOs whose statuses have changed
                if ((status & pSources[i].pPin->mask) != 0) {

                    TRACE_DEBUG("Interrupt source #%d triggered\n\r", i);

                    pSources[i].handler(pSources[i].pPin);
                    status &= ~(pSources[i].pPin->mask);
                }
            }
            i++;
        }
    }
}

//------------------------------------------------------------------------------
/// \brief Generic PIO interrupt handler.
/// Single entry point for interrupts coming from any PIO controller
/// (PIO A, B, C ...). Dispatches the interrupt to the user-configured handlers.
//------------------------------------------------------------------------------
void PIO_IT_InterruptHandler(void)
{
    // Treat PIOA interrupts
    PioInterruptHandler(ID_PIOA, PIOA);
    // Treat PIOB interrupts
    PioInterruptHandler(ID_PIOB, PIOB);
    // Treat PIOC interrupts
    PioInterruptHandler(ID_PIOC, PIOC);
}

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller A interrupt handler
/// Redefined PIOA interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOA_IrqHandler(void)
{
    PioInterruptHandler(ID_PIOA, PIOA);
}

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller B interrupt handler
/// Redefined PIOB interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOB_IrqHandler(void)
{
    PioInterruptHandler(ID_PIOB, PIOB);
}

//------------------------------------------------------------------------------
/// \brief Parallel IO Controller C interrupt handler
/// Redefined PIOC interrupt handler for NVIC interrupt table.
//------------------------------------------------------------------------------
void PIOC_IrqHandler(void)
{
    PioInterruptHandler(ID_PIOC, PIOC);
}

//------------------------------------------------------------------------------
/// \brief Initializes the PIO interrupt management logic
/// The desired priority of PIO interrupts must be provided.
/// Calling this function multiple times result in the reset of currently
/// configured interrupts.
/// \param priority  PIO controller interrupts priority.
//------------------------------------------------------------------------------
void PIO_InitializeInterrupts(unsigned int priority)
{
    TRACE_DEBUG("PIO_Initialize()\n\r");

    // Reset sources
    numSources = 0;

    // Configure PIO interrupt sources
    TRACE_DEBUG("PIO_Initialize: Configuring PIOA\n\r");
    PMC_EnablePeripheral(ID_PIOA);
    PIOA->PIO_ISR;
    PIOA->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOA_IRQn);
    NVIC_ClearPendingIRQ(PIOA_IRQn);
    NVIC_SetPriority(PIOA_IRQn, priority);
    NVIC_EnableIRQ(PIOA_IRQn);

    TRACE_DEBUG("PIO_Initialize: Configuring PIOB\n\r");
    PMC_EnablePeripheral(ID_PIOB);
    PIOB->PIO_ISR;
    PIOB->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOB_IRQn);
    NVIC_ClearPendingIRQ(PIOB_IRQn);
    NVIC_SetPriority(PIOB_IRQn, priority);
    NVIC_EnableIRQ(PIOB_IRQn);

    TRACE_DEBUG("PIO_Initialize: Configuring PIOC\n\r");
    PMC_EnablePeripheral(ID_PIOC);
    PIOC->PIO_ISR;
    PIOC->PIO_IDR = 0xFFFFFFFF;
    NVIC_DisableIRQ(PIOC_IRQn);
    NVIC_ClearPendingIRQ(PIOC_IRQn);
    NVIC_SetPriority(PIOC_IRQn, priority);
    NVIC_EnableIRQ(PIOC_IRQn);
}

//------------------------------------------------------------------------------
/// Configures a PIO or a group of PIO to generate an interrupt on status
/// change. The provided interrupt handler will be called with the triggering
/// pin as its parameter (enabling different pin instances to share the same
/// handler).
/// \param pPin  Pointer to a Pin instance.
/// \param handler  Interrupt handler function pointer.
//------------------------------------------------------------------------------
void PIO_ConfigureIt(const Pin *pPin, void (*handler)(const Pin *))
{
    Pio* pio = pPin->pio;
    InterruptSource *pSource;

    TRACE_DEBUG("PIO_ConfigureIt()\n\r");

    SANITY_CHECK(pPin);
    ASSERT(numSources < MAX_INTERRUPT_SOURCES,
           "-F- PIO_ConfigureIt: Increase MAX_INTERRUPT_SOURCES\n\r");

    // Define new source
    TRACE_DEBUG("PIO_ConfigureIt: Defining new source #%d.\n\r",  numSources);

    pSource = &(pSources[numSources]);
    pSource->pPin = pPin;
    pSource->handler = handler;
    numSources++;

    // PIO3 with additional interrupt support
    // Configure additional interrupt mode registers
    if (pPin->attribute & PIO_IT_AIME) {
        // enable additional interrupt mode
        pio->PIO_AIMER       = pPin->mask;
        
        // if bit field of selected pin is 1, set as Rising Edge/High level detection event
        if (pPin->attribute & PIO_IT_RE_OR_HL)
            pio->PIO_REHLSR    = pPin->mask;
        else
            pio->PIO_FELLSR     = pPin->mask;

        // if bit field of selected pin is 1, set as edge detection source
        if (pPin->attribute & PIO_IT_EDGE)
            pio->PIO_ESR     = pPin->mask;
        else
            pio->PIO_LSR     = pPin->mask;
    } else {
        // disable additional interrupt mode
        pio->PIO_AIMDR       = pPin->mask;
    }
}

//------------------------------------------------------------------------------
/// Enables the given interrupt source if it has been configured. The status
/// register of the corresponding PIO controller is cleared prior to enabling
/// the interrupt.
/// \param pPin  Interrupt source to enable.
//------------------------------------------------------------------------------
void PIO_EnableIt(const Pin *pPin)
{
    TRACE_DEBUG("PIO_EnableIt()\n\r");

    SANITY_CHECK(pPin);

#ifndef NOASSERT
    unsigned int i = 0;
    unsigned char found = 0;
    while ((i < numSources) && !found) {

        if (pSources[i].pPin == pPin) {

            found = 1;
        }
        i++;
    }
    ASSERT(found, "-F- PIO_EnableIt: Interrupt source has not been configured\n\r");
#endif

    pPin->pio->PIO_ISR;
    pPin->pio->PIO_IER = pPin->mask;


}

//------------------------------------------------------------------------------
/// Disables a given interrupt source, with no added side effects.
/// \param pPin  Interrupt source to disable.
//------------------------------------------------------------------------------
void PIO_DisableIt(const Pin *pPin)
{
    SANITY_CHECK(pPin);

    TRACE_DEBUG("PIO_DisableIt()\n\r");

    pPin->pio->PIO_IDR = pPin->mask;
}

