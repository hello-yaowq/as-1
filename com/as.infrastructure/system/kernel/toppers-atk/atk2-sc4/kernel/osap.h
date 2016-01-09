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
 *  $Id: osap.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		OSアプリケーション管理機能
 */

#ifndef TOPPERS_OSAP_H
#define TOPPERS_OSAP_H

/*
 *  OSアプリケーションの属性
 */
#define TA_NONTRUSTED			(FALSE)
#define TA_TRUSTED				(TRUE)

/*
 *  OSアプリケーションIDからOSAPINIBを取り出すためのマクロ
 */
#define get_osapinib(osapid)	(&(osapinib_table[(osapid)]))

/*
 *  OSAPIDからOSAPCBを取り出すためのマクロ
 */
#define get_osapcb(osapid)		(&(osapcb_table[(osapid)]))

/*
 *  OSAPCBからOSアプリケーションIDを取り出すためのマクロ
 */
#define OSAPID(p_osapcb)	((ApplicationType) ((p_osapcb) - osapcb_table))

#ifndef TOPPERS_MACRO_ONLY

typedef struct os_application_control_block OSAPCB;
#include "task.h"

/*
 *  OSアプリケーション初期化ブロック
 *
 *  OSアプリケーションに関する情報を，OSアプリケーション制御ブロックと
 *  して定義する
 *  他のオブジェクトは，ROMに置く初期化ブロックとRAMに置く制御ブロック
 *  で構成されているが，OSアプリケーションに関する情報は実行時に変更さ
 *  れることがないため，初期化ブロックを制御ブロックとして使用する
 *
 *  ATK2-SC3では，メモリプロテクション機能のための情報を持たせるが，
 *  メモリプロテクション実装はCPUに依存するため，メモリプロテクション
 *  情報の本体は機種依存部に持たせる
 */
typedef struct os_application_initialization_block {
	TCB		*p_restart_tcb;                             /* OSAPのリスタートタスク管理ブロックへのポインタ */
	boolean	osap_trusted;                               /* OSアプリケーションの属性 */
	uint32	btptn;                                      /* 非信頼OSアプリケーションのビットパターン */
#ifndef OMIT_OSAPMPUINFOB
	OSAPMPUINFOB osap_mpu;                              /* OSアプリケーションのMPU情報 */
#endif

} OSAPINIB;

/*
 *  OSアプリケーション管理ブロック
 */
struct os_application_control_block {
	const OSAPINIB			*p_osapinib;                /* 初期化ブロックへのポインタ */
	ApplicationStateType	osap_stat;                  /* OSAP状態 */
};

/*
 *  OSアプリケーション数を保持する変数の宣言（Os_Lcfg.c）
 */
extern const ApplicationType			tnum_osap;      /* OSアプリケーションの数 */

extern const TrustedFunctionIndexType	tnum_tfn;       /* 信頼関数の数 */

typedef StatusType (*TrustedFunctionRefType)(TrustedFunctionIndexType FunctionIndex,
											 TrustedFunctionParameterRefType FunctionParams);

/*
 *  信頼関数初期化ブロック
 */
typedef struct trusted_function_initialization_block {
	TrustedFunctionRefType	trs_func;   /* 信頼関数の起動番地 */
	MemorySizeType			tf_stksz;   /* スタックサイズ */
	TickType				tf_budget;  /* 信頼関数実行時間バジェット */
} TFINIB;

/*
 *  OSAPCBのエリア（Os_Lcfg.c）
 */
extern OSAPCB			osapcb_table[];

/*
 *  実行中のOSアプリケーション
 */
extern OSAPCB			*p_runosap;

/*
 *  OSAPINIBの外部参照（kernel_mem.c）
 */
extern const OSAPINIB	osapinib_table[];

/*
 *  TFINIBの外部参照（Os_Lcfg.c）
 */
extern const TFINIB		tfinib_table[];

/*
 *  指定OSAPを終了/再起動する関数(プロテクションフックからも呼ばれる)
 */
extern void force_term_osap(OSAPCB *p_osapcb, RestartType RestartOption);

/*
 *  OSアプリケーション管理モジュールの初期化
 */
extern void osap_initialize(void);

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_OSAP_H_ */
