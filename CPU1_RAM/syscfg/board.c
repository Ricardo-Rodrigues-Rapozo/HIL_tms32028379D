/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "board.h"

//*****************************************************************************
//
// Board Configurations
// Initializes the rest of the modules. 
// Call this function in your application if you wish to do all module 
// initialization.
// If you wish to not use some of the initializations, instead of the 
// Board_init use the individual Module_inits
//
//*****************************************************************************
void Board_init()
{
	EALLOW;

	PinMux_init();
	INPUTXBAR_init();
	SYNC_init();
	CLA_init();
	MEMCFG_init();
	ADC_init();
	CMPSS_init();
	CPUTIMER_init();
	DAC_init();
	EPWM_init();
	EPWMXBAR_init();
	GPIO_init();
	XINT_init();
	INTERRUPT_init();

	EDIS;
}

//*****************************************************************************
//
// PINMUX Configurations
//
//*****************************************************************************
void PinMux_init()
{
	//
	// PinMux for modules assigned to CPU1
	//
	
	//
	// EPWM1 -> myEPWM1 Pinmux
	//
	GPIO_setPinConfig(myEPWM1_EPWMA_PIN_CONFIG);
	GPIO_setPadConfig(myEPWM1_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myEPWM1_EPWMA_GPIO, GPIO_QUAL_SYNC);

	// GPIO122 -> myGPIO0 Pinmux
	GPIO_setPinConfig(GPIO_122_GPIO122);

}

//*****************************************************************************
//
// ADC Configurations
//
//*****************************************************************************
void ADC_init(){
	ADC0_init();
}

