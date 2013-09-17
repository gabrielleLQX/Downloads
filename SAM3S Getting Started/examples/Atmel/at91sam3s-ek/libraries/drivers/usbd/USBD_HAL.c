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

/**

 \file

 \section Purpose

    Implementation of USB device functions on a UDP controller.

    See \ref usbd_api_method USBD API Methods.
*/

/*---------------------------------------------------------------------------
 *      Headers
 *---------------------------------------------------------------------------*/

/* These headers were introduced in C99 by
   working group ISO/IEC JTC1/SC22/WG14. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <board.h>
#include <pio/pio.h>
#include <utility/trace.h>
#include <pmc/pmc.h>
#include <utility/led.h>

#include "USBD_LEDs.h"
#include "usb/device/core/USBD_HAL.h"

/*---------------------------------------------------------------------------
 *      Definitions
 *---------------------------------------------------------------------------*/

/**
 *  \section UDP_registers_sec "UDP register field values"
 * 
 *  This section lists the initialize values of UDP registers.
 * 
 *  \subsection Values
 *  - UDP_RXDATA
 */
/** Bit mask for both banks of the UDP_CSR register. */
#define UDP_CSR_RXDATA_BK      (UDP_CSR_RX_DATA_BK0 | UDP_CSR_RX_DATA_BK1)
//------------------------------------------------------------------------------

/**
 * \section endpoint_states_sec "Endpoint states"
 * 
 *  This page lists the endpoint states.
 * 
 *  \subsection States
 *  - UDP_ENDPOINT_DISABLED
 *  - UDP_ENDPOINT_HALTED
 *  - UDP_ENDPOINT_IDLE
 *  - UDP_ENDPOINT_SENDING
 *  - UDP_ENDPOINT_RECEIVING
 *  - UDP_ENDPOINT_SENDINGM
 *  - UDP_ENDPOINT_RECEIVINGM
 */

/// Endpoint states: Endpoint is disabled
#define UDP_ENDPOINT_DISABLED       0
/// Endpoint states: Endpoint is halted (i.e. STALLs every request)
#define UDP_ENDPOINT_HALTED         1
/// Endpoint states: Endpoint is idle (i.e. ready for transmission)
#define UDP_ENDPOINT_IDLE           2
/// Endpoint states: Endpoint is sending data
#define UDP_ENDPOINT_SENDING        3
/// Endpoint states: Endpoint is receiving data
#define UDP_ENDPOINT_RECEIVING      4
/// Endpoint states: Endpoint is sending MBL
#define UDP_ENDPOINT_SENDINGM       5
/// Endpoint states: Endpoint is receiving MBL
#define UDP_ENDPOINT_RECEIVINGM     6
//------------------------------------------------------------------------------

/**
 *  \section udp_csr_register_access_sec "UDP_CSR register access"
 * 
 *  This page lists the macros to access UDP CSR register.
 * 
 *  !Macros
 *  - CLEAR_CSR
 *  - SET_CSR
 */

/// Bitmap for all status bits in CSR.
#define REG_NO_EFFECT_1_ALL      UDP_CSR_RX_DATA_BK0 | UDP_CSR_RX_DATA_BK1 \
                                |UDP_CSR_STALLSENTISOERROR | UDP_CSR_RXSETUP \
                                |UDP_CSR_TXCOMP

/// Clears the specified bit(s) in the UDP_CSR register.
/// \param endpoint The endpoint number of the CSR to process.
/// \param flags The bitmap to set to 1.
#define SET_CSR(endpoint, flags) \
    { \
        volatile uint32_t reg; \
        int32_t timeOut = 200; \
        reg = UDP->UDP_CSR[endpoint] ; \
        reg |= REG_NO_EFFECT_1_ALL; \
        reg |= (flags); \
        UDP->UDP_CSR[endpoint] = reg; \
        while ( (UDP->UDP_CSR[endpoint] & (flags)) != (flags)) \
        { \
            if (-- timeOut <= 0) break; \
        } \
    }

/// Sets the specified bit(s) in the UDP_CSR register.
/// \param endpoint The endpoint number of the CSR to process.
/// \param flags The bitmap to clear to 0.
#define CLEAR_CSR(endpoint, flags) \
    { \
        volatile uint32_t reg; \
        reg = UDP->UDP_CSR[endpoint]; \
        reg |= REG_NO_EFFECT_1_ALL; \
        reg &= ~((uint32_t)(flags)); \
        UDP->UDP_CSR[endpoint] = reg; \
        while ( (UDP->UDP_CSR[endpoint] & (flags)) == (flags)); \
    }
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------
 *      Types
 *---------------------------------------------------------------------------*/

/// Describes an ongoing transfer on a UDP endpoint.
typedef struct {

    /// Pointer to a data buffer used for emission/reception.
    uint8_t             *pData;
    /// Number of bytes which have been written into the UDP internal FIFO
    /// buffers.
    volatile int32_t     buffered;
    /// Number of bytes which have been sent/received.
    volatile int32_t     transferred;
    /// Number of bytes which have not been buffered/transferred yet.
    volatile int32_t     remaining;
    /// Optional callback to invoke when the transfer completes.
    volatile TransferCallback fCallback;
    /// Optional argument to the callback function.
    void             *pArgument;
} Transfer;

/// Describes Multi Buffer List transfer on a UDP endpoint.
typedef struct {
    /// Pointer to frame list
    USBDTransferBuffer *pMbl;
    /// Pointer to last loaded buffer
    USBDTransferBuffer *pLastLoaded;
    /// List size
    uint16_t      listSize;
    /// Current processing frame
    uint16_t      currBuffer;
    /// First freed frame to re-use
    uint16_t      freedBuffer;
    /// Frame setting, circle frame list
    uint8_t       circList;
    /// All buffer listed is used
    uint8_t       allUsed;
    /// Optional callback to invoke when the transfer completes.
    MblTransferCallback fCallback;
    /// Optional argument to the callback function.
    void                *pArgument;
} MblTransfer;

