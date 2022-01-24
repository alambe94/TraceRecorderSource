/*
 * Trace Recorder for Tracealyzer v4.6.0(RC1)
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The hardware abstraction layer for the trace recorder.
 */

#ifndef TRC_HARDWARE_PORT_H
#define TRC_HARDWARE_PORT_H

#include <trcDefines.h>


#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NOT_SET)
	#error "TRC_CFG_HARDWARE_PORT not selected - see trcConfig.h"
#endif

/*******************************************************************************
 * TRC_IRQ_PRIORITY_ORDER
 *
 * Macro which should be defined as an integer of 0 or 1.
 *
 * This should be 0 if lower IRQ priority values implies higher priority
 * levels, such as on ARM Cortex M. If the opposite scheme is used, i.e.,
 * if higher IRQ priority values means higher priority, this should be 1.
 *
 * This setting is not critical. It is used only to sort and colorize the
 * interrupts in priority order, in case you record interrupts using
 * the vTraceStoreISRBegin and vTraceStoreISREnd routines.
 *
 ******************************************************************************
 *
 * HWTC Macros
 *
 * These macros provides a hardware isolation layer representing the
 * hardware timer/counter used for the event timestamping.
 *
 * TRC_HWTC_COUNT: How to read the current value of the timer/counter.
 *
 * TRC_HWTC_TYPE: Tells the type of timer/counter used for TRC_HWTC_COUNT:
 *
 * - TRC_FREE_RUNNING_32BIT_INCR:
 *   Free-running 32-bit timer/counter, counting upwards from 0.
 *
 * - TRC_FREE_RUNNING_32BIT_DECR
 *   Free-running 32-bit timer/counter, counting downwards from 0xFFFFFFFF.
 *
 * - TRC_OS_TIMER_INCR
 *	 Periodic timer that drives the OS tick interrupt, counting upwards
 *   from 0 until (TRC_HWTC_PERIOD-1).
 *
 * - TRC_OS_TIMER_DECR
 *	 Periodic timer that drives the OS tick interrupt, counting downwards
 *   from TRC_HWTC_PERIOD-1 until 0.
 *
 * - TRC_CUSTOM_TIMER_INCR
 *   A custom timer or counter independent of the OS tick, counting
 *   downwards from TRC_HWTC_PERIOD-1 until 0. (Currently only supported
 *   in streaming mode).
 *
 * - TRC_CUSTOM_TIMER_DECR
 *   A custom timer independent of the OS tick, counting downwards
 *   from TRC_HWTC_PERIOD-1 until 0. (Currently only supported
 *   in streaming mode).
 *
 * TRC_HWTC_PERIOD: The number of HWTC_COUNT ticks until the timer wraps
 * around. If using TRC_FREE_RUNNING_32BIT_INCR/DECR, this should be 0. 
 *
 * TRC_HWTC_FREQ_HZ: The clock rate of the TRC_HWTC_COUNT counter in Hz. If using 
 * TRC_OS_TIMER_INCR/DECR, this is should be TRC_HWTC_PERIOD * TRC_TICK_RATE_HZ.
 * If using a free-running timer, this is often TRACE_CPU_CLOCK_HZ (if running at
 * the core clock rate). If using TRC_CUSTOM_TIMER_INCR/DECR, this should match
 * the clock rate of your custom timer (i.e., TRC_HWTC_COUNT). If the default value
 * of TRC_HWTC_FREQ_HZ is incorrect for your setup, you can override it by calling
 * vTraceSetFrequency before calling vTraceEnable.
 *
 * TRC_HWTC_DIVISOR (used in snapshot mode only):
 * In snapshot mode, the timestamp resolution is TRC_HWTC_FREQ_HZ/TRC_HWTC_DIVISOR.
 * If the timer frequency is very high (hundreds of MHz), we recommend increasing
 * the TRC_HWTC_DIVISOR prescaler, to reduce the bandwidth needed to store
 * timestamps. This since extra "XTS" events are inserted if the time since the
 * previous event exceeds a certain limit (255 or 65535 depending on event type).
 * It is advised to keep the time between most events below 65535 native ticks
 * (after division by TRC_HWTC_DIVISOR) to avoid frequent XTS events.
 ******************************************************************************/

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NOT_SET)
	#error "TRC_CFG_HARDWARE_PORT not selected - see trcConfig.h"
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
/* This can be used as a template for any free-running 32-bit counter */
void vTraceTimerReset(void);
uint32_t uiTraceTimerGetFrequency(void);
uint32_t uiTraceTimerGetValue(void);

