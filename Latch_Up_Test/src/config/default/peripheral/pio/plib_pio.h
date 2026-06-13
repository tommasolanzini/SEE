/*******************************************************************************
  PIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.h

  Summary:
    PIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (PIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_PIO_H
#define PLIB_PIO_H

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for LE_F3 pin ***/
#define LE_F3_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<13U))
#define LE_F3_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<13U))
#define LE_F3_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<13U))
#define LE_F3_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<13U))
#define LE_F3_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<13U))
#define LE_F3_Get()               ((PIOC_REGS->PIO_PDSR >> 13U) & 0x1U)
#define LE_F3_PIN                  PIO_PIN_PC13

/*** Macros for F3V3_F3 pin ***/
#define F3V3_F3_Set()               (PIOB_REGS->PIO_SODR = ((uint32_t)1U<<1U))
#define F3V3_F3_Clear()             (PIOB_REGS->PIO_CODR = ((uint32_t)1U<<1U))
#define F3V3_F3_Toggle()            (PIOB_REGS->PIO_ODSR ^= ((uint32_t)1U<<1U))
#define F3V3_F3_OutputEnable()      (PIOB_REGS->PIO_OER = ((uint32_t)1U<<1U))
#define F3V3_F3_InputEnable()       (PIOB_REGS->PIO_ODR = ((uint32_t)1U<<1U))
#define F3V3_F3_Get()               ((PIOB_REGS->PIO_PDSR >> 1U) & 0x1U)
#define F3V3_F3_PIN                  PIO_PIN_PB1

/*** Macros for SEL_F3 pin ***/
#define SEL_F3_Set()               (PIOB_REGS->PIO_SODR = ((uint32_t)1U<<0U))
#define SEL_F3_Clear()             (PIOB_REGS->PIO_CODR = ((uint32_t)1U<<0U))
#define SEL_F3_Toggle()            (PIOB_REGS->PIO_ODSR ^= ((uint32_t)1U<<0U))
#define SEL_F3_OutputEnable()      (PIOB_REGS->PIO_OER = ((uint32_t)1U<<0U))
#define SEL_F3_InputEnable()       (PIOB_REGS->PIO_ODR = ((uint32_t)1U<<0U))
#define SEL_F3_Get()               ((PIOB_REGS->PIO_PDSR >> 0U) & 0x1U)
#define SEL_F3_PIN                  PIO_PIN_PB0

/*** Macros for AMP_OUT3 pin ***/
#define AMP_OUT3_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<20U))
#define AMP_OUT3_Get()               ((PIOA_REGS->PIO_PDSR >> 20U) & 0x1U)
#define AMP_OUT3_PIN                  PIO_PIN_PA20

/*** Macros for AMP_OUT2 pin ***/
#define AMP_OUT2_Get()               ((PIOA_REGS->PIO_PDSR >> 19U) & 0x1U)
#define AMP_OUT2_PIN                  PIO_PIN_PA19

/*** Macros for AMP_OUT1 pin ***/
#define AMP_OUT1_Get()               ((PIOA_REGS->PIO_PDSR >> 18U) & 0x1U)
#define AMP_OUT1_PIN                  PIO_PIN_PA18

/*** Macros for LE_F2 pin ***/
#define LE_F2_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<30U))
#define LE_F2_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<30U))
#define LE_F2_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<30U))
#define LE_F2_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<30U))
#define LE_F2_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<30U))
#define LE_F2_Get()               ((PIOD_REGS->PIO_PDSR >> 30U) & 0x1U)
#define LE_F2_PIN                  PIO_PIN_PD30

/*** Macros for F3V3_F2 pin ***/
#define F3V3_F2_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<22U))
#define F3V3_F2_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<22U))
#define F3V3_F2_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<22U))
#define F3V3_F2_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<22U))
#define F3V3_F2_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<22U))
#define F3V3_F2_Get()               ((PIOA_REGS->PIO_PDSR >> 22U) & 0x1U)
#define F3V3_F2_PIN                  PIO_PIN_PA22

