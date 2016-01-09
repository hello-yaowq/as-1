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
 *  $Id: timingprotection.c 504 2015-12-24 01:22:56Z witz-itoyo $
 */


/*
 *  タイミング保護管理モジュール
 *
 */
#include "kernel_impl.h"
#include "task.h"
#include "interrupt.h"
#include "timingprotection.h"

/*
 *  タイミング保護機能の初期化
 */
#ifdef TOPPERS_tp_initialize

volatile boolean	is_tp_timer_running;
volatile uint32		tp_time_count;


void
tp_initialize(void)
{
	is_tp_timer_running = FALSE;
	target_tp_initialize();
}

#endif /* TOPPERS_tp_initialize */

/*
 *  タイミング保護機能の終了
 *  OS処理レベルで呼び出される
 */
#ifdef TOPPERS_tp_terminate

void
tp_terminate(void)
{
	is_tp_timer_running = FALSE;
	target_tp_terminate();
}

#endif /* TOPPERS_tp_terminate */

/*
 *  タイミング保護タイマの開始
 *  現在実行中タスクのタイミング保護の監視が必要なら開始する
 *  OS処理レベルで呼び出される
 */
#ifdef TOPPERS_tp_start_timer
void
tp_start_timer(TickType tick)
{
	is_tp_timer_running = TRUE;
	target_tp_start_timer(tick);
}

#endif /* TOPPERS_tp_start_timer  */

/*
 *  タイマの停止処理
 *  OS処理レベルで呼び出される
 */
#ifdef TOPPERS_tp_stop_timer

TickType
tp_stop_timer(void)
{
	is_tp_timer_running = FALSE;
	return(target_tp_stop_timer());
}

#endif /* TOPPERS_tp_stop_timer */

/*
 *  タイミング保護タイマの停止
 *  現在実行中タスクのタイミング保護の監視を停止する
 *  OS処理レベルで呼び出される
 */
#ifdef TOPPERS_tp_stop_task_monitor

void
tp_stop_task_monitor(void)
{
	TCB *tcb = p_runtsk;

	ASSERT(tcb != NULL);

	tcb->remaining_execution = tp_stop_timer();
}

#endif /* TOPPERS_tp_stop_task_monitor */

/*
 *  現在時刻の取得
 */
#ifdef TOPPERS_tp_get_current_time

void
tp_get_current_time(TFTIME *tftime)
{
	TickType	tick1;
	TickType	tick2;
	uint32		current_count;
	boolean		is_timeout;
	SIL_PRE_LOC;

	SIL_LOC_INT();

	current_count = tp_time_count;
	tick1 = target_tp_get_elapsed_ticks();
	is_timeout = target_tp_sense_interrupt();
	tick2 = target_tp_get_elapsed_ticks();

	SIL_UNL_INT();

	if ((is_timeout != FALSE) && (tick2 >= tick1)) {
		/* 割込み禁止後，tick1を取得する前にタイムアウトしている場合 */
		current_count++;
	}

	tftime->tfcount = current_count;
	tftime->tftick = tick1;

	return;
}

#endif /* TOPPERS_tp_get_current_time */

/*
 *  到着間隔チェック
 *  OS処理レベルで呼び出される
 */
#ifdef TOPPERS_tp_check_arrival_time

boolean
tp_check_arrival_time(TFTIME time_frame, TPACB *p_tpacb, uint8 context)
{
	TFTIME	now;
	TFTIME	diff;
	boolean flg = TRUE;

	tp_get_current_time(&now);

	if (p_tpacb->is_not_first != FALSE) {
		/*
		 *  2度目以降の場合は到着間隔をチェックする
		 */
		/*
		 *  起動間隔保護タイマは常に進んでいるから now >= p_tpacb.arrival_timeが成り立つ
		 */
		diff.tfcount = now.tfcount - p_tpacb->lastarrival.tfcount;

		if (now.tftick >= p_tpacb->lastarrival.tftick) {
			diff.tftick = now.tftick - p_tpacb->lastarrival.tftick;
		}
		else {
			/*
			 *  nowのtick部分が小さい場合の対応
			 */
			diff.tfcount--;
			diff.tftick = ((TP_TIMER_MAX_TICK - p_tpacb->lastarrival.tftick) + now.tftick) + 1U;
		}

		if ((diff.tfcount <= time_frame.tfcount) &&
			((diff.tfcount != time_frame.tfcount) ||
			 (diff.tftick < time_frame.tftick))) {
			if (context == ARRIVAL_TASK) {
				/*タスクの到着間隔監視*/
				flg = FALSE;
			}
			else {
				pre_protection_supervised = run_trusted;
				/*C2ISRの到着間隔監視*/
				call_protectionhk_main(E_OS_PROTECTION_ARRIVAL_ISR);
				/* PRO_IGNOREの場合に戻ってくる */
			}
		}
	}
	else {
		/*
		 *  初回の場合は無条件で通す
		 */
		p_tpacb->is_not_first = TRUE;
	}
	if (flg != FALSE) {
		p_tpacb->lastarrival = now;
	}
	return(flg);
}
#endif /* TOPPERS_tp_check_arrival_time */

/*
 *  タイミング保護監視タイマハンドラ
 */
#ifdef TOPPERS_tp_fault_handler

void
tp_fault_handler(void)
{
	StatusType ercd = E_OS_PROTECTION_FATAL;

	target_clear_tp_fault_status();

	if (p_runtsk->watchtype == TASK_MONITORING) {
		ercd = E_OS_PROTECTION_TIME_TASK;
	}
	else if (p_runtsk->watchtype == RESOURCE_MONITORING) {
		ercd = E_OS_PROTECTION_LOCKED_RESOURCE;
	}
	else if (p_runtsk->watchtype == LOCKOSINT_MONITORING) {
		ercd = E_OS_PROTECTION_LOCKED_OSINT;
	}
	else {
		ASSERT_NO_REACHED;
	}

	pre_protection_supervised = run_trusted;
	call_protectionhk_main(ercd);

	internal_shutdownos(E_OS_SYS_ASSERT_FATAL);
	/* 呼び出し元には戻らない */
}

#endif /* TOPPERS_tp_fault_handler */

/*
 *  タイミング保護用タイマハンドラ
 */
#ifdef TOPPERS_tp_timer_handler

void
tp_timer_handler(void)
{
	target_clear_tp_timer_status();

	tp_time_count++;
}

#endif /* TOPPERS_tp_timer_handler */