//------------------------------------------------------------------------------
/// Describes the state of an endpoint of the UDP controller.
//------------------------------------------------------------------------------
typedef struct {

    /// Current endpoint state.
    volatile uint8_t  state;
    /// Current reception bank (0 or 1).
    volatile uint8_t  bank;
    /// Maximum packet size for the endpoint.
    volatile uint16_t size;
    /// Describes an ongoing transfer (if current state is either
    ///  UDP_ENDPOINT_SENDING or UDP_ENDPOINT_RECEIVING)
    union {
        Transfer    singleTransfer;
        MblTransfer mblTransfer;
    } transfer;
} Endpoint;

/*---------------------------------------------------------------------------
 *      Internal variables
 *---------------------------------------------------------------------------*/

/** Holds the internal state for each endpoint of the UDP. */
static Endpoint endpoints[CHIP_USB_NUMENDPOINTS];

/*---------------------------------------------------------------------------
 *      Internal Functions
 *---------------------------------------------------------------------------*/

/**
 * Enables the clock of the UDP peripheral.
 * \return 1 if peripheral status changed.
 */
static uint8_t UDP_EnablePeripheralClock(void)
{
    if (!PMC_IsPeriphEnabled(ID_UDP)) {
        PMC_EnablePeripheral(ID_UDP);
        return 1;
    }
    return 0;
}

/**
 * Disables the UDP peripheral clock.
 */
static inline void UDP_DisablePeripheralClock(void)
{
    PMC_DisablePeripheral(ID_UDP);
}

/**
 * Enables the 48MHz USB clock.
 */
static inline void UDP_EnableUsbClock(void)
{
    REG_PMC_SCER = PMC_SCER_UDP;
}

/**
 *  Disables the 48MHz USB clock.
 */
static inline void UDP_DisableUsbClock(void)
{
    REG_PMC_SCDR = PMC_SCER_UDP;
}

/**
 * Enables the UDP transceiver.
 */
static inline void UDP_EnableTransceiver(void)
{
    UDP->UDP_TXVC &= ~UDP_TXVC_TXVDIS;
}

/**
 * Disables the UDP transceiver.
 */
static inline void UDP_DisableTransceiver(void)
{
    UDP->UDP_TXVC |= UDP_TXVC_TXVDIS;
}

/**
 * Handles a completed transfer on the given endpoint, invoking the
 * configured callback if any.
 * \param bEndpoint Number of the endpoint for which the transfer has completed.
 * \param bStatus   Status code returned by the transfer operation
 */
static void UDP_EndOfTransfer(uint8_t bEndpoint, uint8_t bStatus)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    // Check that endpoint was sending or receiving data
    if( (pEndpoint->state == UDP_ENDPOINT_RECEIVING)
        || (pEndpoint->state == UDP_ENDPOINT_SENDING)) {

        Transfer *pTransfer = (Transfer *)&(pEndpoint->transfer);

        TRACE_DEBUG_WP("EoT ");

        // Endpoint returns in Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;

        // Invoke callback is present
        if (pTransfer->fCallback != 0) {

            ((TransferCallback) pTransfer->fCallback)
                (pTransfer->pArgument,
                 bStatus,
                 pTransfer->transferred,
                 pTransfer->remaining + pTransfer->buffered);
        }
        else {
            TRACE_DEBUG_WP("NoCB ");
        }
    }
    else if ( (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM)
            || (pEndpoint->state == UDP_ENDPOINT_SENDINGM) ) {

        MblTransfer *pTransfer = (MblTransfer*)&(pEndpoint->transfer);

        TRACE_DEBUG_WP("EoMT ");

        // Endpoint returns in Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;
        // Invoke callback
        if (pTransfer->fCallback != 0) {

            ((MblTransferCallback) pTransfer->fCallback)
                (pTransfer->pArgument,
                 bStatus,
                 0);
        }
        else {
            TRACE_DEBUG_WP("NoCB ");
        }
    }
}

/**
 * Clears the correct reception flag (bank 0 or bank 1) of an endpoint
 * \param bEndpoint Index of endpoint
 */
static void UDP_ClearRxFlag(uint8_t bEndpoint)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    // Clear flag and change banks
    if (pEndpoint->bank == 0) {

        CLEAR_CSR(bEndpoint, UDP_CSR_RX_DATA_BK0);
        // Swap bank if in dual-fifo mode
        if (CHIP_USB_ENDPOINTS_BANKS(bEndpoint) > 1) {

            pEndpoint->bank = 1;
        }
    }
    else {

        CLEAR_CSR(bEndpoint, UDP_CSR_RX_DATA_BK1);
        pEndpoint->bank = 0;
    }
}

/**
 * Update multi-buffer-transfer descriptors.
 * \param pTransfer Pointer to instance MblTransfer.
 * \param size      Size of bytes that processed.
 * \param forceEnd  Force the buffer END.
 * \return 1 if current buffer ended.
 */
static uint8_t UDP_MblUpdate(MblTransfer *pTransfer,
                          USBDTransferBuffer * pBi,
                          uint16_t size,
                          uint8_t forceEnd)
{
    // Update transfer descriptor
    pBi->remaining -= size;
    // Check if current buffer ended
    if (pBi->remaining == 0 || forceEnd) {

        // Process to next buffer
        if ((++ pTransfer->currBuffer) == pTransfer->listSize) {
            if (pTransfer->circList) {
                pTransfer->currBuffer = 0;
            }
            else {
                pTransfer->allUsed = 1;
            }
        }
        // All buffer in the list is processed
        if (pTransfer->currBuffer == pTransfer->freedBuffer) {
            pTransfer->allUsed = 1;
        }
        // Continue transfer, prepare for next operation
        if (pTransfer->allUsed == 0) {
            pBi = &pTransfer->pMbl[pTransfer->currBuffer];
            pBi->buffered    = 0;
            pBi->transferred = 0;
            pBi->remaining   = pBi->size;
        }
        return 1;
    }
    return 0;
}