/*** Macros for F3_RB pin ***/
#define F3_RB_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<1U))
#define F3_RB_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<1U))
#define F3_RB_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<1U))
#define F3_RB_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<1U))
#define F3_RB_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<1U))
#define F3_RB_Get()               ((PIOC_REGS->PIO_PDSR >> 1U) & 0x1U)
#define F3_RB_PIN                  PIO_PIN_PC1

/*** Macros for F2_RB pin ***/
#define F2_RB_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<2U))
#define F2_RB_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<2U))
#define F2_RB_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<2U))
#define F2_RB_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<2U))
#define F2_RB_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<2U))
#define F2_RB_Get()               ((PIOC_REGS->PIO_PDSR >> 2U) & 0x1U)
#define F2_RB_PIN                  PIO_PIN_PC2

/*** Macros for F1_RB pin ***/
#define F1_RB_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<3U))
#define F1_RB_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<3U))
#define F1_RB_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<3U))
#define F1_RB_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<3U))
#define F1_RB_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<3U))
#define F1_RB_Get()               ((PIOC_REGS->PIO_PDSR >> 3U) & 0x1U)
#define F1_RB_PIN                  PIO_PIN_PC3

/*** Macros for SEL_F2 pin ***/
#define SEL_F2_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<16U))
#define SEL_F2_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<16U))
#define SEL_F2_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<16U))
#define SEL_F2_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<16U))
#define SEL_F2_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<16U))
#define SEL_F2_Get()               ((PIOA_REGS->PIO_PDSR >> 16U) & 0x1U)
#define SEL_F2_PIN                  PIO_PIN_PA16

/*** Macros for SEL_F1 pin ***/
#define SEL_F1_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<23U))
#define SEL_F1_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<23U))
#define SEL_F1_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<23U))
#define SEL_F1_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<23U))
#define SEL_F1_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<23U))
#define SEL_F1_Get()               ((PIOA_REGS->PIO_PDSR >> 23U) & 0x1U)
#define SEL_F1_PIN                  PIO_PIN_PA23

/*** Macros for LE_F1 pin ***/
#define LE_F1_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<27U))
#define LE_F1_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<27U))
#define LE_F1_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<27U))
#define LE_F1_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<27U))
#define LE_F1_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<27U))
#define LE_F1_Get()               ((PIOD_REGS->PIO_PDSR >> 27U) & 0x1U)
#define LE_F1_PIN                  PIO_PIN_PD27

/*** Macros for F3V3_F1 pin ***/
#define F3V3_F1_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<7U))
#define F3V3_F1_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<7U))
#define F3V3_F1_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<7U))
#define F3V3_F1_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<7U))
#define F3V3_F1_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<7U))
#define F3V3_F1_Get()               ((PIOC_REGS->PIO_PDSR >> 7U) & 0x1U)
#define F3V3_F1_PIN                  PIO_PIN_PC7

/*** Macros for NAND_D0 pin ***/
#define NAND_D0_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<25U))
#define NAND_D0_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<25U))
#define NAND_D0_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<25U))
#define NAND_D0_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<25U))
#define NAND_D0_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<25U))
#define NAND_D0_Get()               ((PIOD_REGS->PIO_PDSR >> 25U) & 0x1U)
#define NAND_D0_PIN                  PIO_PIN_PD25

/*** Macros for NAND_D1 pin ***/
#define NAND_D1_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<26U))
#define NAND_D1_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<26U))
#define NAND_D1_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<26U))
#define NAND_D1_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<26U))
#define NAND_D1_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<26U))
#define NAND_D1_Get()               ((PIOD_REGS->PIO_PDSR >> 26U) & 0x1U)
#define NAND_D1_PIN                  PIO_PIN_PD26