#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
#define TRC_HWTC_COUNT ((TraceUnsignedBaseType_t)uiTraceTimerGetValue())
#define TRC_HWTC_PERIOD 0
#define TRC_HWTC_DIVISOR 1
#define TRC_HWTC_FREQ_HZ ((TraceUnsignedBaseType_t)uiTraceTimerGetFrequency())

#define TRC_IRQ_PRIORITY_ORDER 1

#define TRC_PORT_SPECIFIC_INIT() vTraceTimerReset()

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win64)
/* This can be used as a template for any free-running 32-bit counter */
void vTraceTimerReset(void);
uint32_t uiTraceTimerGetFrequency(void);
uint32_t uiTraceTimerGetValue(void);

#define TRC_BASE_TYPE int64_t

#define TRC_UNSIGNED_BASE_TYPE uint64_t

#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
#define TRC_HWTC_COUNT ((TraceUnsignedBaseType_t)uiTraceTimerGetValue())
#define TRC_HWTC_PERIOD 0
#define TRC_HWTC_DIVISOR 1
#define TRC_HWTC_FREQ_HZ ((TraceUnsignedBaseType_t)uiTraceTimerGetFrequency())

#define TRC_IRQ_PRIORITY_ORDER 1

#define TRC_PORT_SPECIFIC_INIT() vTraceTimerReset()

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_HWIndependent)
	/* Timestamping by OS tick only (typically 1 ms resolution) */
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT 0
	#define TRC_HWTC_PERIOD 1
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ TRC_TICK_RATE_HZ

	/* Set the meaning of IRQ priorities in ISR tracing - see above */
	#define TRC_IRQ_PRIORITY_ORDER NOT_SET

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M)
	
	#ifndef __CORTEX_M
	#error "Can't find the CMSIS API. Please include your processor's header file in trcConfig.h" 	
	#endif
	
	#define TRACE_ALLOC_CRITICAL_SECTION() uint32_t __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* PRIMASK disables ALL interrupts - allows for tracing in any ISR */
	#define TRACE_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}

	/**************************************************************************
	* For Cortex-M3, M4 and M7, the DWT cycle counter is used for timestamping.
	* For Cortex-M0 and M0+, the SysTick timer is used since DWT is not
	* available. Systick timestamping can also be forced on Cortex-M3, M4 and
	* M7 by defining the preprocessor directive TRC_CFG_ARM_CM_USE_SYSTICK,
	* either directly below or in trcConfig.h.
	*
	* #define TRC_CFG_ARM_CM_USE_SYSTICK
    **************************************************************************/

	#if ((__CORTEX_M >= 0x03) && (! defined TRC_CFG_ARM_CM_USE_SYSTICK))
		
		void xTraceHardwarePortInitCortexM(void);

		#define TRC_REG_DEMCR (*(volatile uint32_t*)0xE000EDFC)
		#define TRC_REG_DWT_CTRL (*(volatile uint32_t*)0xE0001000)
		#define TRC_REG_DWT_CYCCNT (*(volatile uint32_t*)0xE0001004)
		#define TRC_REG_DWT_EXCCNT (*(volatile uint32_t*)0xE000100C)

		#define TRC_REG_ITM_LOCKACCESS (*(volatile uint32_t*)0xE0001FB0)		
		#define TRC_ITM_LOCKACCESS_UNLOCK (0xC5ACCE55)
		
		/* Bit mask for TRCENA bit in DEMCR - Global enable for DWT and ITM */
		#define TRC_DEMCR_TRCENA (1 << 24)

		/* Bit mask for NOPRFCNT bit in DWT_CTRL. If 1, DWT_EXCCNT is not supported */
		#define TRC_DWT_CTRL_NOPRFCNT (1 << 24)

		/* Bit mask for NOCYCCNT bit in DWT_CTRL. If 1, DWT_CYCCNT is not supported */
		#define TRC_DWT_CTRL_NOCYCCNT (1 << 25)

		/* Bit mask for EXCEVTENA_ bit in DWT_CTRL. Set to 1 to enable DWT_EXCCNT */
		#define TRC_DWT_CTRL_EXCEVTENA (1 << 18)

		/* Bit mask for EXCEVTENA_ bit in DWT_CTRL. Set to 1 to enable DWT_CYCCNT */
		#define TRC_DWT_CTRL_CYCCNTENA (1)

		#define TRC_PORT_SPECIFIC_INIT() xTraceHardwarePortInitCortexM()

		#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
		#define TRC_HWTC_COUNT TRC_REG_DWT_CYCCNT
		#define TRC_HWTC_PERIOD 0
		#define TRC_HWTC_DIVISOR 4
		#define TRC_HWTC_FREQ_HZ TRACE_CPU_CLOCK_HZ
		#define TRC_IRQ_PRIORITY_ORDER 0
	
	#else
			
		#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
		#define TRC_HWTC_COUNT (*((volatile uint32_t*)0xE000E018))
		#define TRC_HWTC_PERIOD ((*((volatile uint32_t*)0xE000E014)) + 1)
		#define TRC_HWTC_DIVISOR 4
		#define TRC_HWTC_FREQ_HZ TRACE_CPU_CLOCK_HZ
		#define TRC_IRQ_PRIORITY_ORDER 0
	
	#endif

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Renesas_RX600)
	#define TRACE_ALLOC_CRITICAL_SECTION() TraceBaseType_t __x_irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() { __x_irq_status = TRC_KERNEL_PORT_SET_INTERRUPT_MASK(); }
	#define TRACE_EXIT_CRITICAL_SECTION() { TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(__x_irq_status); }

	#include <iodefine.h>

	#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)	
		
		#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
		#define TRC_HWTC_COUNT (CMT0.CMCNT)
		
	#elif (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		
		/* Decreasing counters better for Tickless Idle? */
		#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
		#define TRC_HWTC_COUNT (CMT0.CMCOR - CMT0.CMCNT)
	
	#endif
	
	#define TRC_HWTC_PERIOD (CMT0.CMCOR + 1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1 
	