/**
 * Transfers a data payload from the current tranfer buffer to the endpoint
 * FIFO
 * \param bEndpoint Number of the endpoint which is sending data.
 */
static uint8_t UDP_MblWriteFifo(uint8_t bEndpoint)
{
    Endpoint    *pEndpoint   = &(endpoints[bEndpoint]);
    MblTransfer *pTransfer   = (MblTransfer*)&(pEndpoint->transfer);
    USBDTransferBuffer *pBi = &(pTransfer->pMbl[pTransfer->currBuffer]);
    int32_t size;

    volatile uint8_t * pBytes;
    volatile uint8_t bufferEnd = 1;

    // Get the number of bytes to send
    size = pEndpoint->size;
    if (size > pBi->remaining) {

        size = pBi->remaining;
    }

    TRACE_DEBUG_WP("w%d.%d ", pTransfer->currBuffer, size);
    if (size == 0) {

        return 1;
    }

    pTransfer->pLastLoaded = pBi;
    pBytes = &(pBi->pBuffer[pBi->transferred + pBi->buffered]);
    pBi->buffered += size;
    bufferEnd = UDP_MblUpdate(pTransfer, pBi, size, 0);

    // Write packet in the FIFO buffer
    if (size) {
        int32_t c8 = size >> 3;
        int32_t c1 = size & 0x7;
        //printf("%d[%x] ", pBi->transferred, pBytes);
        for (; c8; c8 --) {
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);

            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
        }
        for (; c1; c1 --) {
            UDP->UDP_FDR[bEndpoint] = *(pBytes ++);
        }
    }
    return bufferEnd;
}
/*
//------------------------------------------------------------------------------
/// Transfers a data payload from an endpoint FIFO to the current transfer
/// buffer, if NULL packet received, the current buffer is ENDed.
/// \param bEndpoint Endpoint number.
/// \param wPacketSize Size of received data packet
/// \return 1 if the buffer ENDed.
//------------------------------------------------------------------------------
static uint8_t UDP_MblReadFifo(uint8_t bEndpoint, uint16_t wPacketSize)
{
    Endpoint    *pEndpoint = &(endpoints[bEndpoint]);
    MblTransfer *pTransfer = (MblTransfer*)&(pEndpoint->transfer);
    USBDTransferBuffer *pBi = &(pTransfer->pMbl[pTransfer->currBuffer]);
    uint8_t bufferEnd;

    // Check that the requested size is not bigger than the remaining transfer
    if (wPacketSize > pTransfer->remaining) {

        pTransfer->buffered += wPacketSize - pTransfer->remaining;
        wPacketSize = pTransfer->remaining;
    }

    // Update transfer descriptor information
    pBi->transferred += wPacketSize;
    bufferEnd = UDP_MblUpdate(pTransfer,
                              wPacketSize,
                              (wPacketSize < pEndpoint->size));

    // Retrieve packet
    if (wPacketSize) {
        uint8_t * pBytes = &pBi->pBuffer[pBi->transferred];
        int32_t c8 = wPacketSize >> 3;
        int32_t c1 = wPacketSize & 0x7;
        for (; c8; c8 --) {
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];

            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
        }
        for (; c1; c1 --) {
            *(pBytes ++) = UDP->UDP_FDR[bEndpoint];
        }
    }
    return bufferEnd;
}
*/
/**
 * Transfers a data payload from the current tranfer buffer to the endpoint
 * FIFO
 * \param bEndpoint Number of the endpoint which is sending data.
 */
static void UDP_WritePayload(uint8_t bEndpoint)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    int32_t size;

    // Get the number of bytes to send
    size = pEndpoint->size;
    if (size > pTransfer->remaining) {

        size = pTransfer->remaining;
    }

    // Update transfer descriptor information
    pTransfer->buffered += size;
    pTransfer->remaining -= size;

    // Write packet in the FIFO buffer
    while (size > 0) {

        UDP->UDP_FDR[bEndpoint] = *(pTransfer->pData);
        pTransfer->pData++;
        size--;
    }
}


/**
 * Transfers a data payload from an endpoint FIFO to the current transfer buffer
 * \param bEndpoint Endpoint number.
 * \param wPacketSize Size of received data packet
 */
static void UDP_ReadPayload(uint8_t bEndpoint, int32_t wPacketSize)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

    // Check that the requested size is not bigger than the remaining transfer
    if (wPacketSize > pTransfer->remaining) {

        pTransfer->buffered += wPacketSize - pTransfer->remaining;
        wPacketSize = pTransfer->remaining;
    }

    // Update transfer descriptor information
    pTransfer->remaining -= wPacketSize;
    pTransfer->transferred += wPacketSize;

    // Retrieve packet
    while (wPacketSize > 0) {

        *(pTransfer->pData) = (uint8_t) UDP->UDP_FDR[bEndpoint];
        pTransfer->pData++;
        wPacketSize--;
    }
}

/**
 * Received SETUP packet from endpoint 0 FIFO
 * \param pRequest Generic USB SETUP request sent over Control endpoints
 */
static void UDP_ReadRequest(USBGenericRequest *pRequest)
{
    uint8_t *pData = (uint8_t *)pRequest;
    uint32_t i;

    // Copy packet
    for (i = 0; i < 8; i++) {

        *pData = (uint8_t) UDP->UDP_FDR[0];
        pData++;
    }
}