/*** Macros for NAND_D2 pin ***/
#define NAND_D2_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<6U))
#define NAND_D2_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<6U))
#define NAND_D2_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<6U))
#define NAND_D2_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<6U))
#define NAND_D2_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<6U))
#define NAND_D2_Get()               ((PIOC_REGS->PIO_PDSR >> 6U) & 0x1U)
#define NAND_D2_PIN                  PIO_PIN_PC6

/*** Macros for NAND_D3 pin ***/
#define NAND_D3_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<24U))
#define NAND_D3_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<24U))
#define NAND_D3_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<24U))
#define NAND_D3_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<24U))
#define NAND_D3_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<24U))
#define NAND_D3_Get()               ((PIOD_REGS->PIO_PDSR >> 24U) & 0x1U)
#define NAND_D3_PIN                  PIO_PIN_PD24

/*** Macros for NAND_D4 pin ***/
#define NAND_D4_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<24U))
#define NAND_D4_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<24U))
#define NAND_D4_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<24U))
#define NAND_D4_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<24U))
#define NAND_D4_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<24U))
#define NAND_D4_Get()               ((PIOA_REGS->PIO_PDSR >> 24U) & 0x1U)
#define NAND_D4_PIN                  PIO_PIN_PA24

/*** Macros for NAND_D5 pin ***/
#define NAND_D5_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<23U))
#define NAND_D5_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<23U))
#define NAND_D5_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<23U))
#define NAND_D5_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<23U))
#define NAND_D5_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<23U))
#define NAND_D5_Get()               ((PIOD_REGS->PIO_PDSR >> 23U) & 0x1U)
#define NAND_D5_PIN                  PIO_PIN_PD23

/*** Macros for NAND_D6 pin ***/
#define NAND_D6_Set()               (PIOC_REGS->PIO_SODR = ((uint32_t)1U<<5U))
#define NAND_D6_Clear()             (PIOC_REGS->PIO_CODR = ((uint32_t)1U<<5U))
#define NAND_D6_Toggle()            (PIOC_REGS->PIO_ODSR ^= ((uint32_t)1U<<5U))
#define NAND_D6_OutputEnable()      (PIOC_REGS->PIO_OER = ((uint32_t)1U<<5U))
#define NAND_D6_InputEnable()       (PIOC_REGS->PIO_ODR = ((uint32_t)1U<<5U))
#define NAND_D6_Get()               ((PIOC_REGS->PIO_PDSR >> 5U) & 0x1U)
#define NAND_D6_PIN                  PIO_PIN_PC5

/*** Macros for NAND_D7 pin ***/
#define NAND_D7_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<25U))
#define NAND_D7_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<25U))
#define NAND_D7_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<25U))
#define NAND_D7_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<25U))
#define NAND_D7_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<25U))
#define NAND_D7_Get()               ((PIOA_REGS->PIO_PDSR >> 25U) & 0x1U)
#define NAND_D7_PIN                  PIO_PIN_PA25

/*** Macros for CE_F3 pin ***/
#define CE_F3_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<21U))
#define CE_F3_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<21U))
#define CE_F3_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<21U))
#define CE_F3_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<21U))
#define CE_F3_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<21U))
#define CE_F3_Get()               ((PIOD_REGS->PIO_PDSR >> 21U) & 0x1U)
#define CE_F3_PIN                  PIO_PIN_PD21

/*** Macros for CE_F2 pin ***/
#define CE_F2_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<11U))
#define CE_F2_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<11U))
#define CE_F2_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<11U))
#define CE_F2_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<11U))
#define CE_F2_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<11U))
#define CE_F2_Get()               ((PIOA_REGS->PIO_PDSR >> 11U) & 0x1U)
#define CE_F2_PIN                  PIO_PIN_PA11

/*** Macros for CE_F1 pin ***/
#define CE_F1_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<20U))
#define CE_F1_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<20U))
#define CE_F1_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<20U))
#define CE_F1_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<20U))
#define CE_F1_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<20U))
#define CE_F1_Get()               ((PIOD_REGS->PIO_PDSR >> 20U) & 0x1U)
#define CE_F1_PIN                  PIO_PIN_PD20

