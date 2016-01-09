/*
 *  TOPPERS ATK2
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Automotive Kernel Version 2
 *
 *  Copyright (C) 2011-2015 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2011-2015 by FUJI SOFT INCORPORATED, JAPAN
 *  Copyright (C) 2011-2013 by Spansion LLC, USA
 *  Copyright (C) 2011-2015 by NEC Communication Systems, Ltd., JAPAN
 *  Copyright (C) 2011-2015 by Panasonic Advanced Technology Development Co., Ltd., JAPAN
 *  Copyright (C) 2011-2014 by Renesas Electronics Corporation, JAPAN
 *  Copyright (C) 2011-2015 by Sunny Giken Inc., JAPAN
 *  Copyright (C) 2011-2015 by TOSHIBA CORPORATION, JAPAN
 *  Copyright (C) 2011-2015 by Witz Corporation
 *  Copyright (C) 2014-2015 by AISIN COMCRUISE Co., Ltd., JAPAN
 *  Copyright (C) 2014-2015 by eSOL Co.,Ltd., JAPAN
 *  Copyright (C) 2014-2015 by SCSK Corporation, JAPAN
 *  Copyright (C) 2015 by SUZUKI MOTOR CORPORATION
 *  Copyright (C) 2016 by Fan Wang(parai@foxmail.com), China
 * 
 * The above copyright holders grant permission gratis to use,
 * duplicate, modify, or redistribute (hereafter called use) this
 * software (including the one made by modifying this software),
 * provided that the following four conditions (1) through (4) are
 * satisfied.
 * 
 * (1) When this software is used in the form of source code, the above
 *    copyright notice, this use conditions, and the disclaimer shown
 *    below must be retained in the source code without modification.
 *
 * (2) When this software is redistributed in the forms usable for the
 *    development of other software, such as in library form, the above
 *    copyright notice, this use conditions, and the disclaimer shown
 *    below must be shown without modification in the document provided
 *    with the redistributed software, such as the user manual.
 *
 * (3) When this software is redistributed in the forms unusable for the
 *    development of other software, such as the case when the software
 *    is embedded in a piece of equipment, either of the following two
 *    conditions must be satisfied:
 *
 *  (a) The above copyright notice, this use conditions, and the
 *      disclaimer shown below must be shown without modification in
 *      the document provided with the redistributed software, such as
 *      the user manual.
 *
 *  (b) How the software is to be redistributed must be reported to the
 *      TOPPERS Project according to the procedure described
 *      separately.
 *
 * (4) The above copyright holders and the TOPPERS Project are exempt
 *    from responsibility for any type of damage directly or indirectly
 *    caused from the use of this software and are indemnified by any
 *    users or end users of this software from any and all causes of
 *    action whatsoever.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
 * THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
 * TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
 * INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
 *
 *  $Id: interrupt_manage.c 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		割込み管理モジュール
 */

#include "kernel_impl.h"
#include "check.h"
#include "interrupt.h"
#include "timingprotection.h"
#include "task.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_DISINT_ENTER
#define LOG_DISINT_ENTER()
#endif /* LOG_DISINT_ENTER */

#ifndef LOG_DISINT_LEAVE
#define LOG_DISINT_LEAVE()
#endif /* LOG_DISINT_LEAVE */

#ifndef LOG_ENAINT_ENTER
#define LOG_ENAINT_ENTER()
#endif /* LOG_ENAINT_ENTER */

#ifndef LOG_ENAINT_LEAVE
#define LOG_ENAINT_LEAVE()
#endif /* LOG_ENAINT_LEAVE */

#ifndef LOG_SUSALL_ENTER
#define LOG_SUSALL_ENTER()
#endif /* LOG_SUSALL_ENTER */

#ifndef LOG_SUSALL_LEAVE
#define LOG_SUSALL_LEAVE()
#endif /* LOG_SUSALL_LEAVE */

#ifndef LOG_RSMALL_ENTER
#define LOG_RSMALL_ENTER()
#endif /* LOG_RSMALL_ENTER */

#ifndef LOG_RSMALL_LEAVE
#define LOG_RSMALL_LEAVE()
#endif /* LOG_RSMALL_LEAVE */

#ifndef LOG_SUSOSI_ENTER
#define LOG_SUSOSI_ENTER()
#endif /* LOG_SUSOSI_ENTER */

#ifndef LOG_SUSOSI_LEAVE
#define LOG_SUSOSI_LEAVE()
#endif /* LOG_SUSOSI_LEAVE */

#ifndef LOG_RSMOSI_ENTER
#define LOG_RSMOSI_ENTER()
#endif /* LOG_RSMOSI_ENTER */

#ifndef LOG_RSMOSI_LEAVE
#define LOG_RSMOSI_LEAVE()
#endif /* LOG_RSMOSI_LEAVE */