/**
 * Checks if an ongoing transfer on an endpoint has been completed.
 * \param bEndpoint Endpoint number.
 * \return 1 if the current transfer on the given endpoint is complete;
 *         otherwise 0.
 */
static uint8_t UDP_IsTransferFinished(uint8_t bEndpoint)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

    // Check if it is a Control endpoint
    //  -> Control endpoint must always finish their transfer with a zero-length
    //     packet
    if ((UDP->UDP_CSR[bEndpoint] & UDP_CSR_EPTYPE)
        == UDP_CSR_EPTYPE_CTRL) {

        return (pTransfer->buffered < pEndpoint->size);
    }
    // Other endpoints only need to transfer all the data
    else {

        return (pTransfer->buffered <= pEndpoint->size)
               && (pTransfer->remaining == 0);
    }
}

/**
 * Endpoint interrupt handler.
 * Handle IN/OUT transfers, received SETUP packets and STALLing
 * \param bEndpoint Index of endpoint
 */
static void UDP_EndpointHandler(uint8_t bEndpoint)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    MblTransfer *pMblt  = (MblTransfer*)&(pEndpoint->transfer);
    uint32_t status = UDP->UDP_CSR[bEndpoint];
    uint16_t wPacketSize;
    USBGenericRequest request;

    TRACE_DEBUG_WP("E%d ", bEndpoint);
    TRACE_DEBUG_WP("st:0x%X ", status);

    // Handle interrupts
    // IN packet sent
    if ((status & UDP_CSR_TXCOMP) != 0) {

        TRACE_DEBUG_WP("Wr ");

        // Check that endpoint was in MBL Sending state
        if (pEndpoint->state == UDP_ENDPOINT_SENDINGM) {

            USBDTransferBuffer * pMbli = pMblt->pLastLoaded;
            uint8_t bufferEnd = 0;

            TRACE_DEBUG_WP("TxM%d.%d ", pMblt->allUsed, pMbli->buffered);

            // End of transfer ?
            if (pMblt->allUsed && pMbli->buffered == 0) {

                pMbli->transferred += pMbli->buffered;
                pMbli->buffered = 0;

                // Disable interrupt
                UDP->UDP_IDR = 1 << bEndpoint;
                UDP_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
                CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
            }
            else {

                // Transfer remaining data
                TRACE_DEBUG_WP("%d ", pEndpoint->size);

                if (pMbli->buffered  > pEndpoint->size) {
                    pMbli->transferred += pEndpoint->size;
                    pMbli->buffered -= pEndpoint->size;
                }
                else {
                    pMbli->transferred += pMbli->buffered;
                    pMbli->buffered  = 0;
                }

                // Send next packet
                if (CHIP_USB_ENDPOINTS_BANKS(bEndpoint) == 1) {

                    // No double buffering
                    bufferEnd = UDP_MblWriteFifo(bEndpoint);
                    SET_CSR(bEndpoint, UDP_CSR_TXPKTRDY);
                    CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
                }
                else {
                    // Double buffering
                    SET_CSR(bEndpoint, UDP_CSR_TXPKTRDY);
                    CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
                    bufferEnd = UDP_MblWriteFifo(bEndpoint);
                }

                if (bufferEnd && pMblt->fCallback) {
                    ((MblTransferCallback) pTransfer->fCallback)
                        (pTransfer->pArgument,
                         USBD_STATUS_PARTIAL_DONE,
                         1);
                }
            }
        }
        // Check that endpoint was in Sending state
        else if (pEndpoint->state == UDP_ENDPOINT_SENDING) {

            // End of transfer ?
            if (UDP_IsTransferFinished(bEndpoint)) {

                pTransfer->transferred += pTransfer->buffered;
                pTransfer->buffered = 0;

                // Disable interrupt if this is not a control endpoint
                if ((status & UDP_CSR_EPTYPE) != UDP_CSR_EPTYPE_CTRL) {

                    UDP->UDP_IDR = 1 << bEndpoint;
                }

                UDP_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
                CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
            }
            else {

                // Transfer remaining data
                TRACE_DEBUG_WP(" %d ", pEndpoint->size);

                pTransfer->transferred += pEndpoint->size;
                pTransfer->buffered -= pEndpoint->size;

                // Send next packet
                if (CHIP_USB_ENDPOINTS_BANKS(bEndpoint) == 1) {

                    // No double buffering
                    UDP_WritePayload(bEndpoint);
                    SET_CSR(bEndpoint, UDP_CSR_TXPKTRDY);
                    CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
                }
                else {
                    // Double buffering
                    SET_CSR(bEndpoint, UDP_CSR_TXPKTRDY);
                    CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
                    UDP_WritePayload(bEndpoint);
                }
            }
        }
        else {
            // Acknowledge interrupt
            TRACE_ERROR("Error Wr");
            CLEAR_CSR(bEndpoint, UDP_CSR_TXCOMP);
        }
    }

    // OUT packet received
    if ((status & UDP_CSR_RXDATA_BK) != 0) {

        TRACE_DEBUG_WP("Rd ");

        // Check that the endpoint is in Receiving state
        if (pEndpoint->state != UDP_ENDPOINT_RECEIVING) {

            // Check if an ACK has been received on a Control endpoint
            if (((status & UDP_CSR_EPTYPE) == UDP_CSR_EPTYPE_CTRL)
                && ((status & UDP_CSR_RXBYTECNT) == 0)) {

                // Acknowledge the data and finish the current transfer
                UDP_ClearRxFlag(bEndpoint);
                UDP_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
            // Check if the data has been STALLed
            else if ((status & UDP_CSR_FORCESTALL) != 0) {

                // Discard STALLed data
                TRACE_DEBUG_WP("Discard ");
                UDP_ClearRxFlag(bEndpoint);
            }
            // NAK the data
            else {

                TRACE_DEBUG_WP("Nak ");
                UDP->UDP_IDR = 1 << bEndpoint;
            }
        }
        // Endpoint is in Read state
        else {

            // Retrieve data and store it into the current transfer buffer
            wPacketSize = (uint16_t) (status >> 16);
            TRACE_DEBUG_WP("%d ", wPacketSize);
            UDP_ReadPayload(bEndpoint, wPacketSize);
            UDP_ClearRxFlag(bEndpoint);

            // Check if the transfer is finished
            if ((pTransfer->remaining == 0) || (wPacketSize < pEndpoint->size)) {

                // Disable interrupt if this is not a control endpoint
                if ((status & UDP_CSR_EPTYPE) != UDP_CSR_EPTYPE_CTRL) {

                    UDP->UDP_IDR = 1 << bEndpoint;
                }
                UDP_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
        }
    }

    // STALL sent
    if ((status & UDP_CSR_STALLSENTISOERROR) != 0) {

        CLEAR_CSR(bEndpoint, UDP_CSR_STALLSENTISOERROR);

        if (   (status & UDP_CSR_EPTYPE) == UDP_CSR_EPTYPE_ISO_IN
            || (status & UDP_CSR_EPTYPE) == UDP_CSR_EPTYPE_ISO_OUT ) {

            TRACE_WARNING("Isoe [%d] ", bEndpoint);
            UDP_EndOfTransfer(bEndpoint, USBD_STATUS_ABORTED);
        }
        else {

            TRACE_WARNING("Sta 0x%X [%d] ", status, bEndpoint);

            if (pEndpoint->state != UDP_ENDPOINT_HALTED) {

                TRACE_WARNING( "_ " );
                // If the endpoint is not halted, clear the STALL condition
                CLEAR_CSR(bEndpoint, UDP_CSR_FORCESTALL);
            }
        }
    }

    // SETUP packet received
    if ((status & UDP_CSR_RXSETUP) != 0) {

        TRACE_DEBUG_WP("Stp ");

        // If a transfer was pending, complete it
        // Handles the case where during the status phase of a control write
        // transfer, the host receives the device ZLP and ack it, but the ack
        // is not received by the device
        if ((pEndpoint->state == UDP_ENDPOINT_RECEIVING)
            || (pEndpoint->state == UDP_ENDPOINT_SENDING)) {

            UDP_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
        }
        // Copy the setup packet
        UDP_ReadRequest(&request);

        // Set the DIR bit before clearing RXSETUP in Control IN sequence
        if (USBGenericRequest_GetDirection(&request) == USBGenericRequest_IN) {

            SET_CSR(bEndpoint, UDP_CSR_DIR);
        }
        // Acknowledge setup packet
        CLEAR_CSR(bEndpoint, UDP_CSR_RXSETUP);

        // Forward the request to the upper layer
        USBD_RequestHandler(0, &request);
    }

}

/*---------------------------------------------------------------------------
 *      Exported functions
 *---------------------------------------------------------------------------*/

/**
 * USBD (UDP) interrupt handler
 * Manages device resume, suspend, end of bus reset. 
 * Forwards endpoint events to the appropriate handler.
 */
void USBD_IrqHandler(void)
{
    uint32_t status;
    int32_t eptnum = 0;

    /* Enable peripheral ? */
    //UDP_EnablePeripheralClock();

    /* Get interrupt status
       Some interrupts may get masked depending on the device state */
    status = UDP->UDP_ISR;
    status &= UDP->UDP_IMR;

    if (USBD_GetState() < USBD_STATE_POWERED) {

        status &= UDP_ICR_WAKEUP | UDP_ICR_RXRSM;
        UDP->UDP_ICR = ~status;
    }

    /* Return immediately if there is no interrupt to service */
    if (status == 0) {

        TRACE_DEBUG_WP(".\n\r");
        return;
    }

    /* Toggle USB LED if the device is active */
    if (USBD_GetState() >= USBD_STATE_POWERED) {

        LED_Set(USBD_LEDUSB);
    }

    /* Service interrupts */

    //// Start Of Frame (SOF)
    //if (ISSET(dStatus, UDP_ISR_SOFINT)) {
    //
    //    TRACE_DEBUG("SOF");
    //
    //    // Invoke the SOF callback
    //    USB_StartOfFrameCallback(pUsb);
    //
    //    // Acknowledge interrupt
    //    UDP->UDP_ICR = UDP_ICR_SOFINT;
    //    dStatus &= ~UDP_ISR_SOFINT;
    //}

    /* Suspend
       This interrupt is always treated last (hence the '==') */
    if (status == UDP_ISR_RXSUSP) {

        TRACE_INFO_WP("Susp ");
        /* Enable wakeup */
        UDP->UDP_IER = UDP_IER_WAKEUP | UDP_IER_RXRSM;
        /* Acknowledge interrupt */
        UDP->UDP_ICR = UDP_ICR_RXSUSP;
        /* Do suspend operations */
        USBD_SuspendHandler();
    }
    /* Resume */
    else if ((status & (UDP_ISR_WAKEUP | UDP_ISR_RXRSM)) != 0) {

        TRACE_INFO_WP("Res ");
        /* Clear and disable resume interrupts */
        UDP->UDP_ICR = UDP_ICR_WAKEUP | UDP_ICR_RXRSM | UDP_ICR_RXSUSP;
        UDP->UDP_IDR = UDP_IDR_WAKEUP | UDP_IDR_RXRSM;
        /* Do resome operations */
        USBD_ResumeHandler();
    }
    /* End of bus reset */
    else if ((status & UDP_ISR_ENDBUSRES) != 0) {

        TRACE_INFO_WP("EoBRes ");
        /* Flush and enable the Suspend interrupt */
        UDP->UDP_ICR = UDP_ICR_WAKEUP | UDP_ICR_RXRSM | UDP_ICR_RXSUSP;
        UDP->UDP_IER = UDP_IER_RXSUSP;

        /* Do RESET operations */
        USBD_ResetHandler();

        /* Acknowledge end of bus reset interrupt */
        UDP->UDP_ICR = UDP_ICR_ENDBUSRES;
    }
    /* Endpoint interrupts */
    else {

        while (status != 0) {

            /* Check if endpoint has a pending interrupt */
            if ((status & (1 << eptnum)) != 0) {
            
                UDP_EndpointHandler(eptnum);
                status &= ~(1 << eptnum);
                
                if (status != 0) {
                
                    TRACE_INFO_WP("\n\r  - ");
                }
            }
            eptnum++;
        }
    }

    /* Toggle LED back to its previous state */
    TRACE_DEBUG_WP("!");
    TRACE_INFO_WP("\n\r");
    if (USBD_GetState() >= USBD_STATE_POWERED) {

        LED_Clear(USBD_LEDUSB);
    }
}

/**
 * \brief Reset endpoints and disable them.
 * -# Terminate transfer if there is any, with given status;
 * -# Reset the endpoint & disable it.
 * \param bmEPs   Bitmap for endpoints to reset.
 * \param bStatus Status passed to terminate transfer on endpoint.
 * \note Use USBD_HAL_ConfigureEP() to configure and enable endpoint.
 * \sa USBD_HAL_ConfigureEP().
 */
void USBD_HAL_ResetEPs(uint32_t bmEPs, uint8_t bStatus)
{
    Endpoint *pEndpoint;
    Transfer *pTransfer;
    uint32_t tmp = bmEPs & ((1<<CHIP_USB_NUMENDPOINTS)-1);
    uint8_t  ep;
    for (ep = 0; ep < CHIP_USB_NUMENDPOINTS; ep ++) {
        if (tmp & 1) {
            /* Terminate transfer on this EP */
            UDP_EndOfTransfer(ep, bStatus);
            /* Reset transfer information */
            pEndpoint = &(endpoints[ep]);
            pTransfer = (Transfer*)&(pEndpoint->transfer);
            /* Reset transfer descriptor */
            pTransfer->pData = 0;
            pTransfer->transferred = -1;
            pTransfer->buffered = -1;
            pTransfer->remaining = -1;
            pTransfer->fCallback = 0;
            pTransfer->pArgument = 0;
            /* Reset endpoint state */
            pEndpoint->bank = 0;
            pEndpoint->state = UDP_ENDPOINT_DISABLED;
        }
        tmp >>= 1;
    }
    /* Reset EPs */
    UDP->UDP_RST_EP |=  bmEPs;
    UDP->UDP_RST_EP &= ~bmEPs;
}

/**
 * Configures an endpoint according to its endpoint Descriptor.
 * \param pDescriptor Pointer to an endpoint descriptor.
 */
uint8_t USBD_HAL_ConfigureEP(const USBEndpointDescriptor *pDescriptor)
{
    Endpoint *pEndpoint;
    uint8_t bEndpoint;
    uint8_t bType;
    uint8_t bEndpointDir;

    /* NULL descriptor -> Control endpoint 0 in default */
    if (pDescriptor == 0) {
        bEndpoint = 0;
        pEndpoint = &(endpoints[bEndpoint]);
        bType= USBEndpointDescriptor_CONTROL;
        bEndpointDir = 0;
        pEndpoint->size = CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0);
    }
    /* Device descriptor -> Specific Control EP */
    else if (pDescriptor->bDescriptorType == USBGenericDescriptor_DEVICE) {
        bEndpoint = 0;
        pEndpoint = &(endpoints[bEndpoint]);
        bType = USBEndpointDescriptor_CONTROL;
        bEndpointDir = 0;
        pEndpoint->size = ((USBDeviceDescriptor *)pDescriptor)->bMaxPacketSize0;
    }
    /* Not endpoint descriptor, ERROR! */
    else if (pDescriptor->bDescriptorType != USBGenericDescriptor_ENDPOINT) {
        return 0xFF;
    }
    else {
        bEndpoint = USBEndpointDescriptor_GetNumber(pDescriptor);
        pEndpoint = &(endpoints[bEndpoint]);
        bType = USBEndpointDescriptor_GetType(pDescriptor);
        bEndpointDir = USBEndpointDescriptor_GetDirection(pDescriptor);
        pEndpoint->size = USBEndpointDescriptor_GetMaxPacketSize(pDescriptor);
    }

    /* Abort the current transfer is the endpoint was configured and in
       Write or Read state */
    if ((pEndpoint->state == UDP_ENDPOINT_RECEIVING)
        || (pEndpoint->state == UDP_ENDPOINT_SENDING)
        || (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM)
        || (pEndpoint->state == UDP_ENDPOINT_SENDINGM)) {
        UDP_EndOfTransfer(bEndpoint, USBD_STATUS_RESET);
    }
    pEndpoint->state = UDP_ENDPOINT_IDLE;

    /* Reset Endpoint Fifos */
    UDP->UDP_RST_EP |= (1 << bEndpoint);
    UDP->UDP_RST_EP &= ~(1 << bEndpoint);

    /* Configure endpoint */
    SET_CSR(bEndpoint, (uint32_t)UDP_CSR_EPEDS
                        | (bType << 8) | (bEndpointDir << 10));
    if (bType != USBEndpointDescriptor_CONTROL) {

    }
    else {

        UDP->UDP_IER = (1 << bEndpoint);
    }

    TRACE_INFO_WP("CfgEp%d ", bEndpoint);
    return bEndpoint;
}

