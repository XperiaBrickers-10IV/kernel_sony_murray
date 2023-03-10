/* SPDX-License-Identifier: GPL-2.0 */
/*
 * NOTE: This file has been modified by Sony Corporation.
 * Modifications are Copyright 2021 Sony Corporation,
 * and licensed under the license of the file.
 */
/*
 * Copyright (C) 1994 Linus Torvalds
 *
 * Pentium III FXSR, SSE support
 * General FPU state handling cleanups
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 * x86-64 work by Andi Kleen 2002
 */

#ifndef _ASM_X86_FPU_API_H
#define _ASM_X86_FPU_API_H
#include <linux/bottom_half.h>

/*
 * Use kernel_fpu_begin/end() if you intend to use FPU in kernel context. It
 * disables preemption so be careful if you intend to use it for long periods
 * of time.
 * If you intend to use the FPU in irq/softirq you need to check first with
 * irq_fpu_usable() if it is possible.
 */

/* Kernel FPU states to initialize in kernel_fpu_begin_mask() */
#define KFPU_387	_BITUL(0)	/* 387 state will be initialized */
#define KFPU_MXCSR	_BITUL(1)	/* MXCSR will be initialized */

extern void kernel_fpu_begin_mask(unsigned int kfpu_mask);
extern void kernel_fpu_end(void);
extern bool irq_fpu_usable(void);
extern void fpregs_mark_activate(void);

/* Code that is unaware of kernel_fpu_begin_mask() can use this */
static inline void kernel_fpu_begin(void)
{
	kernel_fpu_begin_mask(KFPU_387 | KFPU_MXCSR);
}

/*
 * Use fpregs_lock() while editing CPU's FPU registers or fpu->state.
 * A context switch will (and softirq might) save CPU's FPU registers to
 * fpu->state and set TIF_NEED_FPU_LOAD leaving CPU's FPU registers in
 * a random state.
 */
static inline void fpregs_lock(void)
{
	preempt_disable();
	local_bh_disable();
}

static inline void fpregs_unlock(void)
{
	local_bh_enable();
	preempt_enable();
}

#ifdef CONFIG_X86_DEBUG_FPU
extern void fpregs_assert_state_consistent(void);
#else
static inline void fpregs_assert_state_consistent(void) { }
#endif

/*
 * Load the task FPU state before returning to userspace.
 */
extern void switch_fpu_return(void);

/*
 * Query the presence of one or more xfeatures. Works on any legacy CPU as well.
 *
 * If 'feature_name' is set then put a human-readable description of
 * the feature there as well - this can be used to print error (or success)
 * messages.
 */
extern int cpu_has_xfeatures(u64 xfeatures_mask, const char **feature_name);

#endif /* _ASM_X86_FPU_API_H */
