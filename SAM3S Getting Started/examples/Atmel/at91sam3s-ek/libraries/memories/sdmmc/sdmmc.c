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

/** \addtogroup sdmmc_api
 *  @{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

/* These headers were introduced in C99
   by working group ISO/IEC JTC1/SC22/WG14. */
#include <stdbool.h>
#include <stdint.h>

#include "sdmmc.h"

#include <utility/assert.h>
#include <utility/trace.h>

#include <string.h>

/*----------------------------------------------------------------------------
 *         Global variables
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Local constants
 *----------------------------------------------------------------------------*/

/* SD card operation states */
#define SD_STATE_IDLE        0
#define SD_STATE_INIT        1
#define SD_STATE_READY       2
#define SD_STATE_READ     0x10
#define SD_STATE_RD_RDY   0x11
#define SD_STATE_RD_BSY   0x12
#define SD_STATE_WRITE    0x20
#define SD_STATE_WR_RDY   0x21
#define SD_STATE_WR_BSY   0x22
#define SD_STATE_BOOT     0x30

/* Card type */
#define UNKNOWN_CARD      0
#define CARD_SD           1
#define CARD_SDHC         2
#define CARD_MMC          3
#define CARD_MMCHD        4

/* Delay between sending MMC commands */
#define MMC_DELAY     0x4FF

/* Status register constants */
#define STATUS_APP_CMD          (1 << 5)
#define STATUS_SWITCH_ERROR     (1 << 7)
#define STATUS_READY_FOR_DATA   (1 << 8)
#define STATUS_IDLE             (0 << 9)
#define STATUS_READY            (1 << 9)
#define STATUS_IDENT            (2 << 9)
#define STATUS_STBY             (3 << 9)
#define STATUS_TRAN             (4 << 9)
#define STATUS_DATA             (5 << 9)
#define STATUS_RCV              (6 << 9)
#define STATUS_PRG              (7 << 9)
#define STATUS_DIS              (8 << 9)
#define STATUS_STATE          (0xF << 9)
#define STATUS_ERASE_RESET       (1 << 13)
#define STATUS_WP_ERASE_SKIP     (1 << 15)
#define STATUS_CIDCSD_OVERWRITE  (1 << 16)
#define STATUS_OVERRUN           (1 << 17)
#define STATUS_UNERRUN           (1 << 18)
#define STATUS_ERROR             (1 << 19)
#define STATUS_CC_ERROR          (1 << 20)
#define STATUS_CARD_ECC_FAILED   (1 << 21)
#define STATUS_ILLEGAL_COMMAND   (1 << 22)
#define STATUS_COM_CRC_ERROR     (1 << 23)
#define STATUS_UN_LOCK_FAILED    (1 << 24)
#define STATUS_CARD_IS_LOCKED    (1 << 25)
#define STATUS_WP_VIOLATION      (1 << 26)
#define STATUS_ERASE_PARAM       (1 << 27)
#define STATUS_ERASE_SEQ_ERROR   (1 << 28)
#define STATUS_BLOCK_LEN_ERROR   (1 << 29)
#define STATUS_ADDRESS_MISALIGN  (1 << 30)
#define STATUS_ADDR_OUT_OR_RANGE (1 << 31)

#define STATUS_STOP ( STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA )

#define STATUS_WRITE ( STATUS_ADDR_OUT_OR_RANGE \
                        | STATUS_ADDRESS_MISALIGN \
                        | STATUS_BLOCK_LEN_ERROR \
                        | STATUS_WP_VIOLATION \
                        | STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_ERASE_RESET \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA )

#define STATUS_READ  ( STATUS_ADDR_OUT_OR_RANGE \
                        | STATUS_ADDRESS_MISALIGN \
                        | STATUS_BLOCK_LEN_ERROR \
                        | STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CARD_ECC_FAILED \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_ERASE_RESET \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA )

#define STATUS_SD_SWITCH ( STATUS_ADDR_OUT_OR_RANGE \
                            | STATUS_CARD_IS_LOCKED \
                            | STATUS_COM_CRC_ERROR \
                            | STATUS_ILLEGAL_COMMAND \
                            | STATUS_CARD_ECC_FAILED \
                            | STATUS_CC_ERROR \
                            | STATUS_ERROR \
                            | STATUS_UNERRUN \
                            | STATUS_OVERRUN \
                            | STATUS_STATE)

#define STATUS_MMC_SWITCH ( STATUS_CARD_IS_LOCKED \
                            | STATUS_COM_CRC_ERROR \
                            | STATUS_ILLEGAL_COMMAND \
                            | STATUS_CC_ERROR \
                            | STATUS_ERROR \
                            | STATUS_ERASE_RESET \
                            | STATUS_STATE \
                            | STATUS_READY_FOR_DATA \
                            | STATUS_SWITCH_ERROR )

//-----------------------------------------------------------------------------
/// OCR Register
//-----------------------------------------------------------------------------
#define AT91C_VDD_16_17          (1 << 4)
#define AT91C_VDD_17_18          (1 << 5)
#define AT91C_VDD_18_19          (1 << 6)
#define AT91C_VDD_19_20          (1 << 7)
#define AT91C_VDD_20_21          (1 << 8)
#define AT91C_VDD_21_22          (1 << 9)
#define AT91C_VDD_22_23          (1 << 10)
#define AT91C_VDD_23_24          (1 << 11)
#define AT91C_VDD_24_25          (1 << 12)
#define AT91C_VDD_25_26          (1 << 13)
#define AT91C_VDD_26_27          (1 << 14)
#define AT91C_VDD_27_28          (1 << 15)
#define AT91C_VDD_28_29          (1 << 16)
#define AT91C_VDD_29_30          (1 << 17)
#define AT91C_VDD_30_31          (1 << 18)
#define AT91C_VDD_31_32          (1 << 19)
#define AT91C_VDD_32_33          (1 << 20)
#define AT91C_VDD_33_34          (1 << 21)
#define AT91C_VDD_34_35          (1 << 22)
#define AT91C_VDD_35_36          (1 << 23)
#define AT91C_CARD_POWER_UP_BUSY (1 << 31)
#define AT91C_MMC_OCR_BIT2930    (3 << 29)

#define AT91C_MMC_HOST_VOLTAGE_RANGE     (AT91C_VDD_27_28 +\
                                          AT91C_VDD_28_29 +\
                                          AT91C_VDD_29_30 +\
                                          AT91C_VDD_30_31 +\
                                          AT91C_VDD_31_32 +\
                                          AT91C_VDD_32_33)

// MMC OCR response for Bit 29, 30
#define AT91C_MMC_HIGH_DENSITY   (0x0UL << 29)
#define AT91C_MMC_STANDARD       (0x2UL << 29)

#define AT91C_MMC_NOHD           (0x0UL << 29)
#define AT91C_MMC_HD             (0x2UL << 29)

#define AT91C_CCS    (1 << 30)

// Optional commands
#define SD_ACMD6_SUPPORT        (1 << 0)
#define SD_ACMD13_SUPPORT       (1 << 1)
#define SD_ACMD41_SUPPORT       (1 << 2)
#define SD_ACMD51_SUPPORT       (1 << 3)

#define SD_CMD16_SUPPORT        (1 << 8)

#define CARD_ADDR(pSd)          (pSd->cardAddress)
#define BLOCK_SIZE(pSd)         (SDMMC_BLOCK_SIZE)
#define SD_ADDRESS(pSd, address) \
    ( ((pSd)->totalSize == 0xFFFFFFFF) ? \
                            (address):((address) << SDMMC_BLOCK_SIZE_SHIFT) )

/*----------------------------------------------------------------------------
 *         Local functions
 *----------------------------------------------------------------------------*/

/**
 * Delay some loop
 */
static void Delay(volatile unsigned int loop)
{
    for(;loop > 0; loop --);
}

/**
 * Get Trans Speed Value
 * \param pSd    
 */
