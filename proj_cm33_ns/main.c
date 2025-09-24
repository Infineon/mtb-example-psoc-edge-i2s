/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for non-secure
*                    application in the CM33 CPU
*
* Related Document : See README.md
*
********************************************************************************
* Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/

#include "retarget_io_init.h"
#include "app_i2s.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* Button interrupt priority */
#define USER_BTN_1_ISR_PRIORITY           (5u)

/* The timeout value in microsecond used to wait for core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC          (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR          (CYMEM_CM33_0_m55_nvm_START + \
                                        CYBSP_MCUBOOT_HEADER_SIZE)

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Interrupt config structure */
cy_stc_sysint_t intrCfg =
{
    CYBSP_USER_BTN_IRQ,
    USER_BTN_1_ISR_PRIORITY
};

/*******************************************************************************
 * Function Name: user_button_interrupt_handler
 ********************************************************************************
* Summary:
*  User button interrupt handler function. 
*
* Parameters:
*  None
*
* Return:
*  none
*
*******************************************************************************/
void user_button_interrupt_handler(void)
{
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN);
    Cy_GPIO_Write(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN, CYBSP_LED_STATE_ON);
    app_i2s_enable();
    app_i2s_activate();
}

/*******************************************************************************
* Function Name: app_user_button_init
********************************************************************************
* Summary:
* User defined function to register interrupt handler
*
* Parameters:
*  none
*
* Return :
*  none
*
*******************************************************************************/
static void app_user_button_init(void)
{
    /* Initialize the interrupt and register interrupt callback */
    Cy_SysInt_Init(&intrCfg, &user_button_interrupt_handler);

    /* Enable the interrupt in the NVIC */
    NVIC_EnableIRQ(intrCfg.intrSrc);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  The main function for the Cortex-M33 CPU does the following:
*   Initialization:
*   - Initializes all the hardware blocks
*   - Enable Cortex-M55 CPU
*   Do forever loop:
*   - Enters Sleep Mode.
*   - Check if the User Button was pressed. If yes, plays the audio track.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /* Board init failed. Stop program execution */
    handle_app_error(result);

    __enable_irq();

    /* Clear GPIO and NVIC interrupt before initializing to avoid false
     * triggering.
     */
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN);
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN2_PORT, CYBSP_USER_BTN2_PIN);
    NVIC_ClearPendingIRQ(CYBSP_USER_BTN1_IRQ);
    NVIC_ClearPendingIRQ(CYBSP_USER_BTN2_IRQ);

    /* Initialize retarget-io */
    init_retarget_io();
    
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("****************** \
    PSOC Edge MCU: I2S Example \
    ****************** \r\n\n");

    printf("Press User button 1 to play pre-recorded audio.\r\n");

    /* Initialize the User Button */
    app_user_button_init();

    /* I2S initialization */
    app_i2s_init();

    /* TLV codec initiailization */
    app_tlv_codec_init();

    /* Enable CM55. */
    /* CY_CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, 
                     CM55_BOOT_WAIT_TIME_USEC);

    for (;;)
    {
        if(audio_playback_ended)
        {
            audio_playback_ended = false;
            app_i2s_deactivate();
            Cy_GPIO_Write(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN, CYBSP_LED_STATE_OFF);
            Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        }
    }
}
/* [] END OF FILE */