/**
 * Set callback for a USB endpoint for transfer (read/write).
 *
 * \param bEP       Endpoint number.
 * \param fCallback Optional callback function to invoke when the transfer is
 *                  complete.
 * \param pCbData   Optional pointer to data to the callback function.
 * \return USBD_STATUS_SUCCESS or USBD_STATUS_LOCKED if endpoint is busy.
 */
uint8_t USBD_HAL_SetTransferCallback(uint8_t          bEP,
                                  TransferCallback fCallback,
                                  void             *pCbData)
{
    Endpoint *pEndpoint = &(endpoints[bEP]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    /* Check that the endpoint is IDLE */
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {
        return USBD_STATUS_LOCKED;
    }
    TRACE_DEBUG_WP("sBuf ");
    /* Setup the transfer callback and extension data */
    pTransfer->fCallback = fCallback;
    pTransfer->pArgument = pCbData;
    return USBD_STATUS_SUCCESS;
}

/**
 * Sends data through a USB endpoint. Sets up the transfer descriptor,
 * writes one or two data payloads (depending on the number of FIFO bank
 * for the endpoint) and then starts the actual transfer. The operation is
 * complete when all the data has been sent.
 *
 * *If the size of the buffer is greater than the size of the endpoint
 *  (or twice the size if the endpoint has two FIFO banks), then the buffer
 *  must be kept allocated until the transfer is finished*. This means that
 *  it is not possible to declare it on the stack (i.e. as a local variable
 *  of a function which returns after starting a transfer).
 *
 * \param bEndpoint Endpoint number.
 * \param pData Pointer to a buffer with the data to send.
 * \param dLength Size of the data buffer.
 * \return USBD_STATUS_SUCCESS if the transfer has been started;
 *         otherwise, the corresponding error status code.
 */
uint8_t USBD_HAL_Write( uint8_t          bEndpoint,
                        const void       *pData,
                        uint32_t         dLength)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

    /* Check that the endpoint is in Idle state */
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        return USBD_STATUS_LOCKED;
    }
    TRACE_DEBUG_WP("Write%d(%d) ", bEndpoint, dLength);

    /* Setup the transfer descriptor */
    pTransfer->pData = (void *) pData;
    pTransfer->remaining = dLength;
    pTransfer->buffered = 0;
    pTransfer->transferred = 0;

    /* Send the first packet */
    pEndpoint->state = UDP_ENDPOINT_SENDING;
    while((UDP->UDP_CSR[bEndpoint]&UDP_CSR_TXPKTRDY)==UDP_CSR_TXPKTRDY);
    UDP_WritePayload(bEndpoint);
    SET_CSR(bEndpoint, UDP_CSR_TXPKTRDY);

    /* If double buffering is enabled and there is data remaining,
       prepare another packet */
    if ((CHIP_USB_ENDPOINTS_BANKS(bEndpoint) > 1) && (pTransfer->remaining > 0)) {

        UDP_WritePayload(bEndpoint);
    }

    /* Enable interrupt on endpoint */
    UDP->UDP_IER = 1 << bEndpoint;

    return USBD_STATUS_SUCCESS;
}