void GetTransSpeedValue(SdCard *pSd)
{
    uint32_t unit, value;
    const uint32_t units[4] = {10, 100, 1000, 10000 }; // *Kbit/s 
    const uint32_t values_emmc[16] = {0, 10, 12, 13, 15, 20,
                                          26, 30, 35, 40, 45, 52,
                                          55, 60, 70, 80};
    const uint32_t values_sdmmc[16] = {0, 10, 12, 13, 15, 20,
                                           25, 30, 35, 40, 45, 50,
                                           55, 60, 70, 80};

    unit = (SD_CSD_TRAN_SPEED(pSd) & 0x7);
    if(unit < 4)    unit  = units[unit];
    else            return;
    value = (SD_CSD_TRAN_SPEED(pSd) >> 3) & 0xF;
    if (value < 16) {
        if (pSd->cardType >= CARD_MMC && SD_CID_BGA(pSd) == 1) {
            value = values_emmc[value];
        }
        else
            value = values_sdmmc[value];
    }
    else            return;
    TRACE_WARNING_WP("-I- SD/MMC TRANS SPEED %d KBit/s\r\n", (unit * value));
    pSd->transSpeed = (unit * value) * 1000;  
}

static inline uint8_t Pon(SdCard *pSd)
{
    return SdmmcPowerOn(pSd, NULL);
}

static inline uint8_t Cmd0(SdCard *pSd, uint8_t arg)
{
    return SdmmcCmd0(pSd, arg, NULL);
}

static inline uint8_t Cmd1(SdCard *pSd, uint8_t* pHd)
{
    uint8_t error;
    uint32_t arg;
    arg = AT91C_MMC_HOST_VOLTAGE_RANGE | AT91C_MMC_HD;
    error = MmcCmd1(pSd, &arg, NULL);
    if (error) return error;
    if (arg & AT91C_CARD_POWER_UP_BUSY) {
        *pHd = 0;
        if ((arg & AT91C_MMC_OCR_BIT2930) == AT91C_MMC_HIGH_DENSITY) {
            *pHd = 1;
        }
        return 0;
    }
    return SDMMC_ERROR_NOT_INITIALIZED;
}

static inline uint8_t Cmd2(SdCard *pSd)
{
    return SdmmcCmd2(pSd, pSd->cid, NULL);
}

static uint8_t Cmd3(SdCard *pSd)
{
    if (pSd->cardType == CARD_MMC || pSd->cardType == CARD_MMCHD)
        return MmcCmd3(pSd, 1, NULL);
    else {
        uint8_t error;
        uint32_t addr;
        error = SdCmd3(pSd, &addr, NULL);
        pSd->cardAddress = (addr >> 16);
        return error;
    }
}

static inline uint8_t Cmd7(SdCard *pSd, uint16_t address)
{
    return SdmmcCmd7(pSd, address, NULL);
}

static inline uint8_t Cmd9(SdCard *pSd)
{
    return SdmmcCmd9(pSd, CARD_ADDR(pSd), pSd->csd, NULL);
}

static uint8_t Cmd12(SdCard *pSd, uint32_t *pStatus)
{
    return SdmmcCmd12(pSd, pStatus, NULL);
}

static uint8_t Cmd13(SdCard *pSd, uint32_t *pStatus)
{
    return SdmmcCmd13(pSd, CARD_ADDR(pSd), pStatus, NULL);
}

static inline uint8_t Cmd16(SdCard *pSd, uint16_t blkLen)
{
    return SdmmcCmd16(pSd, blkLen, NULL);
}

static inline uint8_t Cmd17(SdCard *pSd,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd17(pSd, BLOCK_SIZE(pSd), pData, address, pStatus, NULL);
}

static inline uint8_t Cmd18(SdCard *pSd,
                            uint16_t nbBlock,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd18(pSd,
                      BLOCK_SIZE(pSd), nbBlock,
                      pData, address,
                      pStatus, NULL);
}

static inline uint8_t Cmd24(SdCard *pSd,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd24(pSd, BLOCK_SIZE(pSd), pData, address, pStatus, NULL);
}

static inline uint8_t Cmd25(SdCard *pSd,
                            uint16_t nbBlock,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd25(pSd,
                      BLOCK_SIZE(pSd), nbBlock,
                      pData, address,
                      pStatus, NULL);
}

/**
 * Defines the data bus width (00=1bit or 10=4 bits bus) to be used for data
 * transfer.
 * The allowed data bus widths are given in SCR register.
 * \param pSd  Pointer to a SD card driver instance.
 * \param busWidth  Bus width in bits.
 * \return the command transfer result (see SendCommand).
 */
static uint8_t Acmd6(SdCard *pSd, uint8_t busWidth)
{
    uint8_t error;
    uint32_t arg;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd6.cmd55:%d\n\r", error);
        return error;
    }
    arg = (busWidth == SDMMC_BUS_4_BIT)
        ? SD_STAT_DATA_BUS_WIDTH_4BIT : SD_STAT_DATA_BUS_WIDTH_1BIT;
    return SdAcmd6(pSd, arg, NULL, NULL);
}

/**
 * The SD Status contains status bits that are related to the SD memory Card
 * proprietary features and may be used for future application-specific usage.
 * Can be sent to a card only in 'tran_state'.
 */
static uint8_t Acmd13(SdCard *pSd, uint32_t *pSdSTAT)
{
    uint8_t error;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd13.cmd55:%d\n\r", error);
        return error;
    }
    return SdAcmd13(pSd, pSdSTAT, NULL);
}

/**
 * Asks to all cards to send their operations conditions.
 * Returns the command transfer result (see SendCommand).
 * \param pSd  Pointer to a SD card driver instance.
 * \param hcs  Shall be true if Host support High capacity.
 * \param pCCS  Set the pointed flag to 1 if hcs != 0 and SD OCR CCS flag is set.
 */
static uint8_t Acmd41(SdCard *pSd, uint8_t hcs, uint8_t *pCCS)
{
    uint8_t error;
    uint32_t arg;
    do {
        error = SdmmcCmd55(pSd, 0, NULL);
        if (error) {
            TRACE_ERROR("Acmd41.cmd55:%d\n\r", error);
            return error;
        }
        arg = AT91C_MMC_HOST_VOLTAGE_RANGE;
        if (hcs) arg |= AT91C_CCS;
        error = SdAcmd41(pSd, &arg, NULL);
        if (error) {
            TRACE_ERROR("Acmd41.cmd41:%d\n\r", error);
            return error;
        }
        *pCCS = ((arg & AT91C_CCS)!=0);
    } while ((arg & AT91C_CARD_POWER_UP_BUSY) != AT91C_CARD_POWER_UP_BUSY);
    return 0;
}

/**
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 */
static uint8_t Acmd51(SdCard* pSd, uint32_t *pSCR)
{
    uint8_t error;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd51.cmd55:%d\n\r", error);
        return error;
    }
    return SdAcmd51(pSd, pSCR, NULL);
}

/**
 * Try SW Reset several times (CMD0 with ARG 0)
 * \param pSd      Pointer to a SD card driver instance.
 * \param retry    Retry times.
 * \return 0 or MCI error code.
 */
static uint8_t SwReset(SdCard *pSd, uint32_t retry)
{
    uint32_t i;
    uint8_t error = 0;

    for (i = 0; i < retry; i ++) {
        error = Cmd0(pSd, 0);
        if (error != SDMMC_ERROR_NORESPONSE)
            break;
    }
    return error;
}

/**
 * Perform sligle block transfer
 */