/*** Macros for NAND_OE pin ***/
#define NAND_OE_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<10U))
#define NAND_OE_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<10U))
#define NAND_OE_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<10U))
#define NAND_OE_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<10U))
#define NAND_OE_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<10U))
#define NAND_OE_Get()               ((PIOA_REGS->PIO_PDSR >> 10U) & 0x1U)
#define NAND_OE_PIN                  PIO_PIN_PA10

/*** Macros for NAND_WE pin ***/
#define NAND_WE_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<19U))
#define NAND_WE_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<19U))
#define NAND_WE_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<19U))
#define NAND_WE_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<19U))
#define NAND_WE_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<19U))
#define NAND_WE_Get()               ((PIOD_REGS->PIO_PDSR >> 19U) & 0x1U)
#define NAND_WE_PIN                  PIO_PIN_PD19

/*** Macros for NAND_CLE pin ***/
#define NAND_CLE_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<12U))
#define NAND_CLE_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<12U))
#define NAND_CLE_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<12U))
#define NAND_CLE_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<12U))
#define NAND_CLE_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<12U))
#define NAND_CLE_Get()               ((PIOA_REGS->PIO_PDSR >> 12U) & 0x1U)
#define NAND_CLE_PIN                  PIO_PIN_PA12

/*** Macros for NAND_ALE pin ***/
#define NAND_ALE_Set()               (PIOD_REGS->PIO_SODR = ((uint32_t)1U<<18U))
#define NAND_ALE_Clear()             (PIOD_REGS->PIO_CODR = ((uint32_t)1U<<18U))
#define NAND_ALE_Toggle()            (PIOD_REGS->PIO_ODSR ^= ((uint32_t)1U<<18U))
#define NAND_ALE_OutputEnable()      (PIOD_REGS->PIO_OER = ((uint32_t)1U<<18U))
#define NAND_ALE_InputEnable()       (PIOD_REGS->PIO_ODR = ((uint32_t)1U<<18U))
#define NAND_ALE_Get()               ((PIOD_REGS->PIO_PDSR >> 18U) & 0x1U)
#define NAND_ALE_PIN                  PIO_PIN_PD18

/*** Macros for NAND_WP pin ***/
#define NAND_WP_Set()               (PIOA_REGS->PIO_SODR = ((uint32_t)1U<<27U))
#define NAND_WP_Clear()             (PIOA_REGS->PIO_CODR = ((uint32_t)1U<<27U))
#define NAND_WP_Toggle()            (PIOA_REGS->PIO_ODSR ^= ((uint32_t)1U<<27U))
#define NAND_WP_OutputEnable()      (PIOA_REGS->PIO_OER = ((uint32_t)1U<<27U))
#define NAND_WP_InputEnable()       (PIOA_REGS->PIO_ODR = ((uint32_t)1U<<27U))
#define NAND_WP_Get()               ((PIOA_REGS->PIO_PDSR >> 27U) & 0x1U)
#define NAND_WP_PIN                  PIO_PIN_PA27