#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32)
	
	#define TRACE_ALLOC_CRITICAL_SECTION() TraceBaseType_t __x_irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() { __x_irq_status = TRC_KERNEL_PORT_SET_INTERRUPT_MASK(); }
	#define TRACE_EXIT_CRITICAL_SECTION() { TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(__x_irq_status); }
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TMR1)
	#define TRC_HWTC_PERIOD (PR1 + 1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48)

	#define TRC_RTIFRC0 *((uint32_t *)0xFFFFFC10)
	#define TRC_RTICOMP0 *((uint32_t *)0xFFFFFC50)
	#define TRC_RTIUDCP0 *((uint32_t *)0xFFFFFC54)
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TRC_RTIFRC0 - (TRC_RTICOMP0 - TRC_RTIUDCP0))
	#define TRC_HWTC_PERIOD (TRC_RTIUDCP0)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Atmel_AT91SAM7)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT ((uint32_t)(AT91C_BASE_PITC->PITC_PIIR & 0xFFFFF))
	#define TRC_HWTC_PERIOD ((uint32_t)(AT91C_BASE_PITC->PITC_PIMR + 1))
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Atmel_UC3A0)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO*/
	
	/* For Atmel AVR32 (AT32UC3A) */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT ((uint32_t)sysreg_read(AVR32_COUNT))
	#define TRC_HWTC_PERIOD ((uint32_t)(sysreg_read(AVR32_COMPARE) + 1))
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_NXP_LPC210X)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */
	
	/* Tested with LPC2106, but should work with most LPC21XX chips. */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT *((uint32_t *)0xE0004008 )
	#define TRC_HWTC_PERIOD *((uint32_t *)0xE0004018 )
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_MSP430)

	/* UNOFFICIAL PORT - NOT YET VERIFIED */
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT (TA0R)
	#define TRC_HWTC_PERIOD (((uint16_t)TACCR0)+1)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 1

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_PPC405)

	/* UNOFFICIAL PORT - NOT YET VERIFIED */
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT mfspr(0x3db)
	#define TRC_HWTC_PERIOD (TRACE_CPU_CLOCK_HZ / TRC_TICK_RATE_HZ)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_PPC440)

	/* UNOFFICIAL PORT */

	/* This should work with most PowerPC chips */

	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT mfspr(0x016)
	#define TRC_HWTC_PERIOD (TRACE_CPU_CLOCK_HZ / TRC_TICK_RATE_HZ)
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_MICROBLAZE)

	/* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */

	/* This should work with most Microblaze configurations.
	 * It uses the AXI Timer 0 - the tick interrupt source.
	 * If an AXI Timer 0 peripheral is available on your hardware platform, no modifications are required.
	 */
	#include <xtmrctr_l.h>
	
	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT XTmrCtr_GetTimerCounterReg( XPAR_TMRCTR_0_BASEADDR, 0 )
 	#define TRC_HWTC_PERIOD (XTmrCtr_GetLoadReg( XPAR_TMRCTR_0_BASEADDR, 0) + 1)
	#define TRC_HWTC_DIVISOR 16
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XILINX_ZyncUltraScaleR5)

	extern int cortex_a9_r5_enter_critical(void);
	extern void cortex_a9_r5_exit_critical(int irq_already_masked_at_enter);

	#define TRACE_ALLOC_CRITICAL_SECTION() uint32_t __irq_mask_status;

	#define TRACE_ENTER_CRITICAL_SECTION() { __irq_mask_status = cortex_a9_r5_enter_critical(); }

	#define TRACE_EXIT_CRITICAL_SECTION() { cortex_a9_r5_exit_critical(__irq_mask_status); }

	#include <xttcps_hw.h>

	#define TRC_HWTC_TYPE  TRC_OS_TIMER_INCR
	#define TRC_HWTC_COUNT  (*(volatile uint32_t *)(configTIMER_BASEADDR + XTTCPS_COUNT_VALUE_OFFSET))
	#define TRC_HWTC_PERIOD  (*(volatile uint32_t *)(configTIMER_BASEADDR + XTTCPS_INTERVAL_VAL_OFFSET))
	#define TRC_HWTC_DIVISOR  16
	#define TRC_HWTC_FREQ_HZ  (TRC_HWTC_PERIOD * TRC_TICK_RATE_HZ)
	#define TRC_IRQ_PRIORITY_ORDER  0

	#ifdef __GNUC__
	/* For Arm Cortex-A and Cortex-R in general. */
	static inline uint32_t prvGetCPSR(void)
	{
		unsigned long ret;
		/* GCC-style assembly for getting the CPSR/APSR register, where the system execution mode is found. */
		asm volatile (" mrs  %0, cpsr" : "=r" (ret) : /* no inputs */  );
		return ret;
	}
	#else
		#error "Only GCC Supported!"
	#endif

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Altera_NiosII)

    /* OFFICIAL PORT */

	#include <system.h>
	#include <altera_avalon_timer_regs.h>
	#include <sys/alt_irq.h>
	
	#define TRACE_ALLOC_CRITICAL_SECTION() alt_irq_context __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION(){__irq_status = alt_irq_disable_all();}
	#define TRACE_EXIT_CRITICAL_SECTION() {alt_irq_enable_all(__irq_status);}

	#define NOT_SET 1

	/* The base address for the sustem timer set.
	 * The name user for the system timer can be found in the BSP editor.
	 * If the name of the timer is sys_tmr SYSTEM_TIMER_BASE should be set to SYS_TMR_BASE.
	*/
	#define SYSTEM_TIMER_BASE NOT_SET

	#if (SYSTEM_TIMER == NOT_SET)
		#error "Set SYSTEM_TIMER_BASE to the timer base used for system ticks."
	#endif

 	static inline uint32_t altera_nios2_GetTimerSnapReg(void)
	{
		/* A processor can read the current counter value by first writing to either snapl or snaph to request a coherent snapshot of the counter,
		 * and then reading snapl and snaph for the full 32-bit value.
		*/
		IOWR_ALTERA_AVALON_TIMER_SNAPL(SYSTEM_TIMER_BASE, 0);
		return (IORD_ALTERA_AVALON_TIMER_SNAPH(SYSTEM_TIMER_BASE) << 16) | IORD_ALTERA_AVALON_TIMER_SNAPL(SYSTEM_TIMER_BASE);
	}

	#define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
	#define TRC_HWTC_COUNT altera_nios2_GetTimerSnapReg()
	#define TRC_HWTC_PERIOD (configCPU_CLOCK_HZ / configTICK_RATE_HZ )
	#define TRC_HWTC_DIVISOR 16
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
	#define TRC_IRQ_PRIORITY_ORDER 0  

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_CORTEX_A9)

	/**************************************************************************
	* This hardware port only supports FreeRTOS and the GCC compiler at the
	* moment, due to the implementation of critical sections (trcKernelPort.h).
	*
	* Assuming FreeRTOS is used:
	* 
    * For critical sections, this uses vTaskEnterCritical is when called from
	* task context and ulPortSetInterruptMask when called from ISR context.
	* Thus, it does not disable all ISRs. This means that the trace recorder
	* can only be called from ISRs with priority less or equal to 
	* configMAX_API_CALL_INTERRUPT_PRIORITY (like FreeRTOS fromISR functions).
	*
    * This hardware port has been tested on it a Xilinx Zync 7000 (Cortex-A9),
	* but should work with all Cortex-A and R processors assuming that
	* TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS is set accordingly.	
	**************************************************************************/

	extern int cortex_a9_r5_enter_critical(void);
	extern void cortex_a9_r5_exit_critical(int irq_already_masked_at_enter);

	#define TRACE_ALLOC_CRITICAL_SECTION() uint32_t __irq_mask_status;

	#define TRACE_ENTER_CRITICAL_SECTION() { __irq_mask_status = cortex_a9_r5_enter_critical(); }

	#define TRACE_EXIT_CRITICAL_SECTION() { cortex_a9_r5_exit_critical(__irq_mask_status); }
	
	/* INPUT YOUR PERIPHERAL BASE ADDRESS HERE (0xF8F00000 for Xilinx Zynq 7000)*/
	#define TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS	0
	
	#if (TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS == 0)
		#error "Please specify TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS."
	#endif

	#define TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET	0x0600
	#define TRC_CA9_MPCORE_PRIVCTR_PERIOD_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x00))
	#define TRC_CA9_MPCORE_PRIVCTR_COUNTER_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x04))
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_REG	(*(volatile uint32_t*)(TRC_CA9_MPCORE_PERIPHERAL_BASE_ADDRESS + TRC_CA9_MPCORE_PRIVATE_MEMORY_OFFSET + 0x08))
	
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_MASK    0x0000FF00
	#define TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_SHIFT   8
	#define TRC_CA9_MPCORE_PRIVCTR_PRESCALER        (((TRC_CA9_MPCORE_PRIVCTR_CONTROL_REG & TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_MASK) >> TRC_CA9_MPCORE_PRIVCTR_CONTROL_PRESCALER_SHIFT) + 1)

    #define TRC_HWTC_TYPE                           TRC_OS_TIMER_DECR
    #define TRC_HWTC_COUNT                          TRC_CA9_MPCORE_PRIVCTR_COUNTER_REG
    #define TRC_HWTC_PERIOD                         (TRC_CA9_MPCORE_PRIVCTR_PERIOD_REG + 1)

    /****************************************************************************************
	NOTE: The private timer ticks with a very high frequency (half the core-clock usually), 
	depending on the prescaler used. If a low prescaler is used, the number of HW ticks between
	the trace events gets large, and thereby inefficient to store (sometimes extra events are
	needed). To improve efficiency, you may use the TRC_HWTC_DIVISOR as an additional prescaler.
    *****************************************************************************************/	
	#define TRC_HWTC_DIVISOR 1
	
	#define TRC_HWTC_FREQ_HZ (TRC_TICK_RATE_HZ * TRC_HWTC_PERIOD)
    #define TRC_IRQ_PRIORITY_ORDER 0

	#ifdef __GNUC__
	/* For Arm Cortex-A and Cortex-R in general. */
	static inline uint32_t prvGetCPSR(void)
	{
		unsigned long ret;
		/* GCC-style assembly for getting the CPSR/APSR register, where the system execution mode is found. */
		asm volatile (" mrs  %0, cpsr" : "=r" (ret) : /* no inputs */  );
		return ret;
	}
	#else
		#error "Only GCC Supported!"
	#endif

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ZEPHYR)
	#define TRACE_ALLOC_CRITICAL_SECTION() int key;
	#define TRACE_ENTER_CRITICAL_SECTION() { key = irq_lock(); }
	#define TRACE_EXIT_CRITICAL_SECTION() { irq_unlock(key); }
	
	#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
	#define TRC_HWTC_COUNT k_cycle_get_32()
	#define TRC_HWTC_PERIOD (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC / CONFIG_SYS_CLOCK_TICKS_PER_SEC)
	#define TRC_HWTC_DIVISOR 4
	#define TRC_HWTC_FREQ_HZ CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC
	#define TRC_IRQ_PRIORITY_ORDER 0 // Lower IRQ priority values are more significant

	#define TRC_PORT_SPECIFIC_INIT()