static uint8_t PerformSingleTransfer(SdCard *pSd,
                                     uint32_t address,
                                     uint8_t *pData,
                                     uint8_t isRead)
{
    uint32_t status;
    uint8_t error = 0;
    /* Reset transfer state if previous in multi- mode */
    if(    (pSd->state == SD_STATE_READ)
        || (pSd->state == SD_STATE_WRITE)) {
        /* Stop transfer */
        error = Cmd12(pSd, &status);
        if (error) {
            TRACE_ERROR("SingleTx.Cmd12: st%x, er%d\n\r", pSd->state, error);
        }
        pSd->state = SD_STATE_READY;
        pSd->preBlock = 0xFFFFFFFF;
    }

    if(isRead) {
        /* Wait for card to be ready for data transfers */
        do {
            error = Cmd13(pSd, &status);
            if (error) {
                TRACE_ERROR("SingleTx.RD.Cmd13: %d\n\r", error);
                return error;
            }
            if(  ((status & STATUS_STATE) == STATUS_IDLE)
               ||((status & STATUS_STATE) == STATUS_READY)
               ||((status & STATUS_STATE) == STATUS_IDENT)) {
                TRACE_ERROR("SingleTx.mode\n\r");
                return SDMMC_ERROR_NOT_INITIALIZED;
            }
            /* If the card is in sending data state or
               in receivce data state */
            if (  ((status & STATUS_STATE) == STATUS_RCV)
                ||((status & STATUS_STATE) == STATUS_DATA) ){

                TRACE_DEBUG("SingleTx.state = 0x%X\n\r",
                            (status & STATUS_STATE) >> 9);
            }
        }
        while (    ((status & STATUS_READY_FOR_DATA) == 0)
                || ((status & STATUS_STATE) != STATUS_TRAN) );
        ASSERT((status & STATUS_STATE) == STATUS_TRAN,
                "SD Card can't be configured in transfer state 0x%X\n\r",
                (status & STATUS_STATE)>>9);
        /* Read single block */
        error = Cmd17(pSd,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        if (error) {
            TRACE_ERROR("SingleTx.Cmd17: %d\n\r", error);
            return error;
        }
        if (status & ~(STATUS_READY_FOR_DATA | STATUS_STATE)) {
            TRACE_ERROR("CMD17.stat: %x\n\r",
                status & ~(STATUS_READY_FOR_DATA | STATUS_STATE));
            return SDMMC_ERROR;
        }
        return error;
    }
    /* Write */
    {
        /* Wait for card to be ready for data transfers */
        do {
            error = Cmd13(pSd, &status);
            if (error) {
                TRACE_ERROR("SingleTx.WR.Cmd13: %d\n\r", error);
                return error;
            }
        }
        while ((status & STATUS_READY_FOR_DATA) == 0);
        /* Move to Sending data state */
        error = Cmd24(pSd,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        if (error) {
            TRACE_DEBUG("SingleTx.Cmd24: %d\n\r", error);
            return error;
        }
        if (status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE))) {
            TRACE_ERROR("CMD24(0x%x).stat: %x\n\r",
                SD_ADDRESS(pSd,address), 
                status & (STATUS_WRITE
                            & ~(STATUS_READY_FOR_DATA | STATUS_STATE)));
            return SDMMC_ERROR;
        }
    }
    return error;
}

/**
 * Move SD card to transfer state. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the transfer command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to transfer.
 * \param nbBlocks Number of blocks to be transfer, 0 for infinite transfer.
 * \param pData    Data buffer whose size is at least the block size.
 * \param isRead   1 for read data and 0 for write data.
 */
static uint8_t MoveToTransferState(SdCard *pSd,
                                   uint32_t address,
                                   uint16_t nbBlocks,
                                   uint8_t *pData,
                                   uint8_t isRead)
{
    uint32_t status;
    uint8_t error;

    if(    (pSd->state == SD_STATE_READ)
        || (pSd->state == SD_STATE_WRITE)) {
        /* Stop transfer */
        error = Cmd12(pSd, &status);
        if (error) {
            TRACE_ERROR("MTTranState.Cmd12: st%x, er%d\n\r", pSd->state, error);
            return error;
        }
    }

    if(isRead) {
        /* Wait for card to be ready for data transfers */
        do {
            error = Cmd13(pSd, &status);
            if (error) {
                TRACE_ERROR("MTTranState.RD.Cmd13: %d\n\r", error);
                return error;
            }
            if(  ((status & STATUS_STATE) == STATUS_IDLE)
               ||((status & STATUS_STATE) == STATUS_READY)
               ||((status & STATUS_STATE) == STATUS_IDENT)) {
                TRACE_ERROR("Pb Card Identification mode\n\r");
                return SDMMC_ERROR_NOT_INITIALIZED;
            }
            /* If the card is in sending data state or in receivce data state */
            if (  ((status & STATUS_STATE) == STATUS_RCV)
                ||((status & STATUS_STATE) == STATUS_DATA) ){

                TRACE_DEBUG("state = 0x%X\n\r", (status & STATUS_STATE) >> 9);
            }
        }
        while (    ((status & STATUS_READY_FOR_DATA) == 0)
                || ((status & STATUS_STATE) != STATUS_TRAN) );
        ASSERT((status & STATUS_STATE) == STATUS_TRAN,
                "SD Card can't be configured in transfer state 0x%X\n\r",
                (status & STATUS_STATE)>>9);
        /* Move to Receiving data state */
        error = Cmd18(pSd,
                      nbBlocks,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        //Cmd13(pSd, &status); //status -> 0xB00
        if (error) {
            TRACE_ERROR("MTTranState.Cmd18: %d\n\r", error);
            return error;
        }
        if (status & ~(STATUS_READY_FOR_DATA | STATUS_STATE)) {
            TRACE_ERROR("CMD18.stat: %x\n\r",
                status & ~(STATUS_READY_FOR_DATA | STATUS_STATE));
            return SDMMC_ERROR;
        }
    }
    else {
        /* Wait for card to be ready for data transfers */
        do {
            error = Cmd13(pSd, &status);
            if (error) {
                TRACE_ERROR("MoveToTransferState.WR.Cmd13: %d\n\r", error);
                return error;
            }
        }
        while ((status & STATUS_READY_FOR_DATA) == 0);
        /* Move to Sending data state */
        error = Cmd25(pSd,
                      nbBlocks,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        if (error) {
            TRACE_DEBUG("MoveToTransferState.Cmd25: %d\n\r", error);
            return error;
        }
        if (status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE))) {
            TRACE_ERROR("CMD25(0x%x, %d).stat: %x\n\r",
                SD_ADDRESS(pSd,address), nbBlocks,
                status & (STATUS_WRITE
                            & ~(STATUS_READY_FOR_DATA | STATUS_STATE)));
            return SDMMC_ERROR;
        }
    }

    if (!error) pSd->preBlock = address + (nbBlocks-1);
    return error;
}

/**
 * Switch card state between STBY and TRAN
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Card address to TRAN, 0 to STBY
 */
static uint8_t MmcSelectCard(SdCard *pSd, uint16_t address)
{
    uint8_t error;
    uint32_t  status;
    uint32_t  targetState = address ? STATUS_TRAN : STATUS_STBY;
    uint32_t  srcState    = address ? STATUS_STBY : STATUS_TRAN;

    /* At this stage the Initialization and identification process is achieved
     * The SD card is supposed to be in Stand-by State */
    while(1) {
        error = Cmd13(pSd, &status);
        if (error) {
            TRACE_ERROR("MmcSelectCard.Cmd13 (%d)\n\r", error);
            return error;
        }
        if ((status & STATUS_READY_FOR_DATA)) {
            uint32_t currState = status & STATUS_STATE;
            if (currState == targetState) return 0;
            if (currState != srcState) {
                TRACE_ERROR("MmcSelectCard, wrong state %x\n\r", currState);
                return SDMMC_ERROR;
            }
            break;
        }
    }

    /* witch to TRAN mode to Select the current SD/MMC
     * so that SD ACMD6 can process or EXT_CSD can read. */
    error = Cmd7(pSd, address);
    if (error == SDMMC_ERROR_NOT_INITIALIZED && address == 0) {}
    else if (error) {
        TRACE_ERROR("MmcSelectCard.Cmd7 (%d)\n\r", error);
    }

    return error;
}

/**
 * Get MMC EXT_CSD information
 * \param pSd      Pointer to a SD card driver instance.
 */
static void MmcGetExtInformation(SdCard *pSd)
{
    uint8_t error;
    /* MMC 4.0 Higher version */
    if(SD_CSD_STRUCTURE(pSd) >= 2) {

        error = MmcCmd8(pSd, (uint8_t*)pSd->extData, NULL);
        if (error) {
            TRACE_ERROR("MmcGetExt.Cmd8: %d\n\r", error);
        }
    }
}