/**
 * Reads incoming data on an USB endpoint This methods sets the transfer
 * descriptor and activate the endpoint interrupt. The actual transfer is
 * then carried out by the endpoint interrupt handler. The Read operation
 * finishes either when the buffer is full, or a short packet (inferior to
 * endpoint maximum  size) is received.
 *
 * *The buffer must be kept allocated until the transfer is finished*.
 * \param bEndpoint Endpoint number.
 * \param pData Pointer to a data buffer.
 * \param dLength Size of the data buffer in bytes.
 * \return USBD_STATUS_SUCCESS if the read operation has been started;
 *         otherwise, the corresponding error code.
 */
uint8_t USBD_HAL_Read(uint8_t    bEndpoint,
                      void       *pData,
                      uint32_t   dLength)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

    /* Return if the endpoint is not in IDLE state */
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        return USBD_STATUS_LOCKED;
    }

    /* Endpoint enters Receiving state */
    pEndpoint->state = UDP_ENDPOINT_RECEIVING;
    TRACE_DEBUG_WP("Read%d(%d) ", bEndpoint, dLength);

    /* Set the transfer descriptor */
    pTransfer->pData = pData;
    pTransfer->remaining = dLength;
    pTransfer->buffered = 0;
    pTransfer->transferred = 0;

    /* Enable interrupt on endpoint */
    UDP->UDP_IER = 1 << bEndpoint;

    return USBD_STATUS_SUCCESS;
}

