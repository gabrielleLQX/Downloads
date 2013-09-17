/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation

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

/** \file */

/**
 * \page sdmmc_cmd SD/MMC Low Level Commands
 *
 * \section Purpose
 * 
 * Implementation for SD/MMC low level driver. Supply a set of SD/MMC mode's
 * interface.
 *
 * \section sdmmc_prop_setup SD/MMC HW property and setup functions
 *
 * - SdmmcGetProperty()
 * - SdmmcSetBusWidth()
 * - SdmmcEnableHsMode()
 * - SdmmcSetSpeed()
 *
 * \section sdmmc_cmd_fun SD/MMC command functions
 *
 * - Functions for both SD/MMC
 *   - SdmmcPowerOn() : Perform SD/MMC power on initialization sequence.
 *   - SdmmcCmd0() : Resets all cards to idle state
 *   - SdmmcCmd2() : Asks any card to send the CID numbers on the CMD line
 *   - SdmmcCmd7() : Command toggles a card between the stand-by and transfer
 *                   states or between
 *   - SdmmcCmd9() : Addressed card sends its card-specific data (CSD) on the CMD
 *                   line
 *   - SdmmcCmd12() : Forces the card to stop transmission
 *   - SdmmcCmd13() : Addressed card sends its status register
 *   - SdmmcCmd16() : Set block length
 *   - SdmmcCmd17() : Read single block
 *   - SdmmcCmd18() : Read multiple blocks
 *   - SdmmcCmd24() : Write single block
 *   - SdmmcCmd25() : Write multiple blocks
 *   - SdmmcCmd55() : App command, should be sent before application specific
 *                    command
 *   - SdmmcRead() : Write data without any command
 *                   (usually follow SdmmcCmd18()).
 *   - SdmmcWrite() : Write data without any command
 *                    (usually follow SdmmcCmd25()).
 * - Functions for SD card.
 *   - SdCmd3() : Ask the card to publish a new relative address
 *   - SdCmd6() : SD Switch command.
 *   - SdCmd8() : Sends SD Memory Card interface condition, which includes host supply voltage 
 *                information and asks the card whether card supports voltage
 *   - SdAcmd6() : Defines the data bus width 
 *   - SdAcmd41() : Asks to all cards to send their operations conditions.
 *   - SdAcmd51() : Sends SD Card Configuration Register (SCR).
 * - Functions for MMC card
 *   - MmcCmd1() : MMC send operation condition command.
 *   - MmcCmd3() : Set a new relative address to MMC card.
 *   - MmcCmd6() : MMC Switch.
 *   - MmcCmd8() : Sends MMC EXT_CSD.
 */

#ifndef SDMMC_CMD_H
#define SDMMC_CMD_H
/** \addtogroup sdmmc_hal
 *@{
 */

/*----------------------------------------------------------------------------
 *         Header
 *----------------------------------------------------------------------------*/

/* These headers were introduced in C99
   by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdint.h>
#include <stdio.h>

/*----------------------------------------------------------------------------
 *         Constants
 *----------------------------------------------------------------------------*/

/** There was an error with the SD driver. */
#define SDMMC_ERROR                 1
/** The SD card did not answer the command. */
#define SDMMC_ERROR_NORESPONSE      2
/** The SD card is not initialized. */
#define SDMMC_ERROR_NOT_INITIALIZED 3
/** The SD card is busy. */
#define SDMMC_ERROR_BUSY            4
/** The input parameter error */
#define SDMMC_ERROR_PARAM           5
/** The operation is not supported. */
#define SDMMC_ERROR_NOT_SUPPORT     0xFE

/** Support 1-bit bus mode */
#define SDMMC_BUS_1_BIT             0x0
/** Support 4-bit bus mode */
#define SDMMC_BUS_4_BIT             0x1
/** Support 8-bit bus mode */
#define SDMMC_BUS_8_BIT             0x2

/** SD/MMC card block size in bytes. */
#define SDMMC_BLOCK_SIZE            512

/** SD/MMC card block size binary shift value. */
#define SDMMC_BLOCK_SIZE_SHIFT      9

/** SD/MMC command status: ready */
#define SDMMC_CMD_READY             0
/** SD/MMC command status: waiting command end */
#define SDMMC_CMD_PENDING           1
/** SD/MMC command status: error */
#define SDMMC_CMD_ERROR             2

/** SD/MMC Low Level Property: Bus mode */
#define SDMMC_PROP_BUS_MODE       0
/** SD/MMC Low Level Property: High-speed mode */
#define SDMMC_PROP_HS_MODE        1

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/

/** SD/MMC end-of-transfer callback function. */
typedef void (*SdmmcCallback)(uint8_t status, void *pArg);

/**
 * \typedef SdCard
 * Sdcard driver structure. It holds the current command being processed and
 * the SD card address.
 */
