/*!
 * Copyright (C) 2025 JongHoon Shim <shim9532@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/*==== FILE DESCRIPTION =====================================================*/

/*!
 * @file    linsys_defender.c
 * @author  JongHoon Shim (shim9532@gmail.com)
 * @date    2025.03.23
 * @brief   linsys_defender 메인 소스 파일
*/

/*==== INCLUDES =============================================================*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ftrace.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/kprobes.h>
#include "common/config.h"

/*==== MODULE DESCRIPTION ===================================================*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JongHoon Shim");
MODULE_DESCRIPTION("linsys_defender with syscall hooks");
MODULE_VERSION("1.0.0");

/*==== LOCAL DEFINES ========================================================*/

#ifndef CONFIG_X86_64
/* x86_64 아키텍처 외 다른 아키텍처에서 컴파일 방지 */
#error Currently only x86_64 architecture is supported
#endif

/* 
 * ftrace 후킹 시 함수 주소에 직접 후킹을 걸 것인지 함수 진입 직후의 `fentry` 콜백 위치에
 * 후킹을 걸 것인지 설정하는 매크로.
 * 컴파일 시 `CONFIG_FTRACE` 커널 설정이 활성화되면, 함수의 맨 앞에 NOP(No operation)
 * 형태로 `fentry`라는 추적 포인트를 삽입.
 * 나중에 이 위치에 런타임에서 동적으로 후킹을 걸 수 있음.
*/
#define USE_FENTRY_OFFSET   1

#if !USE_FENTRY_OFFSET
/* 
 * USE_FENTRY_OFFSET == 0이면 GCC 컴파일러에서 `sibling call optimization`을 하지 않도록 함.
 * 해당 최적화가 있으면 후킹하려는 함수가 내부적으로 최적화되어 의도한 지점에 후킹이 안될 수 있음.
 * (재귀적으로 무한 호출될 수 있음)
*/
#pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
#define FTRACE_OPS_FL_RECURSION FTRACE_OPS_FL_RECURSION_SAFE
#define ftrace_regs pt_regs
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
/* `pt_regs`기반 시스템 콜 핸들러 사용 여부 매크로 */
#define PTREGS_SYSCALL_STUBS 1
#endif

#if PTREGS_SYSCALL_STUBS
#define SYSCALL_NAME(_name) ("__x64_sys_" _name)
#else
#define SYSCALL_NAME(_name) (_name)
#endif

/* 시스템 콜 후킹 정보 저장 매크로 */
#define HOOK(_name, _function, _original)   \
    {                                       \
        .name = SYSCALL_NAME(_name),        \
        .function = (_function),            \
        .original = (_original),            \
    }

/*==== LOCAL STRUCTS ========================================================*/

/* ftrace 사용 시스템 콜 후킹 정보 저장 구조체 */
struct ftrace_hook {
    const char *name;           /* 후킹할 시스템콜 이름 */
    void *function;             /* 시스템 콜 후킹 함수 포인터 */
    void *original;             /* 원본 시스템 콜 함수 포인터 */
    unsigned long address;      /* 후킹 대상 함수 주소 */
    struct ftrace_ops ops;      /* ftrace 후킹 설정에 사용되는 구조체 (func, flags, private 등을 설정) */
};

/*==== LOCAL FUNCTIONS DECLARATION ==========================================*/

static int install_hooks(void);
static void remove_hooks(void);
static int install_hook(struct ftrace_hook *hook);
static void remove_hook(struct ftrace_hook *hook);
static int resolve_hook_address(struct ftrace_hook *hook);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
static __always_inline struct pt_regs *ftrace_get_regs(struct ftrace_regs *fregs);
#endif
static void notrace ftrace_thunk(unsigned long ip, unsigned long parent_ip,
    struct ftrace_ops *ops, struct ftrace_regs *fregs);

#if PTREGS_SYSCALL_STUBS
static asmlinkage long linsys_openat(const struct pt_regs *regs);
static asmlinkage long linsys_open(const struct pt_regs *regs);
static asmlinkage long linsys_read(const struct pt_regs *regs);
static asmlinkage long linsys_write(const struct pt_regs *regs);
#endif

/*==== LOCAL VARIABLES ======================================================*/

#if PTREGS_SYSCALL_STUBS
/* 원본 시스템 콜 함수 포인터 */
static asmlinkage long (*org_openat)(const struct pt_regs *);
static asmlinkage long (*org_open)(const struct pt_regs *);
static asmlinkage long (*org_read)(const struct pt_regs *);
static asmlinkage long (*org_write)(const struct pt_regs *);
#endif

