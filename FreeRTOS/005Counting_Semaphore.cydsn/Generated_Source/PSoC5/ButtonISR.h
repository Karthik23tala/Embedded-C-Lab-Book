/*******************************************************************************
* File Name: ButtonISR.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ButtonISR_H) /* Pins ButtonISR_H */
#define CY_PINS_ButtonISR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ButtonISR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ButtonISR__PORT == 15 && ((ButtonISR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ButtonISR_Write(uint8 value);
void    ButtonISR_SetDriveMode(uint8 mode);
uint8   ButtonISR_ReadDataReg(void);
uint8   ButtonISR_Read(void);
void    ButtonISR_SetInterruptMode(uint16 position, uint16 mode);
uint8   ButtonISR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ButtonISR_SetDriveMode() function.
     *  @{
     */
        #define ButtonISR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ButtonISR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ButtonISR_DM_RES_UP          PIN_DM_RES_UP
        #define ButtonISR_DM_RES_DWN         PIN_DM_RES_DWN
        #define ButtonISR_DM_OD_LO           PIN_DM_OD_LO
        #define ButtonISR_DM_OD_HI           PIN_DM_OD_HI
        #define ButtonISR_DM_STRONG          PIN_DM_STRONG
        #define ButtonISR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ButtonISR_MASK               ButtonISR__MASK
#define ButtonISR_SHIFT              ButtonISR__SHIFT
#define ButtonISR_WIDTH              1u

/* Interrupt constants */
#if defined(ButtonISR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ButtonISR_SetInterruptMode() function.
     *  @{
     */
        #define ButtonISR_INTR_NONE      (uint16)(0x0000u)
        #define ButtonISR_INTR_RISING    (uint16)(0x0001u)
        #define ButtonISR_INTR_FALLING   (uint16)(0x0002u)
        #define ButtonISR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ButtonISR_INTR_MASK      (0x01u) 
#endif /* (ButtonISR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ButtonISR_PS                     (* (reg8 *) ButtonISR__PS)
/* Data Register */
#define ButtonISR_DR                     (* (reg8 *) ButtonISR__DR)
/* Port Number */
#define ButtonISR_PRT_NUM                (* (reg8 *) ButtonISR__PRT) 
/* Connect to Analog Globals */                                                  
#define ButtonISR_AG                     (* (reg8 *) ButtonISR__AG)                       
/* Analog MUX bux enable */
#define ButtonISR_AMUX                   (* (reg8 *) ButtonISR__AMUX) 
/* Bidirectional Enable */                                                        
#define ButtonISR_BIE                    (* (reg8 *) ButtonISR__BIE)
/* Bit-mask for Aliased Register Access */
#define ButtonISR_BIT_MASK               (* (reg8 *) ButtonISR__BIT_MASK)
/* Bypass Enable */
#define ButtonISR_BYP                    (* (reg8 *) ButtonISR__BYP)
/* Port wide control signals */                                                   
#define ButtonISR_CTL                    (* (reg8 *) ButtonISR__CTL)
/* Drive Modes */
#define ButtonISR_DM0                    (* (reg8 *) ButtonISR__DM0) 
#define ButtonISR_DM1                    (* (reg8 *) ButtonISR__DM1)
#define ButtonISR_DM2                    (* (reg8 *) ButtonISR__DM2) 
/* Input Buffer Disable Override */
#define ButtonISR_INP_DIS                (* (reg8 *) ButtonISR__INP_DIS)
/* LCD Common or Segment Drive */
#define ButtonISR_LCD_COM_SEG            (* (reg8 *) ButtonISR__LCD_COM_SEG)
/* Enable Segment LCD */
#define ButtonISR_LCD_EN                 (* (reg8 *) ButtonISR__LCD_EN)
/* Slew Rate Control */
#define ButtonISR_SLW                    (* (reg8 *) ButtonISR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ButtonISR_PRTDSI__CAPS_SEL       (* (reg8 *) ButtonISR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ButtonISR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ButtonISR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ButtonISR_PRTDSI__OE_SEL0        (* (reg8 *) ButtonISR__PRTDSI__OE_SEL0) 
#define ButtonISR_PRTDSI__OE_SEL1        (* (reg8 *) ButtonISR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ButtonISR_PRTDSI__OUT_SEL0       (* (reg8 *) ButtonISR__PRTDSI__OUT_SEL0) 
#define ButtonISR_PRTDSI__OUT_SEL1       (* (reg8 *) ButtonISR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ButtonISR_PRTDSI__SYNC_OUT       (* (reg8 *) ButtonISR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ButtonISR__SIO_CFG)
    #define ButtonISR_SIO_HYST_EN        (* (reg8 *) ButtonISR__SIO_HYST_EN)
    #define ButtonISR_SIO_REG_HIFREQ     (* (reg8 *) ButtonISR__SIO_REG_HIFREQ)
    #define ButtonISR_SIO_CFG            (* (reg8 *) ButtonISR__SIO_CFG)
    #define ButtonISR_SIO_DIFF           (* (reg8 *) ButtonISR__SIO_DIFF)
#endif /* (ButtonISR__SIO_CFG) */

/* Interrupt Registers */
#if defined(ButtonISR__INTSTAT)
    #define ButtonISR_INTSTAT            (* (reg8 *) ButtonISR__INTSTAT)
    #define ButtonISR_SNAP               (* (reg8 *) ButtonISR__SNAP)
    
	#define ButtonISR_0_INTTYPE_REG 		(* (reg8 *) ButtonISR__0__INTTYPE)
#endif /* (ButtonISR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ButtonISR_H */


/* [] END OF FILE */