// *****************************************************************************
/* PIO Port

  Summary:
    Identifies the available PIO Ports.

  Description:
    This enumeration identifies the available PIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/


#define    PIO_PORT_A       (PIOA_BASE_ADDRESS)
#define    PIO_PORT_B       (PIOB_BASE_ADDRESS)
#define     PIO_PORT_C      (PIOC_BASE_ADDRESS)
#define     PIO_PORT_D      (PIOD_BASE_ADDRESS)
#define     PIO_PORT_E      (PIOE_BASE_ADDRESS)
typedef uint32_t PIO_PORT;

// *****************************************************************************
/* PIO Port Pins

  Summary:
    Identifies the available PIO port pins.

  Description:
    This enumeration identifies the available PIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

#define    PIO_PIN_PA0     (0U)
#define    PIO_PIN_PA1     (1U)
#define    PIO_PIN_PA2     (2U)
#define    PIO_PIN_PA3     (3U)
#define    PIO_PIN_PA4     (4U)
#define    PIO_PIN_PA5     (5U)
#define    PIO_PIN_PA6     (6U)
#define    PIO_PIN_PA7     (7U)
#define    PIO_PIN_PA8     (8U)
#define    PIO_PIN_PA9     (9U)
#define    PIO_PIN_PA10     (10U)
#define    PIO_PIN_PA11     (11U)
#define    PIO_PIN_PA12     (12U)
#define    PIO_PIN_PA13     (13U)
#define    PIO_PIN_PA14     (14U)
#define    PIO_PIN_PA15     (15U)
#define    PIO_PIN_PA16     (16U)
#define    PIO_PIN_PA17     (17U)
#define    PIO_PIN_PA18     (18U)
#define    PIO_PIN_PA19     (19U)
#define    PIO_PIN_PA20     (20U)
#define    PIO_PIN_PA21     (21U)
#define    PIO_PIN_PA22     (22U)
#define    PIO_PIN_PA23     (23U)
#define    PIO_PIN_PA24     (24U)
#define    PIO_PIN_PA25     (25U)
#define    PIO_PIN_PA26     (26U)
#define    PIO_PIN_PA27     (27U)
#define    PIO_PIN_PA28     (28U)
#define    PIO_PIN_PA29     (29U)
#define    PIO_PIN_PA30     (30U)
#define    PIO_PIN_PA31     (31U)
#define    PIO_PIN_PB0     (32U)
#define    PIO_PIN_PB1     (33U)
#define    PIO_PIN_PB2     (34U)
#define    PIO_PIN_PB3     (35U)
#define    PIO_PIN_PB4     (36U)
#define    PIO_PIN_PB5     (37U)
#define    PIO_PIN_PB6     (38U)
#define    PIO_PIN_PB7     (39U)
#define    PIO_PIN_PB8     (40U)
#define    PIO_PIN_PB9     (41U)
#define    PIO_PIN_PB12     (44U)
#define    PIO_PIN_PB13     (45U)
#define    PIO_PIN_PC0     (64U)
#define    PIO_PIN_PC1     (65U)
#define    PIO_PIN_PC2     (66U)
#define    PIO_PIN_PC3     (67U)
#define    PIO_PIN_PC4     (68U)
#define    PIO_PIN_PC5     (69U)
#define    PIO_PIN_PC6     (70U)
#define    PIO_PIN_PC7     (71U)
#define    PIO_PIN_PC8     (72U)
#define    PIO_PIN_PC9     (73U)
#define    PIO_PIN_PC10     (74U)
#define    PIO_PIN_PC11     (75U)
#define    PIO_PIN_PC12     (76U)
#define    PIO_PIN_PC13     (77U)
#define    PIO_PIN_PC14     (78U)
#define    PIO_PIN_PC15     (79U)
#define    PIO_PIN_PC16     (80U)
#define    PIO_PIN_PC17     (81U)
#define    PIO_PIN_PC18     (82U)
#define    PIO_PIN_PC19     (83U)
#define    PIO_PIN_PC20     (84U)
#define    PIO_PIN_PC21     (85U)
#define    PIO_PIN_PC22     (86U)
#define    PIO_PIN_PC23     (87U)
#define    PIO_PIN_PC24     (88U)
#define    PIO_PIN_PC25     (89U)
#define    PIO_PIN_PC26     (90U)
#define    PIO_PIN_PC27     (91U)
#define    PIO_PIN_PC28     (92U)
#define    PIO_PIN_PC29     (93U)
#define    PIO_PIN_PC30     (94U)
#define    PIO_PIN_PC31     (95U)
#define    PIO_PIN_PD0     (96U)
#define    PIO_PIN_PD1     (97U)
#define    PIO_PIN_PD2     (98U)
#define    PIO_PIN_PD3     (99U)
#define    PIO_PIN_PD4     (100U)
#define    PIO_PIN_PD5     (101U)
#define    PIO_PIN_PD6     (102U)
#define    PIO_PIN_PD7     (103U)
#define    PIO_PIN_PD8     (104U)
#define    PIO_PIN_PD9     (105U)
#define    PIO_PIN_PD10     (106U)
#define    PIO_PIN_PD11     (107U)
#define    PIO_PIN_PD12     (108U)
#define    PIO_PIN_PD13     (109U)
#define    PIO_PIN_PD14     (110U)
#define    PIO_PIN_PD15     (111U)
#define    PIO_PIN_PD16     (112U)
#define    PIO_PIN_PD17     (113U)
#define    PIO_PIN_PD18     (114U)
#define    PIO_PIN_PD19     (115U)
#define    PIO_PIN_PD20     (116U)
#define    PIO_PIN_PD21     (117U)
#define    PIO_PIN_PD22     (118U)
#define    PIO_PIN_PD23     (119U)
#define    PIO_PIN_PD24     (120U)
#define    PIO_PIN_PD25     (121U)
#define    PIO_PIN_PD26     (122U)
#define    PIO_PIN_PD27     (123U)
#define    PIO_PIN_PD28     (124U)
#define    PIO_PIN_PD29     (125U)
#define    PIO_PIN_PD30     (126U)
#define    PIO_PIN_PD31     (127U)
#define    PIO_PIN_PE0     (128U)
#define    PIO_PIN_PE1     (129U)
#define    PIO_PIN_PE2     (130U)
#define    PIO_PIN_PE3     (131U)
#define    PIO_PIN_PE4     (132U)
#define    PIO_PIN_PE5     (133U)

    /* This element should not be used in any of the PIO APIs.
       It will be used by other modules or application to denote that none of the PIO Pin is used */
