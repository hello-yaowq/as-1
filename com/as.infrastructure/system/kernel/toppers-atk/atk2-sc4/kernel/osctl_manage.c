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
 *  $Id: osctl_manage.c 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		OS管理モジュール
 */

#include "kernel_impl.h"
#include "check.h"
#include "interrupt.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_GETAAM_ENTER
#define LOG_GETAAM_ENTER()
#endif /* LOG_GETAAM_ENTER */

#ifndef LOG_GETAAM_LEAVE
#define LOG_GETAAM_LEAVE(mode)
#endif /* LOG_GETAAM_LEAVE */

#ifndef LOG_STAOS_ENTER
#define LOG_STAOS_ENTER(mode)
#endif /* LOG_STAOS_ENTER */

#ifndef LOG_STAOS_LEAVE
#define LOG_STAOS_LEAVE()
#endif /* LOG_STAOS_LEAVE */

#ifndef LOG_STAHOOK_ENTER
#define LOG_STAHOOK_ENTER()
#endif /* LOG_STAHOOK_ENTER */

#ifndef LOG_STAHOOK_LEAVE
#define LOG_STAHOOK_LEAVE()
#endif /* LOG_STAHOOK_LEAVE */

#ifdef TOPPERS_StartOS

/*
 *  OS実行制御のための変数
 */
uint16		callevel_stat = 0U;             /* 実行中のコンテキスト */
AppModeType	appmodeid;                      /* アプリケーションモードID */

/*
 *  カーネル動作状態フラグ
 */
boolean		kerflg = FALSE;

/*
 *  特権モードで動作中かを示すフラグ
 */
boolean		run_trusted;

boolean		pre_protection_supervised;

/*
 *  ファイル名，行番号の参照用の変数
 */
const char8	*fatal_file_name = NULL;                    /* ファイル名 */
sint32		fatal_line_num = 0;                         /* 行番号 */

/*
 *  OSの起動
 */

void
StartOS(AppModeType Mode)
{

	LOG_STAOS_ENTER(Mode);
	if (kerflg != FALSE) {
		/* OS起動中はエラーフックを呼ぶ */
#ifdef CFG_USE_ERRORHOOK
		x_nested_lock_os_int();
#ifdef CFG_USE_PARAMETERACCESS
		_errorhook_par1.mode = Mode;
#endif /* CFG_USE_PARAMETERACCESS */
		call_errorhook(E_OS_ACCESS, OSServiceId_StartOS);
		x_nested_unlock_os_int();
#endif /* CFG_USE_ERRORHOOK */
	}
	else {

		/* 全割込み禁止状態に移行 */
		x_lock_all_int();

#ifdef CFG_USE_STACKMONITORING
		/*
		 *  スタックモニタリング機能の初期化
		 *  スタックモニタリング機能のためのマジックナンバー領域の初期化
		 */
		init_stack_magic_region();
#endif /* CFG_USE_STACKMONITORING */

		/* アプリケーションモードの設定 */
		appmodeid = Mode;

		/* ターゲット依存の初期化 */
		target_initialize();

		/* 各モジュールの初期化 */
		object_initialize();

		callevel_stat = TCL_NULL;

		/* カーネル動作中 */
		kerflg = TRUE;

		run_trusted = TRUE;

		/*
		 *  Modeが不正であった場合，OSシャットダウンを行う
		 *  この時，スタートアップフックは呼び出されない
		 */
		if (Mode >= tnum_appmode) {
			/*
			 *  internal_shutdownosを呼ぶ前にOS割込み禁止状態へ
			 *  全割込み禁止状態解除
			 */
			x_nested_lock_os_int();
			x_unlock_all_int();
			internal_shutdownos(E_OS_MODE);
		}

#ifdef CFG_USE_STARTUPHOOK
		/* OS割込み禁止状態にし，全割込み禁止状態解除 */
		x_nested_lock_os_int();
		x_unlock_all_int();

		/*
		 *  StartupHook の呼び出し
		 */
		ENTER_CALLEVEL(TCL_STARTUP);
		LOG_STAHOOK_ENTER();
		StartupHook();
		LOG_STAHOOK_LEAVE();
		LEAVE_CALLEVEL(TCL_STARTUP);

		/* 元の割込みマスク優先度と全割込み禁止状態に */
		x_lock_all_int();
		x_nested_unlock_os_int();
#endif /* CFG_USE_STARTUPHOOK */


		ENTER_CALLEVEL(TCL_TASK);
		LOG_STAOS_LEAVE();
		start_dispatch();
		ASSERT_NO_REACHED;
	}
}

#endif /* TOPPERS_StartOS */

/*
 *  現在のアプリケーションモードの取得
 */
