/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    templates/nilcore.h
 * @brief   Port macros and structures.
 *
 * @addtogroup NIL_CORE
 * @{
 */

#ifndef _NILCORE_H_
#define _NILCORE_H_

#include "intc.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

/**
 * @name    Architecture and Compiler
 * @{
 */
/**
 * @brief   Macro defining an PPC architecture.
 */
#define PORT_ARCHITECTURE_PPC

/**
 * @brief   Macro defining the specific PPC architecture.
 */
#define PORT_ARCHITECTURE_PPC_E200

/**
 * @brief   Name of the implemented architecture.
 */
#define PORT_ARCHITECTURE_NAME          "Power Architecture"

/**
 * @brief   Compiler name and version.
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
#define PORT_COMPILER_NAME              "GCC " __VERSION__

#else
#error "unsupported compiler"
#endif

/**
 * @brief   This port supports a realtime counter.
 */
#define PORT_SUPPORTS_RT                FALSE
/** @} */

/**
 * @name    E200 core variants
 * @{
 */
#define PPC_VARIANT_e200z0              200
#define PPC_VARIANT_e200z2              202
#define PPC_VARIANT_e200z3              203
#define PPC_VARIANT_e200z4              204
/** @} */

/* Inclusion of the PPC implementation specific parameters.*/
#include "ppcparams.h"
#include "vectors.h"

/*===========================================================================*/
/* Module pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   Per-thread stack overhead for interrupts servicing.
 * @details This constant is used in the calculation of the correct working
 *          area size.
 */
#if !defined(PORT_INT_REQUIRED_STACK) || defined(__DOXYGEN__)
#define PORT_INT_REQUIRED_STACK         256
#endif

/**
 * @brief   Enables an alternative timer implementation.
 * @details Usually the port uses a timer interface defined in the file
 *          @p nilcore_timer.h, if this option is enabled then the file
 *          @p nilcore_timer_alt.h is included instead.
 */
#if !defined(PORT_USE_ALT_TIMER)
#define PORT_USE_ALT_TIMER              FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if PPC_USE_VLE && !PPC_SUPPORTS_VLE
#error "the selected MCU does not support VLE instructions set"
#endif

#if !PPC_USE_VLE && !PPC_SUPPORTS_BOOKE
#error "the selected MCU does not support BookE instructions set"
#endif

/**
 * @brief   Name of the architecture variant.
 */
#if (PPC_VARIANT == PPC_VARIANT_e200z0) || defined(__DOXYGEN__)
#define PORT_CORE_VARIANT_NAME          "e200z0"
#elif PPC_VARIANT == PPC_VARIANT_e200z2
#define PORT_CORE_VARIANT_NAME          "e200z2"
#elif PPC_VARIANT == PPC_VARIANT_e200z3
#define PORT_CORE_VARIANT_NAME          "e200z3"
#elif PPC_VARIANT == PPC_VARIANT_e200z4
#define PORT_CORE_VARIANT_NAME          "e200z4"
#else
#error "unknown or unsupported PowerPC variant specified"
#endif

/**
 * @brief   Port-specific information string.
 */
#if PPC_USE_VLE
#define PORT_INFO                       "VLE mode"
#else
#define PORT_INFO                       "Book-E mode"
#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Type of stack and memory alignment enforcement.
 */
typedef uint64_t stkalign_t;

/**
 * @brief   Generic PPC register.
 */
typedef void *regppc_t;

/**
 * @brief   Mandatory part of a stack frame.
 */
struct port_eabi_frame {
  uint32_t      slink;          /**< Stack back link.                       */
  uint32_t      shole;          /**< Stack hole for LR storage.             */
};

/**
 * @brief   Interrupt saved context.
 * @details This structure represents the stack frame saved during a
 *          preemption-capable interrupt handler.
 * @note    R2 and R13 are not saved because those are assumed to be immutable
 *          during the system life cycle.
 */
struct port_extctx {
  struct port_eabi_frame frame;
  /* Start of the e_stmvsrrw frame (offset 8).*/
  regppc_t      pc;
  regppc_t      msr;
  /* Start of the e_stmvsprw frame (offset 16).*/
  regppc_t      cr;
  regppc_t      lr;
  regppc_t      ctr;
  regppc_t      xer;
  /* Start of the e_stmvgprw frame (offset 32).*/
  regppc_t      r0;
  regppc_t      r3;
  regppc_t      r4;
  regppc_t      r5;
  regppc_t      r6;
  regppc_t      r7;
  regppc_t      r8;
  regppc_t      r9;
  regppc_t      r10;
  regppc_t      r11;
  regppc_t      r12;
  regppc_t      padding;
 };

/**
 * @brief   System saved context.
 * @details This structure represents the inner stack frame during a context
 *          switching.
 * @note    R2 and R13 are not saved because those are assumed to be immutable
 *          during the system life cycle.
 * @note    LR is stored in the caller context so it is not present in this
 *          structure.
 */
struct port_intctx {
  regppc_t      cr;                 /* Part of it is not volatile...        */
  regppc_t      r14;
  regppc_t      r15;
  regppc_t      r16;
  regppc_t      r17;
  regppc_t      r18;
  regppc_t      r19;
  regppc_t      r20;
  regppc_t      r21;
  regppc_t      r22;
  regppc_t      r23;
  regppc_t      r24;
  regppc_t      r25;
  regppc_t      r26;
  regppc_t      r27;
  regppc_t      r28;
  regppc_t      r29;
  regppc_t      r30;
  regppc_t      r31;
  regppc_t      padding;
};

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Platform dependent thread stack setup.
 * @details This code usually setup the context switching frame represented
 *          by an @p port_intctx structure.
 */
#define PORT_SETUP_CONTEXT(tp, wend, pf, arg) {                             \
  uint8_t *sp = (uint8_t *)(wend) -                                         \
                           sizeof(struct port_eabi_frame);                  \
  ((struct port_eabi_frame *)sp)->slink = 0;                                \
  ((struct port_eabi_frame *)sp)->shole = (uint32_t)_port_thread_start;     \
  (tp)->ctxp = (struct port_intctx *)(sp - sizeof(struct port_intctx));     \
  (tp)->ctxp->r31 = (regppc_t)(arg);                                        \
  (tp)->ctxp->r30 = (regppc_t)(pf);                                         \
}