#elif ((TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XTensa_LX6) || (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XTensa_LX7))
	/**
	 * @note	When running with SMP FreeRTOS we cannot use the CCOUNT register for timestamping,
	 * 			instead we use the external 40MHz timer for synchronized timestamping between the cores.
	 */
	#if CONFIG_FREERTOS_UNICORE == 1
		#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
		#define TRC_HWTC_COUNT ({ unsigned int __ccount; 			\
			__asm__ __volatile__("rsr.ccount %0" : "=a"(__ccount)); \
			__ccount; })
#ifdef CONFIG_IDF_TARGET_ESP32
		#define TRC_HWTC_FREQ_HZ (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
		#define TRC_HWTC_FREQ_HZ (CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ * 1000000)
#else
		#error "Invalid IDF target, check your sdkconfig."
#endif
		#define TRC_HWTC_PERIOD 0
		#define TRC_HWTC_DIVISOR 4
		#define TRC_IRQ_PRIORITY_ORDER 0
	#else
		/**
		 * @brief 	Fetch core agnostic timestamp using the external 40MHz timer. This is used by tracerecorder
		 * 			when running with both cores.
		 *
		 * @return 	Ticks since the timer started
		 */
		uint32_t prvGetSMPTimestamp();

		#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
		#define TRC_HWTC_COUNT prvGetSMPTimestamp()
		#define TRC_HWTC_FREQ_HZ 40000000
		#define TRC_HWTC_PERIOD 0
		#define TRC_HWTC_DIVISOR 4
		#define TRC_IRQ_PRIORITY_ORDER 0
	#endif

	#if !defined(TRC_HWTC_FREQ_HZ)
		#error "The XTensa LX6/LX7 trace hardware clock frequency is not defined."
	#endif

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_RISCV_RV32I)
	#define TRACE_ALLOC_CRITICAL_SECTION() unsigned int __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() __asm__ __volatile__("csrr %0, mstatus	\n\t"	\
																"csrci mstatus, 8	\n\t"	\
																"andi %0, %0, 8		\n\t"	\
																: "=r"(__irq_status))
    #define TRACE_EXIT_CRITICAL_SECTION() __asm__ __volatile__("csrr a1, mstatus	\n\t"	\
    															"or %0, %0, a1		\n\t"	\
																"csrs mstatus, %0	\n\t"	\
																:							\
																: "r" (__irq_status)		\
																: "a1")
	#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
	#define TRC_HWTC_COUNT ({ unsigned int __count;			\
		__asm__ __volatile__("rdcycle %0" : "=r"(__count));	\
		__count; })
	#define TRC_HWTC_PERIOD 0
	#define TRC_HWTC_DIVISOR 1
	#define TRC_HWTC_FREQ_HZ 16000000
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XMOS_XCOREAI)
	#define TRC_PORT_SPECIFIC_INIT()
	#define TRC_HWTC_TYPE TRC_FREE_RUNNING_32BIT_INCR
	#define TRC_HWTC_COUNT xscope_gettime()
	#define TRC_HWTC_PERIOD (configCPU_CLOCK_HZ / configTICK_RATE_HZ )
	#define TRC_HWTC_DIVISOR 4
	#define TRC_HWTC_FREQ_HZ 100000000
	#define TRC_IRQ_PRIORITY_ORDER 0

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_POWERPC_Z4)

    /* UNOFFICIAL PORT - NOT YET VERIFIED BY PERCEPIO */
	
	#define TRACE_ALLOC_CRITICAL_SECTION() TraceBaseType_t __x_irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() { __x_irq_status = TRC_KERNEL_PORT_SET_INTERRUPT_MASK(); }
	#define TRACE_EXIT_CRITICAL_SECTION() { TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(__x_irq_status); }

    #define TRC_HWTC_TYPE TRC_OS_TIMER_DECR
    //#define HWTC_COUNT_DIRECTION DIRECTION_DECREMENTING
    #define TRC_HWTC_COUNT PIT.TIMER[configTICK_PIT_CHANNEL].CVAL.R // must be the PIT channel used for the systick
    #define TRC_HWTC_PERIOD ((configPIT_CLOCK_HZ / configTICK_RATE_HZ) - 1U) // TODO FIXME or maybe not -1? what's the right "period" value?
    #define TRC_HWTC_FREQ_HZ configPIT_CLOCK_HZ
    #define TRC_HWTC_DIVISOR 1
    #define TRC_IRQ_PRIORITY_ORDER 1 // higher IRQ priority values are more significant