/**
 * Get SCR and SD Status information
 * \param pSd      Pointer to a SD card driver instance.
 */
static void SdGetExtInformation(SdCard *pSd)
{
    uint8_t error;

    /* SD Status */
    if (pSd->optCmdBitMap & SD_ACMD13_SUPPORT) {
        error = Acmd13(pSd, &pSd->extData[SD_EXT_OFFSET_SD_STAT]);
        if (error) {
            TRACE_ERROR("SdGetExt.Acmd13: %d\n\r", error);
            pSd->optCmdBitMap &= ~SD_ACMD13_SUPPORT;
        }
    }

    /* SD SCR */
    error = Acmd51(pSd, &pSd->extData[SD_EXT_OFFSET_SD_SCR]);
    if (error) {
        TRACE_ERROR("SdGetExt.Acmd51: %d\n\r", error);
    }
}

/**
 * Update SD/MMC information.
 * Update CSD for card speed switch.
 * Update ExtDATA for any card function switch.
 * \param pSd      Pointer to a SD card driver instance.
 * \return error code when update CSD error.
 */
static void SdMmcUpdateInformation(SdCard *pSd,
                                   uint8_t csd,
                                   uint8_t extData)
{
    uint8_t error;

    /* Update CSD for new TRAN_SPEED value */
    if (csd) {
        MmcSelectCard(pSd, 0);
        Delay(800);
        error = Cmd9(pSd);
        if (error ) {
            TRACE_ERROR("SdMmcUpdateInfo.Cmd9 (%d)\n\r", error);
            return;
        }
        error = MmcSelectCard(pSd, pSd->cardAddress);
    }
    if (extData) {
        if (pSd->cardType >= CARD_MMC)     MmcGetExtInformation(pSd);
        else if (pSd->cardType >= CARD_SD) SdGetExtInformation(pSd);
    }
    GetTransSpeedValue(pSd);
}

//------------------------------------------------------------------------------
/// Run the SDcard SD Mode initialization sequence. This function runs the
/// initialisation procedure and the identification process, then it sets the
/// SD card in transfer state to set the block length and the bus width.
/// Returns 0 if successful; otherwise returns an SD_ERROR code.
/// \param pSd  Pointer to a SD card driver instance.
/// \param pSdDriver  Pointer to SD driver already initialized
//------------------------------------------------------------------------------
static uint8_t SdMmcInit(SdCard *pSd, void *pSdDriver)
{
    uint8_t  isCCSet;
    uint16_t error;
    uint32_t   status;
    uint8_t  cmd8Retries = 1;
    uint32_t   cmd1Retries = 10000;//120;
    uint8_t  isHdSupport = 0;
    uint8_t  isHsSupport = 0;
    uint8_t  updateInformation = 0;

    /* The command GO_IDLE_STATE (CMD0) is the software reset command and sets
     * card into Idle State regardless of the current card state. */
    SdmmcEnableHsMode(pSd, 0);
    error = SwReset(pSd, 1);
    if (error) {
        TRACE_ERROR("SdMmcInit.1 (%d)\n\r", error);
        return error;
    }

    /* CMD8 is newly added in the Physical Layer Specification Version 2.00 to
     * support multiple voltage ranges and used to check whether the card
     * supports supplied voltage. The version 2.00 host shall issue CMD8 and
     * verify voltage before card initialization.
     * The host that does not support CMD8 shall supply high voltage range... */
    TRACE_DEBUG("Cmd8()\n\r");
    do {
        error = SdCmd8(pSd, 1, NULL);
    }
    while ((error == SDMMC_ERROR_NORESPONSE) && (-- cmd8Retries > 0));

    if (error == SDMMC_ERROR_NORESPONSE) {
        /* No response : Ver2.00 or later SD Memory Card(voltage mismatch)
         * or Ver1.X SD Memory Card
         * or not SD Memory Card */

        TRACE_DEBUG("No Resp Cmd8\n\r");
        Delay(800);

        /* ACMD41 is a synchronization command used to negotiate the operation
         * voltage range and to poll the cards until they are out of their
         * power-up sequence. */
        error = Acmd41(pSd, 0, &isCCSet);
        if (error)
        {
            TRACE_DEBUG("Try MMC Init\n\r");
            /* Acmd41 failed : MMC card or unknown card */
            error = SwReset(pSd, 10);
            if (error) {
                TRACE_ERROR("SdMmcInit.2 (%d)\n\r", error);
                return error;
            }
            do {
                error = Cmd1(pSd, &isHdSupport);
            }
            while ((error) && (cmd1Retries-- > 0));
            if (error) {
                TRACE_ERROR("SdMmcInit.3 (%d)\n\r", error);
                return error;
            }
            else {
                pSd->cardType = CARD_MMC;
                if(isHdSupport) {
                    pSd->cardType = CARD_MMCHD;
                    TRACE_DEBUG("CARD High Density MMC\n\r");
                }
                else {
                    TRACE_DEBUG("CARD MMC\n\r");
                }
            }
        }
        else {
            TRACE_DEBUG("SD Card\n\r");
            if(isCCSet == 0) {
                TRACE_DEBUG("CARD SD\n\r");
                pSd->cardType = CARD_SD;
            }
        }
    }
    else if (!error) {

        /* Valid response : Ver2.00 or later SD Memory Card */
        error = Acmd41(pSd, 1, &isCCSet);
        if (error) {
            TRACE_ERROR("SdMmcInit.4 (%d)\n\r", error);
            return error;
        }
        if (isCCSet) {
            TRACE_DEBUG("CARD SDHC\n\r");
            pSd->cardType = CARD_SDHC;
        }
        else {
            TRACE_DEBUG("CARD SD\n\r");
            pSd->cardType = CARD_SD;
        }
    }
    else {
        TRACE_ERROR("SdMmcInit.5 (%d)\n\r", error);
        return error;
    }

    /* The host then issues the command ALL_SEND_CID (CMD2) to the card to get
     * its unique card identification (CID) number.
     * Card that is unidentified (i.e. which is in Ready State) sends its CID
     * number as the response (on the CMD line). */
    error = Cmd2(pSd);
    if (error) {
        TRACE_ERROR("SdMmcInit.cmd2(%d)\n\r", error);
        return error;
    }
    
    /* Thereafter, the host issues CMD3 (SEND_RELATIVE_ADDR) asks the
     * card to publish a new relative card address (RCA), which is shorter than
     * CID and which is used to address the card in the future data transfer
     * mode. Once the RCA is received the card state changes to the Stand-by
     * State. At this point, if the host wants to assign another RCA number, it
     * can ask the card to publish a new number by sending another CMD3 command
     * to the card. The last published RCA is the actual RCA number of the
     * card. */
    error = Cmd3(pSd);
    if (error) {
        TRACE_ERROR("SdMmcInit.cmd3(%d)\n\r", error);
        return error;
    }

    /* SEND_CSD (CMD9) to obtain the Card Specific Data (CSD register),
     * e.g. block length, card storage capacity, etc... */
    error = Cmd9(pSd);
    if (error) {
        TRACE_ERROR("SdMmcInit.cmd9(%d)\n\r", error);
        return error;
    }

    /* Now select the card, to TRAN state */
    error = MmcSelectCard(pSd, pSd->cardAddress);
    if (error) {
        TRACE_ERROR("SdMmcInit.SelCard(%d)\n\r", error);
        return error;
    }

    /* Now in TRAN, reset bus width to 1bit and obtain setup information */
    SdmmcSetBusWidth(pSd, SDMMC_BUS_1_BIT);

    /* If the card support EXT_CSD, read it! */
    TRACE_INFO("Card Type %d, CSD_STRUCTURE %d\n\r",
               pSd->cardType, SD_CSD_STRUCTURE(pSd));

    /* Get extended information of the card */
    SdMmcUpdateInformation(pSd, 0, 1);

    /* Advanced settings for HD & HS card */
    if (pSd->cardType >= CARD_MMC){

        if (SD_CSD_STRUCTURE(pSd) >= 2) {

            MmcCmd6Arg cmd6Arg;
            uint8_t busWidth = 1, widthMode = SDMMC_BUS_4_BIT;

            /* Switch to max bus width (4 now) */
            cmd6Arg.access = 0x3;
            cmd6Arg.index  = SD_EXTCSD_BUS_WIDTH_INDEX;
            cmd6Arg.value  = SD_EXTCSD_BUS_WIDTH_4BIT;
            cmd6Arg.cmdSet = 0;
            error = MmcCmd6(pSd, &cmd6Arg, 0, NULL);
            if (!error) {
                TRACE_WARNING_WP("-I- MMC %d-BIT BUS\n\r", busWidth);
                SdmmcSetBusWidth(pSd, widthMode);
                updateInformation = 1;
            }
            else {
                TRACE_WARNING("MMC %d-BIT not supported\n\r", busWidth)
            }

            // TODO: Needs test
            /* Switch to HS mode */
            if (SdmmcGetProperty(pSd, SDMMC_PROP_HS_MODE))
            {
                cmd6Arg.access = 0x3;
                cmd6Arg.index  = SD_EXTCSD_HS_TIMING_INDEX;
                cmd6Arg.value  = SD_EXTCSD_HS_TIMING_ENABLE;
                cmd6Arg.cmdSet = 0;
                error = MmcCmd6(pSd, &cmd6Arg, &status, NULL);
                if (error || (status & STATUS_SWITCH_ERROR)) {
                    TRACE_WARNING("MMC HS Fail\n\r");
                }
                else {
                    SdmmcEnableHsMode(pSd, 1);
                    TRACE_WARNING_WP("-I- MMC HS Enabled\n\r");
                    isHsSupport = 1;
                    updateInformation = 1;
                }
            }
        }
    }
    else if (pSd->cardType >= CARD_SD) {

        /* Switch to 4-bits bus width
         * (All SD Card shall support 1-bit, 4-bits width) */
        if (SdmmcGetProperty(pSd, SDMMC_PROP_BUS_MODE) >= SDMMC_BUS_4_BIT) {
            error = Acmd6(pSd, SDMMC_BUS_4_BIT);
            TRACE_WARNING_WP("-I- SD 4-BITS BUS\n\r");
            if (error) {
                TRACE_ERROR("SdMmcInit.Acmd6(%d)\n\r", error);
                return error;
            }
            SdmmcSetBusWidth(pSd, SDMMC_BUS_4_BIT);
        }

        /* SD Spec V1.10 or higher, switch to high-speed mode */
        if (SdmmcGetProperty(pSd, SDMMC_PROP_HS_MODE)) 
        {
            if (SD_SCR_SD_SPEC(pSd) >= SD_SCR_SD_SPEC_1_10) {
                SdCmd6Arg cmd6Arg;
                uint32_t switchStatus[512/32];
                TRACE_WARNING_WP("-I- SD HighSpeed\n\r")
                cmd6Arg.mode = 1;
                cmd6Arg.reserved = 0;
                cmd6Arg.reserveFG6 = 0xF;
                cmd6Arg.reserveFG5 = 0xF;
                cmd6Arg.reserveFG4 = 0xF;
                cmd6Arg.reserveFG3 = 0xF;
                cmd6Arg.command = 0;
                cmd6Arg.accessMode = 1;
                error = SdCmd6(pSd,
                               &cmd6Arg,
                               switchStatus,
                               &status,
                               NULL);
                #if 0
                {
                uint32_t i;
                printf("SD Switch status:");
                for(i = 0; i < 512 / 8; i ++) {
                    if ((i % 8) == 0) printf("\n\r[%3d]", i);
                    printf(" %02x", ((char*)switchStatus)[i]);
                }
                printf("\n\r");
                printf(" _FG1_INFO %x\n\r",
                    SD_SW_STAT_FUN_GRP1_INFO(switchStatus));
                printf(" _FG1_RC   %x\n\r",
                    SD_SW_STAT_FUN_GRP1_RC(switchStatus));
                printf(" _FG1_BUSY %x\n\r",
                    SD_SW_STAT_FUN_GRP1_BUSY(switchStatus));
                printf(" _FG1_DS_V %x\n\r",
                    SD_SW_STAT_DATA_STRUCT_VER(switchStatus));
                }
                #endif
                if (error || (status & STATUS_SWITCH_ERROR)) {
                    TRACE_WARNING("SD HS Fail\n\r");
                }
                else if (SD_SW_STAT_FUN_GRP1_RC(switchStatus)
                                == SD_SW_STAT_FUN_GRP_RC_ERROR) {
                    TRACE_ERROR_WP("-I- SD HS Not Supported\n\r");
                }
                else if (SD_SW_STAT_FUN_GRP1_BUSY(switchStatus)) {
                    TRACE_WARNING("SD HS Busy\n\r")
                }
                else {
                    SdmmcEnableHsMode(pSd, 1);
                    TRACE_WARNING_WP("-I- SD HS Enable\n\r");
                    isHsSupport = 1;
                }
            }
        }

        // Update
        updateInformation = 1;
    }

    if (updateInformation) {

        SdMmcUpdateInformation(pSd, isHsSupport, 1);
    }
    return 0;
}

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