/**
 * @brief   Computes the thread working area global size.
 * @note    There is no need to perform alignments in this macro.
 */
#define PORT_WA_SIZE(n) (sizeof(struct port_intctx) +                       \
                         sizeof(struct port_extctx) +                       \
                         (n) + (PORT_INT_REQUIRED_STACK))

/**
 * @brief   IRQ prologue code.
 * @details This macro must be inserted at the start of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_PROLOGUE()

/**
 * @brief   IRQ epilogue code.
 * @details This macro must be inserted at the end of all IRQ handlers
 *          enabled to invoke system APIs.
 */
#define PORT_IRQ_EPILOGUE()

/**
 * @brief   IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#define PORT_IRQ_HANDLER(id) void id(void)

/**
 * @brief   Fast IRQ handler function declaration.
 * @note    @p id can be a function name or a vector number depending on the
 *          port implementation.
 */
#define PORT_FAST_IRQ_HANDLER(id) void id(void)

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_PRIORITY(n)                                       \
  (((n) >= 0U) && ((n) < INTC_PRIORITY_LEVELS))

/**
 * @brief   Priority level verification macro.
 */
#define PORT_IRQ_IS_VALID_KERNEL_PRIORITY(n)                                \
    (((n) >= 0U) && ((n) < INTC_PRIORITY_LEVELS))

/**
 * @brief   Performs a context switch between two threads.
 * @details This is the most critical code in any port, this function
 *          is responsible for the context switch between 2 threads.
 * @note    The implementation of this code affects <b>directly</b> the context
 *          switch performance so optimize here as much as you can.
 *
 * @param[in] ntp       the thread to be switched in
 * @param[in] otp       the thread to be switched out
 */
#if !NIL_CFG_ENABLE_STACK_CHECK || defined(__DOXYGEN__)
#define port_switch(ntp, otp) _port_switch(ntp, otp)
#else
#define port_switch(ntp, otp) {                                             \
  register struct port_intctx *sp asm ("%r1");                              \
  if ((stkalign_t *)(sp - 1) < otp->stklim)                                 \
    chSysHalt("stack overflow");                                            \
  _port_switch(ntp, otp);                                                   \
}
#endif

/**
 * @brief   Writes to a special register.
 *
 * @param[in] spr       special register number
 * @param[in] val       value to be written, must be an automatic variable
 */
#define port_write_spr(spr, val)                                            \
  asm volatile ("mtspr   %[p0], %[p1]" : : [p0] "n" (spr), [p1] "r" (val))

/**
 * @brief   Writes to a special register.
 *
 * @param[in] spr       special register number
 * @param[in] val       returned value, must be an automatic variable
 */
#define port_read_spr(spr, val)                                             \
  asm volatile ("mfspr   %[p0], %[p1]" : [p0] "=r" (val) : [p1] "n" (spr))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

#ifdef __cplusplus
extern "C" {
#endif
  void _port_switch(thread_t *ntp, thread_t *otp);
  void _port_thread_start(void);
#ifdef __cplusplus
}
#endif

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

