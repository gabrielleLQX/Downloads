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
 *  \file
 */

/**
 *
 * \section Purpose
 *
 * MultiMediaCard(MCI) Interface Driver
 *
 * \section Usage
 *
 * -# MCI_Init: Initializes a MCI driver instance and the underlying peripheral.
 * -# MCI_SendCommand: Starts a MCI  transfer which described by \ref MciCmd.
 * -# MCI_Handler : Interrupt handler which is called by ISR handler.
 * -# MCI_SetSpeed : Configure the  MCI CLKDIV in the _MR register.
 * -# MCI_SetBusWidth : Configure the  MCI SDCBUS in the _SDCR register.
 * -# MCI_EnableHsMode : Configure the MCI HSMODE in the _CFG register.
 */

#ifndef HSMCID_H
#define HSMCID_H
/** \addtogroup sdmmc_hal
 *@{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

/* These headers were introduced in C99
   by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdint.h>
#include <stdio.h>

#include <board.h>

#include <memories/sdmmc/sdmmc_cmd.h>

/*----------------------------------------------------------------------------
 *         Constants
 *----------------------------------------------------------------------------*/

/* Transfer type */

/** MultiMedia Transfer type: no data */
#define MCI_NO_TRANSFER     0
/** MultiMedia Transfer type: Device to Host (read) */
#define MCI_START_READ      1
/** MultiMedia Transfer type: Host to Device (write) & check BUSY */
#define MCI_START_WRITE     2
/** Device to Host (read) without command */
#define MCI_READ            3
/** Host to Device (write) without command & check BUSY */
#define MCI_WRITE           4
/** MultiMedia Transfer type: STOP transfer */
#define MCI_STOP_TRANSFER   5

/** MCI Initialize clock 400K Hz */
#define MCI_INITIAL_SPEED   400000

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/

/**
 * \brief MCI Transfer Request prepared by the application upper layer. 
 * 
 * This structure is sent to the MCI_SendCommand function to start the transfer.
 * At the end of the transfer, the callback is invoked.
 */
typedef struct _MciCmd {

    /** Command code. */
    uint32_t cmd;
    /** Command argument. */
    uint32_t arg;
    /** Data buffer, with MCI_DMA_ENABLE defined 1, the buffer can be
     * 1, 2 or 4 bytes aligned. It has to be 4 byte aligned if no DMA.
     */
    uint8_t *pData;
    /** Size of data block in bytes. */
    uint16_t blockSize;
    /** Number of blocks to be transfered */
    uint16_t nbBlock;
    /** Response buffer. */
    uint32_t  *pResp;
    /** Optional user-provided callback function. */
    SdmmcCallback callback;
    /** Optional argument to the callback function. */
    void *pArg;
    /** SD card command option. */
    uint8_t resType:7,      /** Response */
            busyCheck:1;    /** Check busy as end of command */
    /** Indicate transfer type */
    uint8_t tranType;

    /** Indicate end of transfer status */
    uint8_t status;

    /** Command state. */
    volatile uint8_t state;
} MciCmd;

/**
 *  \brief MCI Driver
 */
typedef struct {

    /** Pointer to a MCI peripheral. */
    Hsmci *pMciHw;
    /** Pointer to currently executing command. */
    MciCmd *pCommand;
    /** MCI peripheral identifier. */
    uint8_t mciId;
    /** Mutex. */
    volatile uint8_t semaphore;
} Mcid;

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/

extern void MCI_Init(
    Mcid  *pMci,
    Hsmci *pMciHw,
    uint8_t mciId);

extern void MCI_Handler(Mcid *pMci);

extern uint32_t MCI_SetSpeed(Mcid *pMci,
                             uint32_t mciSpeed,
                             uint32_t mck);

extern uint8_t MCI_SendCommand(Mcid *pMci, MciCmd *pMciCmd);

extern uint8_t MCI_IsCommandComplete(Mcid *pMci);

extern uint8_t MCI_EnableHsMode(Mcid * pMci, uint8_t hsEnable);

extern uint8_t MCI_SetBusWidth(Mcid *pMci, uint8_t busWidth);

/**@}*/
#endif //#ifndef HSMCID_H