#ifdef TOPPERS_GetActiveApplicationMode

AppModeType
GetActiveApplicationMode(void)
{
	AppModeType	appmode;
	StatusType	ercd;

	LOG_GETAAM_ENTER();
	CHECK_DISABLEDINT();
	CHECK_CALLEVEL(CALLEVEL_GETACTIVEAPPMODE);

	appmode = appmodeid;

  exit_finish:
	LOG_GETAAM_LEAVE(appmode);
	return(appmode);

#ifdef CFG_USE_ERRORHOOK
  exit_errorhook:
	x_nested_lock_os_int();
	/*
	 *  エラー発生時はINVALID_APPMODETYPEが返るが，エラーが発生したのか実行中の
	 *  C2ISRが存在しないのか区別するため，エラーフックを呼ぶ
	 */
	call_errorhook(ercd, OSServiceId_GetActiveApplicationMode);
	x_nested_unlock_os_int();
#endif /* CFG_USE_ERRORHOOK */

  exit_no_errorhook:
	appmode = INVALID_APPMODETYPE;
	goto exit_finish;
}

#endif /* TOPPERS_GetActiveApplicationMode */

/*
 *  OSの終了
 */
#ifdef TOPPERS_ShutdownOS

void
ShutdownOS(StatusType Error)
{
	StatusType ercd = Error;

	/*
	 *  呼出し元所属 OSアプリケーションの信頼/非信頼のチェック
	 *  呼出し元が信頼関数，フックである場合は
	 *  ShutdownOS() を実行する
	 */
	if (run_trusted == FALSE) {
		/* 非信頼から呼ばれた場合はエラーフックを呼ぶ */
#ifdef CFG_USE_ERRORHOOK
		x_nested_lock_os_int();
#ifdef CFG_USE_PARAMETERACCESS
		_errorhook_par1.error = Error;
#endif /* CFG_USE_PARAMETERACCESS */
		call_errorhook(E_OS_ACCESS, OSServiceId_ShutdownOS);
		x_nested_unlock_os_int();
#endif /* CFG_USE_ERRORHOOK */
	}
	else {

		/*
		 *  不正な処理単位から呼び出した場合も，ErrorをE_OS_SHUTDOWN_FATALとして
		 *  ShutdownOSを呼び出したものとして，シャットダウン処理を行う
		 */
		if (((callevel_stat & TCLMASK) | (CALLEVEL_SHUTDOWNOS)) != (CALLEVEL_SHUTDOWNOS)) {
			ercd = E_OS_SHUTDOWN_FATAL;
		}

		/*
		 *  OSで定義されていないエラーコードが指定された場合，ErrorをE_OS_SHUTDOWN_FATALとして
		 *  ShutdownOSを呼び出したものとして，シャットダウン処理を行う
		 */
		if (ercd > ERRCODE_NUM) {
			ercd = E_OS_SHUTDOWN_FATAL;
		}

		internal_shutdownos(ercd);
	}
}
#endif /* TOPPERS_ShutdownOS */

/*
 *  保護違反を起こした処理単位の取得
 */
#ifdef TOPPERS_GetFaultyContext

FaultyContextType
GetFaultyContext(void)
{
	FaultyContextType faultycontext = FC_INVALID;

#ifdef CFG_USE_PROTECTIONHOOK
	if ((callevel_stat & CALLEVEL_GETFAULTYCONTEXT) != 0U) {

		/* C1ISR以外で発生 */
		if ((callevel_stat & TSYS_ISR1) == 0U) {
			if ((callevel_stat & (TCL_ERROR | TCL_PREPOST | TCL_STARTUP | TCL_SHUTDOWN)) != 0U) {
				/* フック中に発生 */
				faultycontext = FC_SYSTEM_HOOK;
			}
			else if ((callevel_stat & TCL_ISR2) != 0U) {
				if (p_runisr->calltfn != FALSE) {
					/* C2ISRから呼出した信頼関数中に発生 */
					faultycontext = FC_TRUSTED_FUNC;
				}
				else {
					faultycontext = FC_C2ISR;
				}
			}
			else if ((callevel_stat & TCL_TASK) != 0U) {
				if (p_runtsk->calltfn != FALSE) {
					/* タスクから呼出した信頼関数中に発生 */
					faultycontext = FC_TRUSTED_FUNC;
				}
				else {
					faultycontext = FC_TASK;
				}
			}
			else {
				/* 上記以外の場合，処理は行わない(戻り値：FC_INVALID) */
			}
		}
	}
#endif /* CFG_USE_PROTECTIONHOOK */

	return(faultycontext);
}
#endif /* TOPPERS_GetFaultyContext */