/**
 * Read Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 * \param length   Number of blocks to be read.
 * \param pCallback Pointer to callback function that invoked when read done.
 *                  0 to start a blocked read.
 * \param pArgs     Pointer to callback function arguments.
 */
uint8_t SD_Read(SdCard        *pSd,
                uint32_t      address,
                void          *pData,
                uint16_t      length,
                SdmmcCallback pCallback,
                void          *pArgs)
{
    uint8_t error;

    SANITY_CHECK(pSd);
    SANITY_CHECK(pData);

    if (   pSd->state != SD_STATE_READ
        || pSd->preBlock + 1 != address ) {
        /* Start infinite block reading */
        error = MoveToTransferState(pSd, address, 0, 0, 1);
    }
    else    error = 0;
    if (!error) {
        pSd->state = SD_STATE_READ;
        pSd->preBlock = address + (length - 1);
        error = SdmmcRead(pSd, BLOCK_SIZE(pSd), length, pData,
                          pCallback, pArgs);
    }
    TRACE_DEBUG("SDrd(%u,%u):%u\n\r", address, length, error);

    return 0;    
}

/**
 * Write Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 * \param length   Number of blocks to be read.
 * \param pCallback Pointer to callback function that invoked when read done.
 *                  0 to start a blocked read.
 * \param pArgs     Pointer to callback function arguments.
 */
uint8_t SD_Write(SdCard        *pSd,
                 uint32_t      address,
                 void          *pData,
                 uint16_t      length,
                 SdmmcCallback pCallback,
                 void          *pArgs)
{
    uint8_t error = 0;

    SANITY_CHECK(pSd);

    if (   pSd->state != SD_STATE_WRITE
        || pSd->preBlock + 1 != address ) {
        /* Start infinite block writing */
        error = MoveToTransferState(pSd, address, 0, 0, 0);
    }
    if (!error) {
        pSd->state = SD_STATE_WRITE;
        error = SdmmcWrite(pSd, BLOCK_SIZE(pSd), length, pData,
                           pCallback, pArgs);
        pSd->preBlock = address + (length - 1);
    }
    TRACE_DEBUG("SDwr(%u,%u):%u\n\r", address, length, error);

    return 0;
}

/**
 * Read 1 Block of data in a buffer pointed by pData. The buffer size must be
 * one block size. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size.
 */
uint8_t SD_ReadBlock(SdCard *pSd,
                     uint32_t address,
                     uint8_t *pData)
{
    SANITY_CHECK(pSd);
    SANITY_CHECK(pData);

    TRACE_DEBUG("ReadBlk(%d,%d)\n\r", address);

    return PerformSingleTransfer(pSd, address, pData, 1);
}