config_t g_config;

/* LKM 파라미터 정의 */
static bool debug_mode = false;

/* 후킹할 시스템 콜 정보 저장 */
static struct ftrace_hook hooks[] = {
    HOOK("openat",  linsys_openat,  &org_openat),
    HOOK("open",    linsys_open,    &org_open),
    HOOK("read",    linsys_read,    &org_read),
    HOOK("write",   linsys_write,   &org_write),
};

/*==== FUNCTIONS ============================================================*/

/* LKM 파라미터 값 설정 */
module_param(debug_mode, bool, 0644);
MODULE_PARM_DESC(debug_mode, "Debug mode enabled");

/*!
 * @brief   linsys_defender 커널 모듈 가동
*/
static int __init linsys_defender_init(void)
{
    int err = 0;

    g_config.debug_mode = debug_mode;

    /* 시스템 콜 후킹 설정 */
    err = install_hooks();
    if (0 != err) {
        return err;
    }

    pr_info("[%s] Module loaded successfully (mode: %s)\n", MODULE_NAME,
        (g_config.debug_mode) ? "debug":"normal");

    return 0;
}

/*!
 * @brief   linsys_defender 커널 모듈 종료
*/
static void __exit linsys_defender_exit(void)
{
    /* 시스템 콜 후킹 해제 */
    remove_hooks();
    
    pr_info("[%s] Module unloaded successfully", MODULE_NAME);
}

/* 모듈이 로드될 때 실행될 함수 등록 */
module_init(linsys_defender_init);
/* 모듈이 언로드될 때 실행될 함수 등록 */
module_exit(linsys_defender_exit);

/*!
 * @brief   hooks 구조체 배열에 정의된 모든 시스템 콜 후킹 설정
 * @return  int: 성공(0), 실패(Non-zero)
*/
static int install_hooks(void)
{
    int err = 0, i = 0;

    /* hooks 구조체 배열에 정의된 모든 후킹을 순회 */
    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        /* 시스템 콜 후킹 설정 */
        err = install_hook(&hooks[i]);
        if (0 != err) {
            goto error;
        }
    }

    return 0;

error:
    while (i-- > 0) {
        /* 시스템 콜 후킹 설정 실패 시 지금까지 설정한 후킹들을 역순으로 제거 */
        remove_hook(&hooks[i]);
    }

    return err;
}

/*!
 * @brief   설정한 시스템 콜 후킹들을 제거
*/
static void remove_hooks(void)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZE(hooks); ++i) {
        remove_hook(&hooks[i]);
    }
}

/*!
 * @brief   시스템 콜 후킹 설정
 * @param   hook: 후킹할 시스템 콜 정보 구조체
 * @return  int: 성공(0), 실패(Non-zero)
*/
static int install_hook(struct ftrace_hook *hook)
{
    int err = 0;

    /* 후킹할 시스템 콜 함수 주소를 찾아서 ftrace_hook 구조체에 저장 */
    err = resolve_hook_address(hook);
    if (0 != err) {
        return err;
    }

    /* ftrace에 등록할 핸들러 함수 설정 */
    hook->ops.func = ftrace_thunk;
    
    /* 
     * ftrace 동작 방식 지정 플래그 설정.
     * - SAVE_REGS: 레지스터 저장 (pt_regs 사용 가능하게 함)
     * - RECURSION: 재귀 후킹 방지
     * - IPMODIFY: 함수 호출 주소(IP)를 변경 가능 (실제 후킹 핵심)
     */
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | 
                      FTRACE_OPS_FL_RECURSION |
                      FTRACE_OPS_FL_IPMODIFY;

    /* ftrace에 후킹할 함수 주소를 필터로 등록 */
    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (0 != err) {
        pr_debug("ftrace_set_filter_ip() failed: %d\n", err);
		return err;
    }

    /* ftrace 시스템에 후킹 함수(ops)를 실제로 등록 */
    err = register_ftrace_function(&hook->ops);
	if (0 != err) {
		pr_debug("register_ftrace_function() failed: %d\n", err);
        /* 실패 시 이전에 등록한 필터 주소를 해제하여 정리 */
		ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
		return err;
	}

    return 0;
}