void ADC0_init(){
	//
	// Configures the analog-to-digital converter module prescaler.
	//
	ADC_setPrescaler(ADC0_BASE, ADC_CLK_DIV_4_0);
	//
	// Configures the analog-to-digital converter resolution and signal mode.
	//
	ADC_setMode(ADC0_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
	//
	// Sets the timing of the end-of-conversion pulse
	//
	ADC_setInterruptPulseMode(ADC0_BASE, ADC_PULSE_END_OF_CONV);
	//
	// Powers up the analog-to-digital converter core.
	//
	ADC_enableConverter(ADC0_BASE);
	//
	// Delay for 1ms to allow ADC time to power up
	//
	DEVICE_DELAY_US(500);
	//
	// SOC Configuration: Setup ADC EPWM channel and trigger settings
	//
	// Disables SOC burst mode.
	//
	ADC_disableBurstMode(ADC0_BASE);
	//
	// Sets the priority mode of the SOCs.
	//
	ADC_setSOCPriority(ADC0_BASE, ADC_PRI_ALL_ROUND_ROBIN);
	//
	// Start of Conversion 0 Configuration
	//
	//
	// Configures a start-of-conversion (SOC) in the ADC and its interrupt SOC trigger.
	// 	  	SOC number		: 0
	//	  	Trigger			: ADC_TRIGGER_EPWM1_SOCA
	//	  	Channel			: ADC_CH_ADCIN5
	//	 	Sample Window	: 15 SYSCLK cycles
	//		Interrupt Trigger: ADC_INT_SOC_TRIGGER_NONE
	//
	ADC_setupSOC(ADC0_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN5, 15U);
	ADC_setInterruptSOCTrigger(ADC0_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
	//
	// ADC Interrupt 1 Configuration
	// 		Source	: ADC_SOC_NUMBER0
	// 		Interrupt Source: enabled
	//		Continuous Mode	: enabled
	//
	//
	ADC_setInterruptSource(ADC0_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
	ADC_clearInterruptStatus(ADC0_BASE, ADC_INT_NUMBER1);
	ADC_enableContinuousMode(ADC0_BASE, ADC_INT_NUMBER1);
	ADC_enableInterrupt(ADC0_BASE, ADC_INT_NUMBER1);
}


//*****************************************************************************
//
// CLA Configurations
//
//*****************************************************************************

void myCLA0_init(){
	//
    // Configure all CLA task vectors
    // On Type-1 and Type-2 CLAs the MVECT registers accept full 16-bit task addresses as
    // opposed to offsets used on older Type-0 CLAs
    //
#pragma diag_suppress=770
    //
    // CLA Task 1
    //
    CLA_mapTaskVector(myCLA0_BASE, CLA_MVECT_1, (uint16_t)&Cla1Task1);
    CLA_setTriggerSource(CLA_TASK_1, CLA_TRIGGER_SOFTWARE);
#pragma diag_warning=770
	//
    // Enable the IACK instruction to start a task on CLA in software
    // for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
    // subset of tasks) by writing to their respective bits in the
    // MIER register
    //
	CLA_enableIACK(myCLA0_BASE);
    CLA_enableTasks(myCLA0_BASE, CLA_TASKFLAG_1 );
}


void CLA_init()
{
#ifdef _FLASH
#ifndef CMDTOOL // Linker command tool is not used

    extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    extern uint32_t Cla1ConstRunStart, Cla1ConstLoadStart, Cla1ConstLoadSize;

    //
    // Copy the program and constants from FLASH to RAM before configuring
    // the CLA
    //
    memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
           (uint32_t)&Cla1funcsLoadSize);
    memcpy((uint32_t *)&Cla1ConstRunStart, (uint32_t *)&Cla1ConstLoadStart,
        (uint32_t)&Cla1ConstLoadSize );


#endif //CMDTOOL
#endif //_FLASH

	myCLA0_init();
}

//*****************************************************************************
//
// CMPSS Configurations
//
//*****************************************************************************
void CMPSS_init(){
	myCMPSS0_init();
}

void myCMPSS0_init(){
    //
    // Sets the configuration for the high comparator.
    //
    CMPSS_configHighComparator(myCMPSS0_BASE,(CMPSS_INSRC_PIN));
    //
    // Sets the configuration for the low comparator.
    //
    CMPSS_configLowComparator(myCMPSS0_BASE,(CMPSS_INSRC_PIN));
    //
    // Sets the configuration for the internal comparator DACs.
    //
    CMPSS_configDAC(myCMPSS0_BASE,(CMPSS_DACVAL_SYSCLK | CMPSS_DACREF_VDDA | CMPSS_DACSRC_SHDW));
    //
    // Sets the value of the internal DAC of the high comparator.
    //
    CMPSS_setDACValueHigh(myCMPSS0_BASE,20U);
    //
    // Sets the value of the internal DAC of the low comparator.
    //
    CMPSS_setDACValueLow(myCMPSS0_BASE,0U);
    //
    //  Configures the digital filter of the high comparator.
    //
    CMPSS_configFilterHigh(myCMPSS0_BASE, 0U, 1U, 1U);
    //
    // Configures the digital filter of the low comparator.
    //
    CMPSS_configFilterLow(myCMPSS0_BASE, 0U, 1U, 1U);
    //
    // Sets the output signal configuration for the high comparator.
    //
    CMPSS_configOutputsHigh(myCMPSS0_BASE,(CMPSS_TRIPOUT_ASYNC_COMP | CMPSS_TRIP_ASYNC_COMP));
    //
    // Sets the output signal configuration for the low comparator.
    //
    CMPSS_configOutputsLow(myCMPSS0_BASE,(CMPSS_TRIPOUT_ASYNC_COMP | CMPSS_TRIP_ASYNC_COMP));
    //
    // Sets the comparator hysteresis settings.
    //
    CMPSS_setHysteresis(myCMPSS0_BASE,0U);
    //
    // Configures the comparator subsystem's ramp generator.
    //
    CMPSS_configRamp(myCMPSS0_BASE,0U,0U,0U,1U,true);
    //
    // Disables reset of HIGH comparator digital filter output latch on PWMSYNC
    //
    CMPSS_disableLatchResetOnPWMSYNCHigh(myCMPSS0_BASE);
    //
    // Disables reset of LOW comparator digital filter output latch on PWMSYNC
    //
    CMPSS_disableLatchResetOnPWMSYNCLow(myCMPSS0_BASE);
    //
    // Configures whether or not the digital filter latches are reset by PWMSYNC
    //
    CMPSS_configLatchOnPWMSYNC(myCMPSS0_BASE,false,false);
    //
    // Enables the CMPSS module.
    //
    CMPSS_enableModule(myCMPSS0_BASE);
    //
    // Delay for CMPSS DAC to power up.
    //
    DEVICE_DELAY_US(500);
}

//*****************************************************************************
//
// CPUTIMER Configurations
//
//*****************************************************************************
void CPUTIMER_init(){
	myCPUTIMER1_init();
}

void myCPUTIMER1_init(){
	CPUTimer_setEmulationMode(myCPUTIMER1_BASE, CPUTIMER_EMULATIONMODE_RUNFREE);
	CPUTimer_setPreScaler(myCPUTIMER1_BASE, 0U);
	CPUTimer_setPeriod(myCPUTIMER1_BASE, 999U);
	CPUTimer_enableInterrupt(myCPUTIMER1_BASE);
	CPUTimer_stopTimer(myCPUTIMER1_BASE);

	CPUTimer_reloadTimerCounter(myCPUTIMER1_BASE);
	CPUTimer_startTimer(myCPUTIMER1_BASE);
}

//*****************************************************************************
//
// DAC Configurations
//
//*****************************************************************************
void DAC_init(){
	DAC0_init();
}

void DAC0_init(){
	//
	// Set DAC reference voltage.
	//
	DAC_setReferenceVoltage(DAC0_BASE, DAC_REF_ADC_VREFHI);
	//
	// Set DAC load mode.
	//
	DAC_setLoadMode(DAC0_BASE, DAC_LOAD_SYSCLK);
	//
	// Enable the DAC output
	//
	DAC_enableOutput(DAC0_BASE);
	//
	// Set the DAC shadow output
	//
	DAC_setShadowValue(DAC0_BASE, 0U);

	//
	// Delay for buffered DAC to power up.
	//
	DEVICE_DELAY_US(500);
}

//*****************************************************************************
//
// EPWM Configurations
//
//*****************************************************************************
void EPWM_init(){
    EPWM_setEmulationMode(myEPWM1_BASE, EPWM_EMULATION_FREE_RUN);	
    EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_2, EPWM_HSCLOCK_DIVIDER_1);	
    EPWM_setTimeBasePeriod(myEPWM1_BASE, 2500);	
    EPWM_setTimeBaseCounter(myEPWM1_BASE, 0);	
    EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);	
    EPWM_disablePhaseShiftLoad(myEPWM1_BASE);	
    EPWM_setPhaseShift(myEPWM1_BASE, 0);	
    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, 1250);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_B, 1);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM1_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM1_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setRisingEdgeDelayCountShadowLoadMode(myEPWM1_BASE, EPWM_RED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableRisingEdgeDelayCountShadowLoadMode(myEPWM1_BASE);	
    EPWM_setFallingEdgeDelayCountShadowLoadMode(myEPWM1_BASE, EPWM_FED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableFallingEdgeDelayCountShadowLoadMode(myEPWM1_BASE);	
    EPWM_setTripZoneAction(myEPWM1_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);	
    EPWM_enableTripZoneSignals(myEPWM1_BASE, EPWM_TZ_SIGNAL_OSHT4);	
    EPWM_enableInterrupt(myEPWM1_BASE);	
    EPWM_setInterruptSource(myEPWM1_BASE, EPWM_INT_TBCTR_ZERO);	
    EPWM_setInterruptEventCount(myEPWM1_BASE, 1);	
    EPWM_enableADCTrigger(myEPWM1_BASE, EPWM_SOC_A);	
    EPWM_setADCTriggerSource(myEPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_ZERO);	
    EPWM_setADCTriggerEventPrescale(myEPWM1_BASE, EPWM_SOC_A, 1);	
}

//*****************************************************************************
//
// EPWMXBAR Configurations
//
//*****************************************************************************
void EPWMXBAR_init(){
	myEPWMXBAR0_init();
}

void myEPWMXBAR0_init(){
		
	XBAR_setEPWMMuxConfig(myEPWMXBAR0, XBAR_EPWM_MUX00_CMPSS1_CTRIPH);
	XBAR_enableEPWMMux(myEPWMXBAR0, XBAR_MUX00);
}

//*****************************************************************************
//
// GPIO Configurations
//
//*****************************************************************************
void GPIO_init(){
	myGPIO0_init();
}

void myGPIO0_init(){
	GPIO_writePin(myGPIO0, 0);
	GPIO_setPadConfig(myGPIO0, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myGPIO0, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(myGPIO0, GPIO_DIR_MODE_IN);
	GPIO_setControllerCore(myGPIO0, GPIO_CORE_CPU1);
}

//*****************************************************************************
//
// INPUTXBAR Configurations
//
//*****************************************************************************
void INPUTXBAR_init(){
	myINPUTXBARINPUT0_init();
}

void myINPUTXBARINPUT0_init(){
	XBAR_setInputPin(myINPUTXBARINPUT0_INPUT, myINPUTXBARINPUT0_SOURCE);
}

//*****************************************************************************
//
// INTERRUPT Configurations
//
//*****************************************************************************
void INTERRUPT_init(){
	
	// Interrupt Settings for INT_myCPUTIMER1
	// ISR need to be defined for the registered interrupts
	Interrupt_register(INT_myCPUTIMER1, &INT_myCPUTIMER1_ISR);
	Interrupt_enable(INT_myCPUTIMER1);
}
//*****************************************************************************
//
// MEMCFG Configurations
//
//*****************************************************************************
void MEMCFG_init(){
	//
	// Initialize RAMs
	//
	MemCfg_initSections(MEMCFG_SECT_MSGCPUTOCLA1);
	MemCfg_initSections(MEMCFG_SECT_MSGCLA1TOCPU);
	while(!MemCfg_getInitStatus(MEMCFG_SECT_MSGCPUTOCLA1));
	while(!MemCfg_getInitStatus(MEMCFG_SECT_MSGCLA1TOCPU));
	//
	// Configure LSRAMs
	//
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS0, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS1, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
	MemCfg_setCLAMemType(MEMCFG_SECT_LS1, MEMCFG_CLA_MEM_DATA);
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS2, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS3, MEMCFG_LSRAMCONTROLLER_CPU_ONLY);
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS4, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
	MemCfg_setCLAMemType(MEMCFG_SECT_LS4, MEMCFG_CLA_MEM_PROGRAM);
	MemCfg_setLSRAMControllerSel(MEMCFG_SECT_LS5, MEMCFG_LSRAMCONTROLLER_CPU_CLA1);
	MemCfg_setCLAMemType(MEMCFG_SECT_LS5, MEMCFG_CLA_MEM_PROGRAM);
	//
	// Configure GSRAMs
	//
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS0, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS1, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS2, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS3, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS4, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS5, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS6, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS7, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS8, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS9, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS10, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS11, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS12, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS13, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS14, MEMCFG_GSRAMCONTROLLER_CPU1);
	MemCfg_setGSRAMControllerSel(MEMCFG_SECT_GS15, MEMCFG_GSRAMCONTROLLER_CPU1);
	//
	// ROM Access Configuration
	//
	MemCfg_enableROMWaitState();
	MemCfg_disableROMPrefetch();
	//
	// Configure Access Protection for RAMs
	//
	MemCfg_setProtection(MEMCFG_SECT_D0, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_D1, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS0, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS1, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS2, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS3, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS4, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_LS5, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS0, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS1, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS2, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS3, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS4, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS5, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS6, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS7, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS8, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS9, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS10, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS11, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS12, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS13, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS14, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	MemCfg_setProtection(MEMCFG_SECT_GS15, MEMCFG_PROT_ALLOWCPUFETCH | MEMCFG_PROT_ALLOWCPUWRITE | MEMCFG_PROT_ALLOWDMAWRITE);
	//
	// Lock/Commit Registers
	//
	//
	// Enable Access Violation Interrupt
	//
	//
	// Correctable error Interrupt
	//
	MemCfg_setCorrErrorThreshold(0);
	MemCfg_disableCorrErrorInterrupt(MEMCFG_CERR_CPUREAD);
}        
//*****************************************************************************
//
// SYNC Scheme Configurations
//
//*****************************************************************************
void SYNC_init(){
	SysCtl_setSyncOutputConfig(SYSCTL_SYNC_OUT_SRC_EPWM1SYNCOUT);
	//
	// For EPWM1, the sync input is: SYSCTL_SYNC_IN_SRC_EXTSYNCIN1
	//
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM7, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM10, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP1, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	//
	// SOCA
	//
	SysCtl_enableExtADCSOCSource(0);
	//
	// SOCB
	//
	SysCtl_enableExtADCSOCSource(0);
}
//*****************************************************************************
//
// XINT Configurations
//
//*****************************************************************************
void XINT_init(){
	myGPIO0_XINT_init();
}

void myGPIO0_XINT_init(){
	GPIO_setInterruptType(myGPIO0_XINT, GPIO_INT_TYPE_BOTH_EDGES);
	GPIO_setInterruptPin(myGPIO0, myGPIO0_XINT);
	GPIO_enableInterrupt(myGPIO0_XINT);
}