#define    PIO_PIN_NONE         ( -1)

typedef uint32_t PIO_PIN;


void PIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t PIO_PortRead(PIO_PORT port);

void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value);

uint32_t PIO_PortLatchRead ( PIO_PORT port );

void PIO_PortSet(PIO_PORT port, uint32_t mask);

void PIO_PortClear(PIO_PORT port, uint32_t mask);

void PIO_PortToggle(PIO_PORT port, uint32_t mask);

void PIO_PortInputEnable(PIO_PORT port, uint32_t mask);

void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: PIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void PIO_PinWrite(PIO_PIN pin, bool value)
{
    PIO_PortWrite((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U))), (uint32_t)(0x1) << (pin & 0x1fU), (uint32_t)(value) << (pin & 0x1fU));
}

static inline bool PIO_PinRead(PIO_PIN pin)
{
    return (bool)((PIO_PortRead((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U)))) >> (pin & 0x1FU)) & 0x1U);
}

static inline bool PIO_PinLatchRead(PIO_PIN pin)
{
    return (bool)((PIO_PortLatchRead((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U)))) >> (pin & 0x1FU)) & 0x1U);
}

static inline void PIO_PinToggle(PIO_PIN pin)
{
    PIO_PortToggle((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U))), 0x1UL << (pin & 0x1FU));
}

static inline void PIO_PinSet(PIO_PIN pin)
{
    PIO_PortSet((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5))), 0x1UL << (pin & 0x1FU));
}

static inline void PIO_PinClear(PIO_PIN pin)
{
    PIO_PortClear((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U))), 0x1UL << (pin & 0x1FU));
}

static inline void PIO_PinInputEnable(PIO_PIN pin)
{
    PIO_PortInputEnable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U))), 0x1UL << (pin & 0x1FU));
}

static inline void PIO_PinOutputEnable(PIO_PIN pin)
{
    PIO_PortOutputEnable((PIO_PORT)(PIOA_BASE_ADDRESS + (0x200U * (pin>>5U))), 0x1UL << (pin & 0x1FU));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_PIO_H