/**
 * Write 1 Block of data pointed by pData. The buffer size must be
 * one block size. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an SD_ERROR code.
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of block to write.
 * \param pData    Data buffer whose size is at least the block size, it can
 *                 be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_WriteBlock(SdCard *pSd,
                      uint32_t address,
                      uint8_t *pData)
{
    SANITY_CHECK(pSd);
    SANITY_CHECK(pData);

    TRACE_DEBUG("WriteBlk(%d)\n\r", address);

    return PerformSingleTransfer(pSd, address, pData, 0);
}

/**
 * Read Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param nbBlocks Number of blocks to be read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_ReadBlocks(SdCard *pSd,
                      uint32_t address,
                      uint16_t nbBlocks,
                      uint8_t *pData)
{
    uint8_t error = 0;

    SANITY_CHECK(pSd);
    SANITY_CHECK(pData);
    SANITY_CHECK(nbBlocks);

    TRACE_DEBUG("ReadBlk(%d,%d)\n\r", address, nbBlocks);
    while(nbBlocks --) {
        error = PerformSingleTransfer(pSd, address, pData, 1);
        if (error)
            break;
        address += 1;
        pData = &pData[512];
    }
    return error;
}

/**
 * Write Block of data pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * Returns 0 if successful; otherwise returns an SD_ERROR code.
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of block to write.
 * \param nbBlocks Number of blocks to be read
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_WriteBlocks(SdCard *pSd,
                       uint32_t address,
                       uint16_t nbBlocks,
                       uint8_t *pData)
{
    uint8_t error = 0;
    uint8_t *pB = (uint8_t*)pData;

    SANITY_CHECK(pSd);
    SANITY_CHECK(pData);
    SANITY_CHECK(nbBlocks);

    TRACE_DEBUG("WriteBlk(%d,%d)\n\r", address, nbBlocks);

    while(nbBlocks --) {
        error = PerformSingleTransfer(pSd, address, pB, 0);
        if (error)
            break;
        address += 1;
        pB = &pB[512];
    }
    return error;
}

/**
 * Run the SDcard initialization sequence. This function runs the
 * initialisation procedure and the identification process, then it sets the
 * SD card in transfer state to set the block length and the bus width.
 * Returns 0 if successful; otherwise returns an SD_ERROR code.
 * \param pSd  Pointer to a SD card driver instance.
 * \param pSdDriver  Pointer to SD driver already initialized.
 */
uint8_t SD_Init(SdCard *pSd, void *pSdDriver)
{
    uint8_t  error;
    uint32_t clock;

    /* Initialize SdCard structure */
    pSd->pSdDriver = pSdDriver;
    pSd->cardAddress = 0;
    pSd->preBlock = 0xffffffff;
    pSd->state = SD_STATE_INIT;
    pSd->cardType = UNKNOWN_CARD;
    pSd->optCmdBitMap = 0xFFFFFFFF;
    pSd->mode = 0;

    /* Initialization delay: The maximum of 1 msec, 74 clock cycles and supply
     * ramp up time. Supply ramp up time provides the time that the power is
     * built up to the operating level (the bus master supply voltage) and the
     * time to wait until the SD card can accept the first command. */
    /* Power On Init Special Command */
    SdmmcSetSpeed(pSd, 400000);
    error = Pon(pSd);
    if (error) {
        TRACE_ERROR("SD_Init.PowON:%d\n\r", error);
        return error;
    }

    /* After power-on or CMD0, all cards?
     * CMD lines are in input mode, waiting for start bit of the next command.
     * The cards are initialized with a default relative card address
     * (RCA=0x0000) and with a default driver stage register setting
     * (lowest speed, highest driving current capability). */
    error = SdMmcInit(pSd, pSdDriver);
    if (error) {
        TRACE_ERROR("SD_Init.2 (%d)\n\r", error);
        return error;
    }

    /* In the case of a Standard Capacity SD Memory Card, this command sets the
     * block length (in bytes) for all following block commands
     * (read, write, lock).
     * Default block length is fixed to 512 Bytes.
     * Set length is valid for memory access commands only if partial block read
     * operation are allowed in CSD.
     * In the case of a High Capacity SD Memory Card, block length set by CMD16
     * command does not affect the memory read and write commands. Always 512
     * Bytes fixed block length is used. This command is effective for
     * LOCK_UNLOCK command.
     * In both cases, if block length is set larger than 512Bytes, the card sets
     * the BLOCK_LEN_ERROR bit. */
    if (pSd->cardType == CARD_SD) {
        error = Cmd16(pSd, SDMMC_BLOCK_SIZE);
        if (error) {
            pSd->optCmdBitMap &= ~SD_CMD16_SUPPORT;
            TRACE_INFO("SD_Init.Cmd16 (%d)\n\r", error);
            TRACE_INFO("Fail to set BLK_LEN, default is 512\n\r");
        }
    }

    /* Reset status for R/W */
    pSd->state = SD_STATE_READY;

    /* If MMC Card & get size from EXT_CSD */
    if (pSd->cardType >= CARD_MMC && SD_CSD_C_SIZE(pSd) == 0xFFF) {
        pSd->blockNr = SD_EXTCSD_BLOCKNR(pSd);
        // Block number less than 0x100000000/512
        if (pSd->blockNr > 0x800000)
            pSd->totalSize = 0xFFFFFFFF;
        else
            pSd->totalSize = SD_EXTCSD_TOTAL_SIZE(pSd);
    }
    /* If SD CSD v2.0 */
    else if(pSd->cardType >= CARD_SD && SD_CSD_STRUCTURE(pSd) >= 1) {
        pSd->blockNr   = SD_CSD_BLOCKNR_HC(pSd);
        pSd->totalSize = 0xFFFFFFFF;
    }
    /* Normal SD/MMC card */
    else {
        pSd->totalSize = SD_CSD_TOTAL_SIZE(pSd);
        pSd->blockNr = SD_CSD_BLOCKNR(pSd);
    }

    if (pSd->cardType == UNKNOWN_CARD) {
        return SDMMC_ERROR_NOT_INITIALIZED;
    }
    /* Automatically select the max clock */
    clock = SdmmcSetSpeed(pSd, pSd->transSpeed);
    TRACE_WARNING_WP("-I- Set SD/MMC clock to %dK\n\r", clock/1000);
    return 0;
}

/**
 * Return size of the SD/MMC card, in KB.
 * \param pSd Pointer to SdCard instance.
 */
uint32_t SD_GetTotalSizeKB(SdCard *pSd)
{
    SANITY_CHECK(pSd);

    if (pSd->totalSize == 0xFFFFFFFF) {

        return pSd->blockNr / 2;
    }
    
    return pSd->totalSize / 1024;
}

