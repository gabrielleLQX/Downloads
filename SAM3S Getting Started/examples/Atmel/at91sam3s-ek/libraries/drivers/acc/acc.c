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

/** \addtogroup acc_module Working with ACC
 * The ACC driver provides the interface to configure and use the ACC peripheral.\n
 *
 * It applies comparison on two inputs and gives a compare output. 
 *
 * To Enable a ACC Comparison,the user has to follow these few steps:
 * <ul>
 * <li> Enable ACC peripheral clock by setting the corresponding bit in PMC_PCER1
 *      (PMC Peripheral Clock Enable Register 1)
 * </li>
 * <li> Reset the controller by asserting ACC_CR_SWRST in ACC_CR(ACC Control Register)
 </li>
 * <li> Configure the mode as following steps:  </li>
 * -#   Select inputs for SELMINUS and SELPLUS in ACC_MR (ACC Mode Register).
 * -#   Enable Analog Comparator by setting ACEN in ACC_MR.
 * -#   Configure Edge Type to detect different compare output.
 * </li>
 * <li> Wait until the automatic mask period expires by polling MASK bit in
 *      ACC_ISR.  
 * </ul>
 *
 * For more accurate information, please look at the ACC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref acc.c\n
 * \ref acc.h\n
 */
/*@{*/
/*@}*/
/**
 * \file
 *
 * Implementation of Analog Comparator Controller (ACC).
 *
 */
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <board.h>
#include <acc/acc.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Initialize the ACC controller
 * \param pAcc Pointer to an Acc instance.
 * \param idAcc ACC identifier
 * \param selplus input connected to inp, 0~7
 * \param selminus input connected to inm,0~7
 * \param ac_en Analog comprator enabled/disabled
 * \param edge CF flag triggering mode
 * \param invert INVert comparator output,use pattern defined in the device header
 file
 */
void ACC_Configure(Acc *pAcc, uint8_t idAcc, uint8_t selplus, uint8_t selminus,
                uint16_t ac_en, uint16_t edge, uint16_t invert)
{

    /* Enable peripheral clock*/
    PMC->PMC_PCER1 = 1 << (idAcc - 32);

    /*  Reset the controller */
    pAcc->ACC_CR |= ACC_CR_SWRST;

    /*  Write to the MR register */
    ACC_CfgModeReg( pAcc,
                    ( (selplus<<4) & ACC_MR_SELPLUS)
                    | (( selminus) & ACC_MR_SELMINUS)
                    | ( ac_en & ACC_MR_ACEN)
                    | ( edge & ACC_MR_EDGETYP)
                    | ( invert & ACC_MR_INV) );
    /* set hysteresis and current option*/
    pAcc->ACC_ACR = (ACC_ACR_ISEL_HISP | ((0x01 << 1) & ACC_ACR_HYST));
    /* Automatic Output Masking Period*/
    while (pAcc->ACC_ISR & (uint32_t) ACC_ISR_MASK);
}

/**
 * Return the Channel Converted Data
 * \param pAcc Pointer to an Acc instance.
 * \param selplus input applied on ACC SELPLUS 
 * \param selminus input applied on ACC SELMINUS
 */
void ACC_SetComparisionPair(Acc *pAcc, uint8_t selplus, uint8_t selminus)
{

    uint32_t temp;

    ASSERT(selplus < 8 && selminus < 8,
                    "The assigned channel number is invalid!");

    temp = pAcc->ACC_MR;

    pAcc->ACC_MR = temp & (uint32_t) ((~ACC_MR_SELMINUS) & (~ACC_MR_SELPLUS));

    pAcc->ACC_MR |= (((selplus << 4) & ACC_MR_SELPLUS) | (selminus
                    & ACC_MR_SELMINUS));

}
/**
 * Return Comparison Result
 * \param pAcc Pointer to an Acc instance.
 * \param status value of ACC_ISR
 */
uint8_t ACC_GetComparisionResult(Acc *pAcc, uint32_t status)
{
    uint32_t temp = pAcc->ACC_MR;
    if ((temp & ACC_MR_INV) == ACC_MR_INV) {

        if (status & ACC_ISR_SCO) {
            return 0; /* inn>inp*/
        } else
            return 1;/* inp>inn*/

    } else {

        if (status & ACC_ISR_SCO) {
            return 1; /* inp>inn*/
        } else
            return 0;/* inn>inp*/
    }

}