/**
 *  \brief Enable Pull-up, connect.
 *
 *  -# Enable HW access if needed
 *  -# Enable Pull-Up
 *  -# Disable HW access if needed
 */
void USBD_HAL_Connect(void)
{
    uint8_t dis = UDP_EnablePeripheralClock();
    UDP->UDP_TXVC |= UDP_TXVC_PUON;
    if (dis) UDP_DisablePeripheralClock();
}

/**
 *  \brief Disable Pull-up, disconnect.
 *
 *  -# Enable HW access if needed
 *  -# Disable PULL-Up
 *  -# Disable HW access if needed
 */
void USBD_HAL_Disconnect(void)
{
    uint8_t dis = UDP_EnablePeripheralClock();
    UDP->UDP_TXVC &= ~UDP_TXVC_PUON;
    if (dis) UDP_DisablePeripheralClock();
}

/**
 * Starts a remote wake-up procedure.
 */
void USBD_HAL_RemoteWakeUp(void)
{
    UDP_EnablePeripheralClock();
    UDP_EnableUsbClock();
    UDP_EnableTransceiver();

    TRACE_INFO_WP("RWUp ");

    // Activates a remote wakeup (edge on ESR), then clear ESR
    UDP->UDP_GLB_STAT |= UDP_GLB_STAT_ESR;
    UDP->UDP_GLB_STAT &= ~UDP_GLB_STAT_ESR;
}