/**
 * Display the content of the CID register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterCID(SdCard *pSd)
{ 
    TRACE_INFO("======= CID =======\n\r");
  #if 0
    TRACE_INFO(" .Card/BGA         %X\n\r", SD_CID_BGA(pSd));
  #else
    TRACE_INFO("CID MID Manufacturer ID       %02X\n\r",
        SD_CID_MID(pSd));
    
    TRACE_INFO("CID OID OEM/Application ID    %c%c\n\r",
        (char)SD_CID_OID_BYTE_1(pSd),
        (char)SD_CID_OID_BYTE_0(pSd));
    
    TRACE_INFO("CID PNM Product revision      %c%c%c%c%c\n\r",
        (char)SD_CID_PNM_BYTE_4(pSd),
        (char)SD_CID_PNM_BYTE_3(pSd),
        (char)SD_CID_PNM_BYTE_2(pSd),
        (char)SD_CID_PNM_BYTE_1(pSd),
        (char)SD_CID_PNM_BYTE_0(pSd));
    
    TRACE_INFO("CID PRV Product serial number %02X%04X\n\r", 
         SD_CID_PRV_2(pSd),
         SD_CID_PRV_1(pSd));

    TRACE_INFO("CID MDT Manufacturing date    %04d/%02d\n\r",
        (uint16_t)SD_CID_MDT_YEAR(pSd),
        (uint8_t)SD_CID_MDT_MONTH(pSd));               
    
    TRACE_INFO("CID CRC checksum              %02X\n\r",   
         SD_CID_CRC(pSd));
  #endif
}

/**
 * Display the content of the CSD register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterCSD(SdCard *pSd)
{ 
    TRACE_INFO("======== CSD ========");
  #if 0
  {
    uint32_t i;
    uint8_t *p = (uint8_t *)pSd->csd;
    for(i = 0; i < 128 / 8; i++) {
        if ((i % 16) == 0) TRACE_INFO_WP("\n\r [%3d]:", i);
        TRACE_INFO_WP(" %2x", p[i]);
    }
    TRACE_INFO_WP("\n\r");
    TRACE_INFO("------------------------\n\r");
  }
  #else
    TRACE_INFO_WP("\n\r");
  #endif
    TRACE_INFO(" .CSD_STRUCTURE      0x%x\r\n", SD_CSD_STRUCTURE(pSd));
    TRACE_INFO(" .SPEC_VERS          0x%x\r\n", SD_CSD_SPEC_VERS(pSd));
    TRACE_INFO(" .TAAC               0x%X\r\n", SD_CSD_TAAC(pSd)              );
    TRACE_INFO(" .NSAC               0x%X\r\n", SD_CSD_NSAC(pSd)              );
    TRACE_INFO(" .TRAN_SPEED         0x%X\r\n", SD_CSD_TRAN_SPEED(pSd)        );
    TRACE_INFO(" .CCC                0x%X\r\n", SD_CSD_CCC(pSd)               );
    TRACE_INFO(" .READ_BL_LEN        0x%X\r\n", SD_CSD_READ_BL_LEN(pSd)       );
    TRACE_INFO(" .READ_BL_PARTIAL    0x%X\r\n", SD_CSD_READ_BL_PARTIAL(pSd)   );
    TRACE_INFO(" .WRITE_BLK_MISALIGN 0x%X\r\n", SD_CSD_WRITE_BLK_MISALIGN(pSd));
    TRACE_INFO(" .READ_BLK_MISALIGN  0x%X\r\n", SD_CSD_READ_BLK_MISALIGN(pSd) );
    TRACE_INFO(" .DSR_IMP            0x%X\r\n", SD_CSD_DSR_IMP(pSd)           );
    TRACE_INFO(" .C_SIZE             0x%X\r\n", SD_CSD_C_SIZE(pSd)            );
    TRACE_INFO(" .C_SIZE_HC          0x%X\r\n", SD_CSD_C_SIZE_HC(pSd)         );
    TRACE_INFO(" .VDD_R_CURR_MIN     0x%X\r\n", SD_CSD_VDD_R_CURR_MIN(pSd)    );
    TRACE_INFO(" .VDD_R_CURR_MAX     0x%X\r\n", SD_CSD_VDD_R_CURR_MAX(pSd)    );
    TRACE_INFO(" .VDD_W_CURR_MIN     0x%X\r\n", SD_CSD_VDD_W_CURR_MIN(pSd)    );
    TRACE_INFO(" .VDD_W_CURR_MAX     0x%X\r\n", SD_CSD_VDD_W_CURR_MAX(pSd)    );
    TRACE_INFO(" .C_SIZE_MULT        0x%X\r\n", SD_CSD_C_SIZE_MULT(pSd)       );
    TRACE_INFO(" .ERASE_BLK_EN       0x%X\r\n", SD_CSD_ERASE_BLK_EN(pSd)      );
    TRACE_INFO(" .SECTOR_SIZE        0x%X\r\n", SD_CSD_SECTOR_SIZE(pSd)       );
    TRACE_INFO(" .WP_GRP_SIZE        0x%X\r\n", SD_CSD_WP_GRP_SIZE(pSd)       );
    TRACE_INFO(" .WP_GRP_ENABLE      0x%X\r\n", SD_CSD_WP_GRP_ENABLE(pSd)     );
    TRACE_INFO(" .R2W_FACTOR         0x%X\r\n", SD_CSD_R2W_FACTOR(pSd)        );
    TRACE_INFO(" .WRITE_BL_LEN       0x%X\r\n", SD_CSD_WRITE_BL_LEN(pSd)      );
    TRACE_INFO(" .WRITE_BL_PARTIAL   0x%X\r\n", SD_CSD_WRITE_BL_PARTIAL(pSd)  );
    TRACE_INFO(" .FILE_FORMAT_GRP    0x%X\r\n", SD_CSD_FILE_FORMAT_GRP(pSd)   );
    TRACE_INFO(" .COPY               0x%X\r\n", SD_CSD_COPY(pSd)              );
    TRACE_INFO(" .PERM_WRITE_PROTECT 0x%X\r\n", SD_CSD_PERM_WRITE_PROTECT(pSd));
    TRACE_INFO(" .TMP_WRITE_PROTECT  0x%X\r\n", SD_CSD_TMP_WRITE_PROTECT(pSd) );
    TRACE_INFO(" .FILE_FORMAT        0x%X\r\n", SD_CSD_FILE_FORMAT(pSd)       );
    TRACE_INFO(" .ECC                0x%X\r\n", SD_CSD_ECC(pSd)               );
    TRACE_INFO(" .CRC                0x%X\r\n", SD_CSD_CRC(pSd)               );
    TRACE_INFO(" .MULT               0x%X\r\n", SD_CSD_MULT(pSd)              );
    TRACE_INFO(" .BLOCKNR            0x%X\r\n", SD_CSD_BLOCKNR(pSd)           );
    TRACE_INFO(" .BLOCKNR_HC         0x%X\r\n", SD_CSD_BLOCKNR_HC(pSd)        );
    TRACE_INFO(" .BLOCK_LEN          0x%X\r\n", SD_CSD_BLOCK_LEN(pSd)         );
    TRACE_INFO(" .TOTAL_SIZE         0x%X\r\n", SD_CSD_TOTAL_SIZE(pSd)        );
    TRACE_INFO(" .TOTAL_SIZE_HC      0x%X\r\n", SD_CSD_TOTAL_SIZE_HC(pSd)     );
    TRACE_INFO(" -SD_TOTAL_SIZE      0x%X\r\n", SD_TOTAL_SIZE(pSd)            );
    TRACE_INFO(" -SD_TOTAL_BLOCK     0x%X\r\n", SD_TOTAL_BLOCK(pSd)           );
}   

/**
 * Display the content of the EXT_CSD register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterECSD(SdCard *pSd)
{
    if (pSd->cardType >= CARD_MMC && SD_CSD_STRUCTURE(pSd) >= 2) {}
    else {
        TRACE_INFO("** EXT_CSD NOT SUPPORTED\n\r");
        return;
    }
    TRACE_INFO("======= EXT_CSD =======");
  #if 0
  {
    uint32_t i;
    uint8_t *p = (uint8_t *)pSd->extData;
    for(i = 0; i < 512; i++) {
        if ((i % 16) == 0) TRACE_INFO_WP("\n\r [%3d]:", i);
        TRACE_INFO_WP(" %2x", p[i]);
    }
    TRACE_INFO_WP("\n\r");
    TRACE_INFO("------------------------\n\r");
  }
  #else
    TRACE_INFO_WP("\n\r");
  #endif
    TRACE_INFO(" .S_CMD_SET            : 0x%X\n\r",
        SD_EXTCSD_S_CMD_SET(pSd));
    TRACE_INFO(" .BOOT_INFO            : 0x%X\n\r",
        SD_EXTCSD_BOOT_INFO(pSd));
    TRACE_INFO(" .BOOT_SIZE_MULTI      : 0x%X\n\r",
        SD_EXTCSD_BOOT_SIZE_MULTI(pSd));
    TRACE_INFO(" .ACC_SIZE             : 0x%X\n\r",
        SD_EXTCSD_ACC_SIZE(pSd));
    TRACE_INFO(" .HC_ERASE_GRP_SIZE    : 0x%X\n\r",
        SD_EXTCSD_HC_ERASE_GRP_SIZE(pSd));
    TRACE_INFO(" .ERASE_TIMEOUT_MULT   : 0x%X\n\r",
        SD_EXTCSD_ERASE_TIMEOUT_MULT(pSd));
    TRACE_INFO(" .REL_WR_SEC_C         : 0x%X\n\r",
        SD_EXTCSD_REL_WR_SEC_C(pSd));
    TRACE_INFO(" .HC_WP_GRP_SIZE       : 0x%X\n\r",
        SD_EXTCSD_HC_WP_GRP_SIZE(pSd));
    TRACE_INFO(" .S_C_VCC              : 0x%X\n\r",
        SD_EXTCSD_S_C_VCC(pSd));
    TRACE_INFO(" .S_C_VCCQ             : 0x%X\n\r",
        SD_EXTCSD_S_C_VCCQ(pSd));
    TRACE_INFO(" .S_A_TIMEOUT          : 0x%X\n\r",
        SD_EXTCSD_S_A_TIMEOUT(pSd));
    TRACE_INFO(" .SEC_COUNT            : 0x%X\n\r",
        SD_EXTCSD_SEC_COUNT(pSd));
    TRACE_INFO(" .MIN_PERF_W_8_52      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_8_52(pSd));
    TRACE_INFO(" .MIN_PERF_R_8_52      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_8_52(pSd));
    TRACE_INFO(" .MIN_PERF_W_8_26_4_52 : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_8_26_4_52(pSd));
    TRACE_INFO(" .MIN_PERF_R_8_26_4_52 : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_8_26_4_52(pSd));
    TRACE_INFO(" .MIN_PERF_W_4_26      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_4_26(pSd));
    TRACE_INFO(" .MIN_PERF_R_4_26      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_4_26(pSd));
    TRACE_INFO(" .PWR_CL_26_360        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_26_360(pSd));
    TRACE_INFO(" .PWR_CL_52_360        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_52_360(pSd));
    TRACE_INFO(" .PWR_CL_26_195        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_26_195(pSd));
    TRACE_INFO(" .PWR_CL_52_195        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_52_195(pSd));
    TRACE_INFO(" .CARD_TYPE            : 0x%X\n\r",
        SD_EXTCSD_CARD_TYPE(pSd));
    TRACE_INFO(" .CSD_STRUCTURE        : 0x%X\n\r",
        SD_EXTCSD_CSD_STRUCTURE(pSd));
    TRACE_INFO(" .EXT_CSD_REV          : 0x%X\n\r",
        SD_EXTCSD_EXT_CSD_REV(pSd));
    TRACE_INFO(" .CMD_SET              : 0x%X\n\r",
        SD_EXTCSD_CMD_SET(pSd));
    TRACE_INFO(" .CMD_SET_REV          : 0x%X\n\r",
        SD_EXTCSD_CMD_SET_REV(pSd));
    TRACE_INFO(" .POWER_CLASS          : 0x%X\n\r",
        SD_EXTCSD_POWER_CLASS(pSd));
    TRACE_INFO(" .HS_TIMING            : 0x%X\n\r",
        SD_EXTCSD_HS_TIMING(pSd));
    TRACE_INFO(" .BUS_WIDTH            : 0x%X\n\r",
        SD_EXTCSD_BUS_WIDTH(pSd));
    TRACE_INFO(" .ERASED_MEM_CONT      : 0x%X\n\r",
        SD_EXTCSD_ERASED_MEM_CONT(pSd));
    TRACE_INFO(" .BOOT_CONFIG          : 0x%X\n\r",
        SD_EXTCSD_BOOT_CONFIG(pSd));
    TRACE_INFO(" .BOOT_BUS_WIDTH       : 0x%X\n\r",
        SD_EXTCSD_BOOT_BUS_WIDTH(pSd));
    TRACE_INFO(" .ERASE_GROUP_DEF      : 0x%X\n\r",
        SD_EXTCSD_ERASE_GROUP_DEF(pSd));
}

/**
 * Display the content of the SCR register
 * \param pSd  Pointer to SdCard instance.
 */
