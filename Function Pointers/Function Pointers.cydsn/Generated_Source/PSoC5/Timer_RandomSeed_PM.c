/*******************************************************************************
* File Name: Timer_RandomSeed_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_RandomSeed.h"

static Timer_RandomSeed_backupStruct Timer_RandomSeed_backup;


/*******************************************************************************
* Function Name: Timer_RandomSeed_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RandomSeed_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_RandomSeed_SaveConfig(void) 
{
    #if (!Timer_RandomSeed_UsingFixedFunction)
        Timer_RandomSeed_backup.TimerUdb = Timer_RandomSeed_ReadCounter();
        Timer_RandomSeed_backup.InterruptMaskValue = Timer_RandomSeed_STATUS_MASK;
        #if (Timer_RandomSeed_UsingHWCaptureCounter)
            Timer_RandomSeed_backup.TimerCaptureCounter = Timer_RandomSeed_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_RandomSeed_UDB_CONTROL_REG_REMOVED)
            Timer_RandomSeed_backup.TimerControlRegister = Timer_RandomSeed_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_RandomSeed_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RandomSeed_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_RandomSeed_RestoreConfig(void) 
{   
    #if (!Timer_RandomSeed_UsingFixedFunction)

        Timer_RandomSeed_WriteCounter(Timer_RandomSeed_backup.TimerUdb);
        Timer_RandomSeed_STATUS_MASK =Timer_RandomSeed_backup.InterruptMaskValue;
        #if (Timer_RandomSeed_UsingHWCaptureCounter)
            Timer_RandomSeed_SetCaptureCount(Timer_RandomSeed_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_RandomSeed_UDB_CONTROL_REG_REMOVED)
            Timer_RandomSeed_WriteControlRegister(Timer_RandomSeed_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_RandomSeed_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RandomSeed_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_RandomSeed_Sleep(void) 
{
    #if(!Timer_RandomSeed_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_RandomSeed_CTRL_ENABLE == (Timer_RandomSeed_CONTROL & Timer_RandomSeed_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_RandomSeed_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_RandomSeed_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_RandomSeed_Stop();
    Timer_RandomSeed_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_RandomSeed_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RandomSeed_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_RandomSeed_Wakeup(void) 
{
    Timer_RandomSeed_RestoreConfig();
    #if(!Timer_RandomSeed_UDB_CONTROL_REG_REMOVED)
        if(Timer_RandomSeed_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_RandomSeed_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