/**
 * Sets the device address to the given value.
 * \param address New device address.
 */
void USBD_HAL_SetAddress(uint8_t address)
{
    /* Set address */
    UDP->UDP_FADDR = UDP_FADDR_FEN | (address & UDP_FADDR_FADD);
    /* If the address is 0, the device returns to the Default state */
    if (address == 0)   UDP->UDP_GLB_STAT = 0;
    /* If the address is non-zero, the device enters the Address state */
    else        UDP->UDP_GLB_STAT = UDP_GLB_STAT_FADDEN;
}

/**
 * Sets the current device configuration.
 * \param cfgnum - Configuration number to set.
 */
void USBD_HAL_SetConfiguration(uint8_t cfgnum)
{
    /* If the configuration number if non-zero, the device enters the
       Configured state */
    if (cfgnum != 0) UDP->UDP_GLB_STAT |= UDP_GLB_STAT_CONFG;
    /* If the configuration number is zero, the device goes back to the Address
       state */
    else {
        UDP->UDP_GLB_STAT = UDP_FADDR_FEN;
    }
}

/**
 * Initializes the USB HW Access driver.
 */
void USBD_HAL_Init(void)
{
    /* Must before USB & TXVC access! */
    UDP_EnablePeripheralClock();

    /* Reset & disable endpoints */
    USBD_HAL_ResetEPs(0xFFFFFFFF, USBD_STATUS_RESET);

    /* Configure the pull-up on D+ and disconnect it */
    UDP->UDP_TXVC &= ~UDP_TXVC_PUON;

    UDP_EnableUsbClock();

    UDP->UDP_IDR = 0xFE;
    UDP->UDP_IER = UDP_IER_WAKEUP;
}

/**
 * Causes the given endpoint to acknowledge the next packet it receives
 * with a STALL handshake except setup request.
 * \param bEP Endpoint number.
 * \return USBD_STATUS_SUCCESS or USBD_STATUS_LOCKED.
 */
uint8_t USBD_HAL_Stall(uint8_t bEP)
{
    Endpoint *pEndpoint = &(endpoints[bEP]);

    /* Check that endpoint is in Idle state */
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {
        TRACE_WARNING("UDP_Stall: EP%d locked\n\r", bEP);
        return USBD_STATUS_LOCKED;
    }
    /* STALL endpoint */
    SET_CSR(bEP, UDP_CSR_FORCESTALL);
    TRACE_DEBUG_WP("Stall%d ", bEP);
    return USBD_STATUS_SUCCESS;
}

/**
 * Sets/Clear/Get the HALT state on the endpoint.
 * In HALT state, the endpoint should keep stalling any packet.
 * \param bEndpoint Endpoint number.
 * \param ctl       Control code CLR/HALT/READ.
 *                  0: Clear HALT state;
 *                  1: Set HALT state;
 *                  .: Return HALT status.
 */
uint8_t USBD_HAL_Halt(uint8_t bEndpoint, uint8_t ctl)
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    uint8_t status = 0;

    /* SET Halt */
    if (ctl == 1) {
        /* Check that endpoint is enabled and not already in Halt state */
        if ((pEndpoint->state != UDP_ENDPOINT_DISABLED)
            && (pEndpoint->state != UDP_ENDPOINT_HALTED)) {
        
            TRACE_DEBUG_WP("Halt%d ", bEndpoint);
        
            /* Abort the current transfer if necessary */
            UDP_EndOfTransfer(bEndpoint, USBD_STATUS_ABORTED);
        
            /* Put endpoint into Halt state */
            SET_CSR(bEndpoint, UDP_CSR_FORCESTALL);
            pEndpoint->state = UDP_ENDPOINT_HALTED;
        
            /* Enable the endpoint interrupt */
            UDP->UDP_IER = 1 << bEndpoint;
        }
    }
    /* CLEAR Halt */
    else if (ctl == 0) {
        /* Check if the endpoint is halted */
        //if (pEndpoint->state != UDP_ENDPOINT_DISABLED) {
        if (pEndpoint->state == UDP_ENDPOINT_HALTED) {
        
            TRACE_DEBUG_WP("Unhalt%d ", bEndpoint);
        
            /* Return endpoint to Idle state */
            pEndpoint->state = UDP_ENDPOINT_IDLE;
        
            /* Clear FORCESTALL flag */
            CLEAR_CSR(bEndpoint, UDP_CSR_FORCESTALL);
        
            /* Reset Endpoint Fifos, beware this is a 2 steps operation */
            UDP->UDP_RST_EP |= 1 << bEndpoint;
            UDP->UDP_RST_EP &= ~(1 << bEndpoint);
        }
    }

    /* Return Halt status */
    if (pEndpoint->state == UDP_ENDPOINT_HALTED) {
        status = 1;
    }
    return( status );
}

/**
 * Indicates if the device is running in high or full-speed. Always returns 0
 * since UDP does not support high-speed mode.
 */
uint8_t USBD_HAL_IsHighSpeed(void)
{
    return 0;
}

/**
 * Suspend USB Device HW Interface
 *
 * -# Disable transceiver
 * -# Disable USB Clock
 * -# Disable USB Peripheral
 */
void USBD_HAL_Suspend(void)
{
    /* The device enters the Suspended state */
    UDP_DisableTransceiver();
    UDP_DisableUsbClock();
    UDP_DisablePeripheralClock();
}

/**
 * Activate USB Device HW Interface
 * -# Enable USB Peripheral
 * -# Enable USB Clock
 * -# Enable transceiver
 */
void USBD_HAL_Activate(void)
{
    UDP_EnablePeripheralClock();
    UDP_EnableUsbClock();
    UDP_EnableTransceiver();
}