/* The following code is not processed when the file is included from an
   asm module.*/
#if !defined(_FROM_ASM_)

/**
 * @brief   Port-related initialization code.
 */
static inline void port_init(void) {
  uint32_t n;
  unsigned i;

  /* Initializing the SPRG0 register to zero, it is required for interrupts
     handling.*/
  n = 0;
  port_write_spr(272, n);

#if PPC_SUPPORTS_IVORS
  /* The CPU supports IVOR registers, the kernel requires IVOR4 and IVOR10
     and the initialization is performed here.*/
  asm volatile ("li          %%r3, _IVOR4@l       \t\n"
                "mtIVOR4     %%r3                 \t\n"
                "li          %%r3, _IVOR10@l      \t\n"
                "mtIVOR10    %%r3" : : : "r3", "memory");
#endif

  /* INTC initialization, software vector mode, 4 bytes vectors, starting
     at priority 0.*/
  INTC_BCR = 0;
  for (i = 0; i < PPC_CORE_NUMBER; i++) {
    INTC_CPR(i)   = 0;
    INTC_IACKR(i) = (uint32_t)_vectors;
  }
}

/**
 * @brief   Returns a word encoding the current interrupts status.
 *
 * @return              The interrupts status.
 */
static inline syssts_t port_get_irq_status(void) {
  uint32_t sts;

  asm volatile ("mfmsr   %[p0]" : [p0] "=r" (sts) :);
  return sts;
}

/**
 * @brief   Checks the interrupt status.
 *
 * @param[in] sts       the interrupt status word
 *
 * @return              The interrupt status.
 * @retvel false        the word specified a disabled interrupts status.
 * @retvel true         the word specified an enabled interrupts status.
 */
static inline bool port_irq_enabled(syssts_t sts) {

  return (bool)((sts & (1 << 15)) != 0);
}

/**
 * @brief   Determines the current execution context.
 *
 * @return              The execution context.
 * @retval false        not running in ISR mode.
 * @retval true         running in ISR mode.
 */
static inline bool port_is_isr_context(void) {
  uint32_t sprg0;

  /* The SPRG0 register is increased before entering interrupt handlers and
     decreased at the end.*/
  port_read_spr(272, sprg0);
  return (bool)(sprg0 > 0);
}

/**
 * @brief   Kernel-lock action.
 */
static inline void port_lock(void) {

  asm volatile ("wrteei  0" : : : "memory");
}

/**
 * @brief   Kernel-unlock action.
 */
static inline void port_unlock(void) {

  asm volatile("wrteei  1" : : : "memory");
}

/**
 * @brief   Kernel-lock action from an interrupt handler.
 */
static inline void port_lock_from_isr(void) {

}

/**
 * @brief   Kernel-unlock action from an interrupt handler.
 */
static inline void port_unlock_from_isr(void) {

}

/**
 * @brief   Disables all the interrupt sources.
 */
static inline void port_disable(void) {

  asm volatile ("wrteei  0" : : : "memory");
}

/**
 * @brief   Disables the interrupt sources below kernel-level priority.
 */
static inline void port_suspend(void) {

  asm volatile ("wrteei  0" : : : "memory");
}

/**
 * @brief   Enables all the interrupt sources.
 */
static inline void port_enable(void) {

  asm volatile ("wrteei  1" : : : "memory");
}

/**
 * @brief   Enters an architecture-dependent IRQ-waiting mode.
 * @details The function is meant to return when an interrupt becomes pending.
 *          The simplest implementation is an empty function or macro but this
 *          would not take advantage of architecture-specific power saving
 *          modes.
 */
static inline void port_wait_for_interrupt(void) {

  asm volatile ("wait" : : : "memory");
}

/**
 * @brief   Returns the current value of the realtime counter.
 *
 * @return              The realtime counter value.
 */
static inline rtcnt_t port_rt_get_counter_value(void) {

  return 0;
}

#endif /* !defined(_FROM_ASM_) */

/*===========================================================================*/
/* Module late inclusions.                                                   */
/*===========================================================================*/

#if !defined(_FROM_ASM_)

#if NIL_CFG_ST_TIMEDELTA > 0
#if !PORT_USE_ALT_TIMER
#include "nilcore_timer.h"
#else /* PORT_USE_ALT_TIMER */
#include "nilcore_timer_alt.h"
#endif /* PORT_USE_ALT_TIMER */
#endif /* NIL_CFG_ST_TIMEDELTA > 0 */

#endif /* !defined(_FROM_ASM_) */

#endif /* _NILCORE_H_ */

/** @} */