#ifndef LOG_GETISRID_ENTER
#define LOG_GETISRID_ENTER()
#endif /* LOG_GETISRID_ENTER */

#ifndef LOG_GETISRID_LEAVE
#define LOG_GETISRID_LEAVE(ercd)
#endif /* LOG_GETISRID_LEAVE */

#ifndef LOG_DISINTSRC_ENTER
#define LOG_DISINTSRC_ENTER(isrid)
#endif /* LOG_DISINTSRC_ENTER */

#ifndef LOG_DISINTSRC_LEAVE
#define	LOG_DISINTSRC_LEAVE(ercd)
#endif /* LOG_DISINTSRC_LEAVE */

#ifndef LOG_ENAINTSRC_ENTER
#define LOG_ENAINTSRC_ENTER(isrid)
#endif /* LOG_ENAINTSRC_ENTER */

#ifndef LOG_ENAINTSRC_LEAVE
#define	LOG_ENAINTSRC_LEAVE(ercd)
#endif /* LOG_ENAINTSRC_LEAVE */

/*
 *  すべての割込みの禁止（高速簡易版）
 *  全割込み禁止状態へ移行
 */
#ifdef TOPPERS_DisableAllInterrupts

#ifndef OMIT_STANDARD_DISALLINT

void
DisableAllInterrupts(void)
{
	LOG_DISINT_ENTER();
	if ((callevel_stat & (TSYS_DISALLINT | TSYS_SUSALLINT | TSYS_SUSOSINT)) == TSYS_NULL) {
		x_lock_all_int();
		ENTER_CALLEVEL(TSYS_DISALLINT);
	}
	LOG_DISINT_LEAVE();
}

#endif /* OMIT_STANDARD_DISALLINT */

#endif /* TOPPERS_DisableAllInterrupts */

/*
 *  すべての割込みの許可（高速簡易版）
 *  全割込み禁止状態を解除する
 */
#ifdef TOPPERS_EnableAllInterrupts

#ifndef OMIT_STANDARD_DISALLINT

void
EnableAllInterrupts(void)
{
	LOG_ENAINT_ENTER();
	if ((callevel_stat & (TSYS_SUSALLINT | TSYS_SUSOSINT)) == TSYS_NULL) {
		if ((callevel_stat & TSYS_DISALLINT) != TSYS_NULL) {
			LEAVE_CALLEVEL(TSYS_DISALLINT);
			x_unlock_all_int();
		}
	}
	LOG_ENAINT_LEAVE();
}

#endif /* OMIT_STANDARD_DISALLINT */

#endif /* TOPPERS_EnableAllInterrupts */

/*
 *  全割込み禁止
 *  CPU全ての割込みが対象の割込み禁止(ネストカウント有り)
 */
#ifdef TOPPERS_SuspendAllInterrupts