void SD_DisplayRegisterSCR(SdCard *pSd)
{ 
    if (pSd->cardType >= CARD_SD && pSd->cardType <= CARD_SDHC) {}
    else {
        TRACE_INFO("** SCR NOT Supported!\n\r");
        return;
    }
    TRACE_INFO("========== SCR ==========");
  #if 0
  {
    uint32_t i;
    uint8_t *p = (uint8_t*)pSd->extData;
    //TRACE_INFO_WP("\n\r");
    //TRACE_INFO("DATA @ 0x%X", (uint32_t)p);
    for(i = 0; i < 16; i ++) {
        if ((i % 8) == 0) TRACE_INFO_WP("\n\r [%3d]:", i);
        TRACE_INFO_WP(" %02x", p[i]);
    }
    TRACE_INFO_WP("\n\r");
    TRACE_INFO("------------------------\n\r");
  }
  #else
    TRACE_INFO_WP("\n\r");
  #endif

    TRACE_INFO(" .SCR_STRUCTURE         :0x%X\n\r",
        SD_SCR_SCR_STRUCTURE(pSd));
    TRACE_INFO(" .SD_SPEC               :0x%X\n\r",
        SD_SCR_SD_SPEC(pSd));
    TRACE_INFO(" .DATA_STAT_AFTER_ERASE :0x%X\n\r",
        SD_SCR_DATA_STAT_AFTER_ERASE(pSd));
    TRACE_INFO(" .SD_SECURITY           :0x%X\n\r",
        SD_SCR_SD_SECURITY(pSd));
    TRACE_INFO(" .SD_BUS_WIDTHS         :0x%X\n\r",
        SD_SCR_SD_BUS_WIDTHS(pSd));
}

/**
 * Display the content of the SD Status
 * \param pSd  Pointer to SdCard instance.
 */
void SD_DisplaySdStatus(SdCard *pSd)
{
    if (   pSd->cardType >= CARD_SD
        && pSd->cardType <= CARD_SDHC
        && (pSd->optCmdBitMap & SD_ACMD13_SUPPORT) ) {}
    else {
        TRACE_INFO("** SD Status NOT Supported!\n\r");
        return;
    }
    TRACE_INFO("=========== STAT ============");
  #if 0
  {
    uint32_t i;
    uint8_t *p = (uint8_t*)pSd->extData;
    //TRACE_INFO_WP("\n\r");
    //TRACE_INFO("DATA @ 0x%X", (uint32_t)p);
    for(i = 0; i < 72; i ++) {
        if ((i % 8) == 0) TRACE_INFO_WP("\n\r [%3d]:", i);
        TRACE_INFO_WP(" %02x", p[i]);
    }
    TRACE_INFO_WP("\n\r");
    TRACE_INFO("------------------------\n\r");
  }
  #else
    TRACE_INFO_WP("\n\r");
  #endif

    TRACE_INFO(" .DAT_BUS_WIDTH          :0x%X\n\r",
        SD_STAT_DAT_BUS_WIDTH(pSd));
    TRACE_INFO(" .SECURED_MODE           :0x%X\n\r",
        SD_STAT_SECURED_MODE(pSd));
    TRACE_INFO(" .SD_CARD_TYPE           :0x%X\n\r",
        SD_STAT_SD_CARD_TYPE(pSd));
    TRACE_INFO(" .SIZE_OF_PROTECTED_AREA :0x%X\n\r",
        SD_STAT_SIZE_OF_PROTECTED_AREA(pSd));
    TRACE_INFO(" .SPEED_CLASS            :0x%X\n\r",
        SD_STAT_SPEED_CLASS(pSd));
    TRACE_INFO(" .PERFORMANCE_MOVE       :0x%X\n\r",
        SD_STAT_PERFORMANCE_MOVE(pSd));
    TRACE_INFO(" .AU_SIZE                :0x%X\n\r",
        SD_STAT_AU_SIZE(pSd));
    TRACE_INFO(" .ERASE_SIZE             :0x%X\n\r",
        SD_STAT_ERASE_SIZE(pSd));
    TRACE_INFO(" .ERASE_TIMEOUT          :0x%X\n\r",
        SD_STAT_ERASE_TIMEOUT(pSd));
    TRACE_INFO(" .ERASE_OFFSET           :0x%X\n\r",
        SD_STAT_ERASE_OFFSET(pSd));
}
/**@}*/