#elif (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_APPLICATION_DEFINED)

	#if !( defined (TRC_HWTC_TYPE) && defined (TRC_HWTC_COUNT) && defined (TRC_HWTC_PERIOD) && defined (TRC_HWTC_FREQ_HZ) && defined (TRC_IRQ_PRIORITY_ORDER) )
		#error "The hardware port is not completely defined!"
	#endif

#elif (TRC_CFG_HARDWARE_PORT != TRC_HARDWARE_PORT_NOT_SET)

	#error "TRC_CFG_HARDWARE_PORT had unsupported value!"
	#define TRC_CFG_HARDWARE_PORT TRC_HARDWARE_PORT_NOT_SET

#endif

#ifndef TRC_HWTC_DIVISOR
	#define TRC_HWTC_DIVISOR 1
#endif

#ifndef TRC_PORT_SPECIFIC_INIT
	#define TRC_PORT_SPECIFIC_INIT() 
#endif

/* If Win32 port */
#ifdef WIN32

	#undef _WIN32_WINNT
	#define _WIN32_WINNT 0x0600

	/* Standard includes. */
	#include <stdio.h>
	#include <windows.h>
	#include <direct.h>

    /***************************************************************************
    * The Win32 port by default saves the trace to file and then kills the
    * program when the recorder is stopped, to facilitate quick, simple tests
    * of the recorder.
    ***************************************************************************/
	#define WIN32_PORT_SAVE_WHEN_STOPPED 1
	#define WIN32_PORT_EXIT_WHEN_STOPPED 1