void
SuspendAllInterrupts(void)
{
#ifdef CFG_USE_ERRORHOOK
	StatusType ercd;
#endif /* CFG_USE_ERRORHOOK */

	LOG_SUSALL_ENTER();
	S_N_CHECK_DISALLINT();
	S_N_CHECK_ERROR((run_trusted != FALSE), E_OS_ACCESS);
	/* ネスト回数の上限値超過 */
	S_N_CHECK_LIMIT(sus_all_cnt != UINT8_INVALID);

	if (sus_all_cnt == 0U) {
		x_lock_all_int();
		ENTER_CALLEVEL(TSYS_SUSALLINT);
	}

	sus_all_cnt++;

  exit_no_errorhook:
	LOG_SUSALL_LEAVE();
	return;

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	if (kerflg != FALSE) {
		x_nested_lock_os_int();
		call_errorhook(ercd, OSServiceId_SuspendAllInterrupts);
		x_nested_unlock_os_int();
		goto exit_no_errorhook;
	}
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_SuspendAllInterrupts */

/*
 *  全割込み禁止解除
 *  CPU全ての割込みが対象の割込み許可(ネストカウント有り)
 */
#ifdef TOPPERS_ResumeAllInterrupts

void
ResumeAllInterrupts(void)
{
#ifdef CFG_USE_ERRORHOOK
	StatusType ercd;
#endif /* CFG_USE_ERRORHOOK */

	LOG_RSMALL_ENTER();
	S_N_CHECK_DISALLINT();
	S_N_CHECK_STATE(sus_all_cnt != 0U);

	sus_all_cnt--;

	if (sus_all_cnt == 0U) {
		LEAVE_CALLEVEL(TSYS_SUSALLINT);
		x_unlock_all_int();
	}

  exit_no_errorhook:
	LOG_RSMALL_LEAVE();
	return;

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	if (kerflg != FALSE) {
		x_nested_lock_os_int();
		call_errorhook(ercd, OSServiceId_ResumeAllInterrupts);
		x_nested_unlock_os_int();
		goto exit_no_errorhook;
	}
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_ResumeAllInterrupts */

/*
 *  OS割込みの禁止
 *  C2ISRが対象の割込み禁止(ネストカウント有り)
 */
#ifdef TOPPERS_SuspendOSInterrupts

void
SuspendOSInterrupts(void)
{
#ifdef CFG_USE_ERRORHOOK
	StatusType	ercd;
#endif /* CFG_USE_ERRORHOOK */
	TickType	tp_timer;

	LOG_SUSOSI_ENTER();
	S_N_CHECK_DISALLINT();
	/* ネスト回数の上限値超過 */
	S_N_CHECK_LIMIT(sus_os_cnt != UINT8_INVALID);

	if (x_is_called_in_c1isr() == FALSE) {
		/*
		 *  タスクかC2ISRから呼ばれた場合
		 */
		x_nested_lock_os_int();

		if (sus_os_cnt == 0U) {
			sus_os_prevpri = x_get_ipm();
			x_set_ipm_c2isr_disable();

			if ((callevel_stat == TCL_TASK) && ((p_runtsk->p_tinib->monitoring & BITMAP_OSINTLOCK) != 0U)) {
				/*
				 *  OS割り込み禁止監視処理
				 */
				/* 一つ前の監視項目に現在の監視項目を保存 */
				os_saved_watchtype = p_runtsk->watchtype;
				if (p_runtsk->watchtype == NON_MONITORING) {
					/*
					 *  現在の監視項目がない
					 */
					/* 現在の監視項目にLOCKOSINT_MONITORINGを保存 */
					p_runtsk->watchtype = LOCKOSINT_MONITORING;
					/* OS割り込み禁止時間バジェットでタイマを開始 */
					tp_start_timer(osinterruptlock_budget);
				}
				else {
					/*
					 *  現在の監視項目がタスクかリソース
					 */
					/* 残り実行時間バジェットを取得 */
					tp_timer = target_tp_get_remaining_ticks();
					if (tp_timer > osinterruptlock_budget) {
						/*
						 *  タイマ切替処理
						 */
						/* 現在の監視項目にLOCKOSINT_MONITORINGを保存 */
						p_runtsk->watchtype = LOCKOSINT_MONITORING;
						/* OS割り込み禁止時間バジェットとタイマ値の値の差を保存 */
						os_difftime = tp_timer - osinterruptlock_budget;
						/* OS割り込み禁止時間バジェットでタイマを開始 */
						tp_start_timer(osinterruptlock_budget);
					}
				}
			}
		}

		ENTER_CALLEVEL(TSYS_SUSOSINT);
		sus_os_cnt++;

		x_nested_unlock_os_int();
	}
	else {
		/*
		 *  C1ISRから呼ばれた場合
		 */
		if (sus_os_cnt == 0U) {
			ENTER_CALLEVEL(TSYS_SUSOSINT);
		}
		sus_os_cnt++;
	}

  exit_no_errorhook:
	LOG_SUSOSI_LEAVE();
	return;

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
	call_errorhook(ercd, OSServiceId_SuspendOSInterrupts);
	x_nested_unlock_os_int();
	goto exit_no_errorhook;
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_SuspendOSInterrupts */

/*
 *  OS割込み禁止解除
 *  C2ISRが対象の割込み許可(ネストカウント有り)
 */
#ifdef TOPPERS_ResumeOSInterrupts

void
ResumeOSInterrupts(void)
{
#ifdef CFG_USE_ERRORHOOK
	StatusType	ercd;
#endif /* CFG_USE_ERRORHOOK */
	TickType	tp_timer;

	LOG_RSMOSI_ENTER();
	S_N_CHECK_DISALLINT();
	S_N_CHECK_STATE(sus_os_cnt != 0U);


	if (x_is_called_in_c1isr() == FALSE) {
		/*
		 *  タスクかC2ISRから呼ばれた場合
		 */
		x_nested_lock_os_int();

		sus_os_cnt--;

		if (sus_os_cnt == 0U) {
			LEAVE_CALLEVEL(TSYS_SUSOSINT);

			x_set_ipm(sus_os_prevpri);

			if ((callevel_stat == TCL_TASK) && ((p_runtsk->p_tinib->monitoring & BITMAP_OSINTLOCK) != 0U)) {
				/*
				 *  OS割り込み禁止監視処理
				 */
				/* 現在の監視項目に一つ前の監視項目を保存 */
				p_runtsk->watchtype = os_saved_watchtype;
				if (os_saved_watchtype == NON_MONITORING) {
					/*
					 *  一つ前の監視項目がない
					 */
					/* 残り実行時間バジェットを取得し，停止する*/
					tp_timer = tp_stop_timer();
				}
				else {
					/*
					 *  一つ前の監視項目がタスクかリソース
					 */
					if (os_difftime != 0U) {
						/*
						 *  タイマ切替処理
						 */
						/* 残り実行時間バジェットを取得し，停止する*/
						tp_timer = tp_stop_timer();
						/* SuspendOSInterrupts時に保存したos_difftimeとタイマ値を一つ前の監視項目の残り時間を復帰させ，タイマをスタート */
						tp_start_timer(os_difftime + tp_timer);
						/* os_difftimeの初期化 */
						os_difftime = 0U;
					}
				}
			}
		}
		x_nested_unlock_os_int();
	}
	else {
		/*
		 *  C1ISRから呼ばれた場合
		 */
		sus_os_cnt--;
		if (sus_os_cnt == 0U) {
			LEAVE_CALLEVEL(TSYS_SUSOSINT);
		}
	}

  exit_no_errorhook:
	LOG_RSMOSI_LEAVE();
	return;

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
	call_errorhook(ercd, OSServiceId_ResumeOSInterrupts);
	x_nested_unlock_os_int();
	goto exit_no_errorhook;
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_ResumeOSInterrupts */

/*
 *  C2ISR IDの取得
 */
#ifdef TOPPERS_GetISRID

ISRType
GetISRID(void)
{
	ISRType		isrid;
	StatusType	ercd;

	LOG_GETISRID_ENTER();
	CHECK_CALLEVEL(CALLEVEL_GETISRID);

	isrid = (p_runisr == NULL) ? INVALID_ISR : ISR2ID(p_runisr);

  exit_finish:
	LOG_GETISRID_LEAVE(isrid);
	return(isrid);

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
	/*
	 *  エラー発生時はINVALID_ISRIDが返るが，エラーが発生したのか実行中の
	 *  C2ISRが存在しないのか区別するため，エラーフックを呼ぶ
	 */
	call_errorhook(ercd, OSServiceId_GetISRID);
	x_nested_unlock_os_int();
#endif /* CFG_USE_ERRORHOOK */

  exit_no_errorhook:
	isrid = INVALID_ISR;
	goto exit_finish;
}

#endif /* TOPPERS_GetISRID */

/*
 *  割込みの禁止
 */
#ifdef TOPPERS_DisableInterruptSource

StatusType
DisableInterruptSource(ISRType DisableISR)
{
	ISRCB		*p_isrcb;
	StatusType	ercd;

	LOG_DISINTSRC_ENTER(DisableISR);
	CHECK_CALLEVEL(CALLEVEL_DISABLEINTERRUPTSOURCE);
	CHECK_ID(DisableISR < tnum_isr2);
	p_isrcb = get_isrcb(DisableISR);
	CHECK_RIGHT(p_isrcb->p_isrinib->acsbtmp);
	CHECK_NOFUNC(target_is_int_controllable(p_isrcb->p_isrinib->p_intinib->intno) != FALSE);

	x_disable_int(GET_INTNO(p_isrcb));
	ercd = E_OK;

  exit_no_errorhook:
	LOG_DISINTSRC_LEAVE(ercd);
	return(ercd);

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
#ifdef CFG_USE_PARAMETERACCESS
	_errorhook_par1.isrid = DisableISR;
#endif /* CFG_USE_PARAMETERACCESS */
	call_errorhook(ercd, OSServiceId_DisableInterruptSource);
	x_nested_unlock_os_int();
	goto exit_no_errorhook;
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_DisableInterruptSource */

/*
 *  割込みの許可
 */
#ifdef TOPPERS_EnableInterruptSource

StatusType
EnableInterruptSource(ISRType EnableISR)
{
	ISRCB		*p_isrcb;
	StatusType	ercd;

	LOG_ENAINTSRC_ENTER(EnableISR);
	CHECK_CALLEVEL(CALLEVEL_ENABLEINTERRUPTSOURCE);
	CHECK_ID(EnableISR < tnum_isr2);
	p_isrcb = get_isrcb(EnableISR);
	CHECK_RIGHT(p_isrcb->p_isrinib->acsbtmp);
	CHECK_NOFUNC(target_is_int_controllable(p_isrcb->p_isrinib->p_intinib->intno) != FALSE);

	x_enable_int(GET_INTNO(p_isrcb));
	ercd = E_OK;

  exit_no_errorhook:
	LOG_ENAINTSRC_LEAVE(ercd);
	return(ercd);

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
#ifdef CFG_USE_PARAMETERACCESS
	_errorhook_par1.isrid = EnableISR;
#endif /* CFG_USE_PARAMETERACCESS */
	call_errorhook(ercd, OSServiceId_EnableInterruptSource);
	x_nested_unlock_os_int();
	goto exit_no_errorhook;
#endif /* CFG_USE_ERRORHOOK */
}

#endif /* TOPPERS_EnableInterruptSource */