typedef struct _SdCard {

    /** Pointer to the underlying MCI driver. */
    void *pSdDriver;
    /* Current MultiMedia command being processed. */
    //SdCmd command;
    /** Card IDentification (CID register) */
    uint32_t cid[4];    
    /** Card-specific data (CSD register) */
    uint32_t csd[4];
    /** Previous access block number. */
    uint32_t preBlock;
    /** Card total size */
    uint32_t totalSize;
    /** Card block number */
    uint32_t blockNr;
    /** SD SCR(64 bit), Status(512 bit) or
        MMC EXT_CSD(512 bytes) register */
    uint32_t extData[512 / 4];
    /** Card option command support list */
    uint32_t optCmdBitMap;
    /** Card CSD TRANS_SPEED */
    uint32_t transSpeed;
    /** SD card current address. */
    unsigned short cardAddress;
    /** Card type */
    uint8_t cardType;
    /** Card access mode */
    uint8_t mode;
    /** State after sd command complete */
    uint8_t state;
} SdCard;

/**
 * \typedef MmcCmd6Arg
 * Argument for MMC CMD6
 */
typedef struct _MmcCmd6Arg {
    uint8_t access;
    uint8_t index;
    uint8_t value;
    uint8_t cmdSet;
} MmcCmd6Arg;

/**
 * \typedef SdCmd6Arg
 * Argument for SD CMD6
 */
typedef struct _SdCmd6Arg {
    uint32_t accessMode:4,  /** [ 3: 0] function group 1, access mode */
             command:4,     /** [ 7: 4] function group 2, command system */
             reserveFG3:4,  /** [11: 8] function group 3, 0xF or 0x0 */
             reserveFG4:4,  /** [15:12] function group 4, 0xF or 0x0 */
             reserveFG5:4,  /** [19:16] function group 5, 0xF or 0x0 */
             reserveFG6:4,  /** [23:20] function group 6, 0xF or 0x0 */
             reserved:7,    /** [30:24] reserved 0 */
             mode:1;        /** [31   ] Mode, 0: Check, 1: Switch */
} SdCmd6Arg;

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/

extern uint8_t SdmmcPowerOn(SdCard * pSd,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd0(SdCard * pSd, uint32_t arg, SdmmcCallback fCallback);

extern uint8_t SdmmcCmd2(SdCard * pSd,
    uint32_t * pCID, SdmmcCallback fCallback);

extern uint8_t SdmmcCmd7(SdCard * pSd,
    uint16_t cardAddr, SdmmcCallback fCallback);

extern uint8_t SdmmcCmd9(SdCard * pSd,
                         uint16_t cardAddr,
                         uint32_t * pCSD,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd12(SdCard * pSd,
                          uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd13(SdCard * pSd,
                          uint16_t cardAddr,
                          uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd17(SdCard * pSd,
    uint16_t blockSize, uint8_t * pData, uint32_t address,
    uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd16(SdCard * pSd, uint16_t blockLength,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd18(SdCard * pSd,
    uint16_t blockSize,uint16_t nbBlocks,uint8_t * pData,uint32_t address,
    uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd24(SdCard * pSd,
    uint16_t blockSize, uint8_t * pData, uint32_t address,
    uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd25(SdCard * pSd,
    uint16_t blockSize,uint16_t nbBlock,uint8_t * pData,uint32_t address,
    uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdmmcCmd55(SdCard * pSd,
    uint16_t cardAddr,SdmmcCallback fCallback);

extern uint8_t SdCmd3(SdCard * pSd,uint32_t * pRsp, SdmmcCallback fCallback);

extern uint8_t SdCmd6(SdCard * pSd,
    const void * pSwitchArg,uint32_t * pStatus,uint32_t * pResp,
    SdmmcCallback fCallback);

extern uint8_t SdCmd8(SdCard * pSd,uint8_t supplyVoltage,SdmmcCallback fCallback);

extern uint8_t SdAcmd6(SdCard * pSd,
    uint32_t arg, uint32_t * pStatus,SdmmcCallback fCallback);

extern uint8_t SdAcmd13(SdCard * pSd,uint32_t * pSdSTAT,SdmmcCallback fCallback);

extern uint8_t SdAcmd41(SdCard * pSd,uint32_t * pIo,SdmmcCallback fCallback);

extern uint8_t SdAcmd51(SdCard * pSd,uint32_t * pSCR,SdmmcCallback fCallback);

extern uint8_t MmcCmd1(SdCard * pSd,uint32_t * pOCR,SdmmcCallback fCallback);

extern uint8_t MmcCmd3(SdCard * pSd,
    uint16_t cardAddr,SdmmcCallback fCallback);

extern uint8_t MmcCmd6(SdCard * pSd,
    const void * pSwitchArg,
    uint32_t * pResp,SdmmcCallback fCallback);

extern uint8_t MmcCmd8(SdCard * pSd,uint8_t * pEXT,SdmmcCallback fCallback);

extern uint8_t SdmmcRead(SdCard * pSd,
    uint16_t blockSize, uint16_t nbBlock, uint8_t * pData,
    SdmmcCallback fCallback,void * pArg);

extern uint8_t SdmmcWrite(SdCard * pSd,
    uint16_t blockSize, uint16_t nbBlock, const uint8_t * pData,
    SdmmcCallback fCallback,void * pArg);

extern uint32_t SdmmcGetProperty(SdCard *pSd, uint8_t property);

extern uint8_t SdmmcSetBusWidth(SdCard *pSd, uint8_t busWidth);

extern uint8_t SdmmcEnableHsMode(SdCard *pSd, uint8_t enable);

extern uint32_t SdmmcSetSpeed(SdCard *pSd, uint32_t clock);

/**@}*/
#endif //#ifndef SDMMC_CMD_H