#endif

#if (TRC_CFG_HARDWARE_PORT != TRC_HARDWARE_PORT_NOT_SET)
	
	#ifndef TRC_HWTC_TYPE
	#error "TRC_HWTC_TYPE is not set!"
	#endif

	#ifndef TRC_HWTC_COUNT
	#error "TRC_HWTC_COUNT is not set!"
	#endif

	#ifndef TRC_HWTC_PERIOD
	#error "TRC_HWTC_PERIOD is not set!"
	#endif

	#ifndef TRC_HWTC_DIVISOR
	#error "TRC_HWTC_DIVISOR is not set!"
	#endif

	#ifndef TRC_IRQ_PRIORITY_ORDER
	#error "TRC_IRQ_PRIORITY_ORDER is not set!"
	#elif (TRC_IRQ_PRIORITY_ORDER != 0) && (TRC_IRQ_PRIORITY_ORDER != 1)
	#error "TRC_IRQ_PRIORITY_ORDER has bad value!"
	#endif

	#if (TRC_HWTC_DIVISOR < 1)
	#error "TRC_HWTC_DIVISOR must be a non-zero positive value!"
	#endif
	
	#ifndef TRC_HWTC_FREQ_HZ 
	#error "TRC_HWTC_FREQ_HZ not defined!"
	#endif
	
#endif

#ifndef TRACE_ALLOC_CRITICAL_SECTION
#define TRACE_ALLOC_CRITICAL_SECTION() TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION()
#endif
#ifndef TRACE_ENTER_CRITICAL_SECTION
	#define TRACE_ENTER_CRITICAL_SECTION() TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION()
#endif
#ifndef TRACE_EXIT_CRITICAL_SECTION
#define TRACE_EXIT_CRITICAL_SECTION() TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION()
#endif

#endif /*TRC_SNAPSHOT_HARDWARE_PORT_H*/