/*!
 * @brief   ftrace를 통해 설정된 시스템 콜 후킹을 해제하는 함수
 * @param   hook: 설정된 시스템 콜 후킹 정보 구조체
*/
static void remove_hook(struct ftrace_hook *hook)
{
	int err = 0;

    /* ftrace에서 등록한 후킹 함수(ops)를 해제 */
	err = unregister_ftrace_function(&hook->ops);
	if (0 != err) {
		pr_debug("unregister_ftrace_function() failed: %d\n", err);
	}

    /* ftrace 필터에서 이 후킹 주소를 제거 (필수 정리 작업) */
	err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
	if (0 != err) {
		pr_debug("ftrace_set_filter_ip() failed: %d\n", err);
	}
}

/*!
 * @brief   후킹할 시스템 콜 함수 주소를 찾아서 ftrace_hook 구조체에 저장
 * @param   hook: 후킹 대상 함수 정보 저장 구조체
 * @return  int: 성공(0), 실패(-ENOENT)
*/
static int resolve_hook_address(struct ftrace_hook *hook)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
    struct kprobe kp = {
        .symbol_name = hook->name
    };

    /* kprobe를 이용하여 시스템콜 함수 주소를 찾음 */
    if (register_krobe(&kp) < 0) {
        return -ENOENT;
    }
    hook->address = (unsigned long)kp.addr;
    unregister_krpobe(&kp);
#else
    /* 시스템콜 함수 주소를 찾음 */
    hook->address = kallsyms_lookup_name(hook->name);
    if (0 == hook->address) {
        return -ENOENT;
    }
#endif

    /* 시스템콜 원본 함수 주소 저장 */
#if USE_FENTRY_OFFSET
    /* USE_FENTRY_OFFSET == 1이면 함수의 시작 부분을 건너뛰도록 함 */
	*((unsigned long*) hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
    /* 
     * USE_FENTRY_OFFSET == 0이면 함수의 시작 지점부터 후킹.
     * 재귀적으로 무한 호출될 수 있으니 주의 필요.
     */
	*((unsigned long*) hook->original) = hook->address;
#endif

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
/*!
 * @brief   `ftrace_regs` 포인터를 `pt_regs` 포인터로 변환해주는 함수
 * @param   fregs: `pt_regs`로 변환할 ftrace_regs 구조체
 * @return  struct pt_regs: `pt_regs`로 변환된 ftrace_regs 구조체
*/
static __always_inline struct pt_regs *ftrace_get_regs(struct ftrace_regs *fregs)
{
    /* 사실상 fregs == pt_regs* */
	return fregs;
}
#endif

/*!
 * @brief   후킹된 함수가 호출될 때 ftrace에 의해 호출되는 중간 함수
 * @param   ip: 원래 호출되려던 함수 주소
 * @param   parent_ip: 이 함수를 호출한 상위 함수의 주소 (caller)
 * @param   ops: 현재 ftrace 후킹 설정 정보
 * @param   fregs: 레지스터 상태 정보
*/
static void notrace ftrace_thunk(unsigned long ip, unsigned long parent_ip,
    struct ftrace_ops *ops, struct ftrace_regs *fregs)
{   
    /* ftrace_regs 구조체를 pt_regs로 변환 */
    struct pt_regs *regs = ftrace_get_regs(fregs);
    /* 현재 ops 포인터가 포함된 struct ftrace_hook 구조체의 주소를 얻음 */
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

#if USE_FENTRY_OFFSET
    /* fentry 방식 사용 시 함수 진입 지점에서 이미 호출되었으므로 무조건 후킹 함수로 점프시킴 */
	regs->ip = (unsigned long)hook->function;
#else
    /* 
     * fentry 방식이 아닐 경우, parent_ip를 확인해서 커널 외부(모듈 외부)에서 호출됐는지 체크.
     * 현재 호출이 외부(THIS_MODULE 아님)에서 온 경우에만 후킹을 적용.
    */
	if (!within_module(parent_ip, THIS_MODULE)) {
        /* 원래 호출하려던 함수 주소(ip)를 후킹 함수 주소로 덮어씀 */
		regs->ip = (unsigned long)hook->function;
    }
#endif
}

#if PTREGS_SYSCALL_STUBS
/*!
 * @brief   `openat` 시스템 콜 후킹 함수
 * @param   regs: 레지스터 상태를 담는 구조체 (시스템 콜 인자값)
*/
static asmlinkage long linsys_openat(const struct pt_regs *regs)
{
    return org_openat(regs);
}

static asmlinkage long linsys_open(const struct pt_regs *regs)
{
    return org_open(regs);
}

static asmlinkage long linsys_read(const struct pt_regs *regs)
{
    return org_read(regs);
}

static asmlinkage long linsys_write(const struct pt_regs *regs)
{
    return org_write(regs);
}
#endif
