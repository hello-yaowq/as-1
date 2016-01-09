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
 *  $Id: interrupt.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		割込み管理機能
 */

#ifndef TOPPERS_INTERRUPT_H
#define TOPPERS_INTERRUPT_H

#include "resource.h"
#include "osap.h"
#include "timingprotection.h"

/*
 *  優先度値の定義（内部表現）
 */
#define TPRI_MINISR		(-1)            /* 最低割込み優先度 */

/*
 *  ISRIDからISRCBを取り出すためのマクロ
 */
#define get_isrcb(isrid)	(&(isrcb_table[(isrid)]))

/*
 *  ISRCBからISRIDを取り出すためのマクロ
 */
#define ISR2ID(p_isrcb)	((ISRType) ((p_isrcb) - isrcb_table))

#ifndef OMIT_INITIALIZE_INTERRUPT

/*
 *  割込み要求ライン初期化ブロック
 */
typedef struct interrupt_request_initialization_block {
	InterruptNumberType	intno;          /* 割込み番号 */
	AttributeType		intatr;         /* 割込み属性 */
	PriorityType		intpri;         /* 割込み優先度 */
#if defined(TOPPERS_CFG1_OUT) || defined(CFG_USE_STACKMONITORING)
	MemorySizeType remain_stksz;        /* スタック残量チェック方式用スタックサイズ */
#endif /* defined(TOPPERS_CFG1_OUT) || defined(CFG_USE_STACKMONITORING) */
} INTINIB;

/*
 *  割込み要求ラインの数（Os_Lcfg.c）
 */
extern const InterruptNumberType	tnum_intno;

/*
 *  割込み要求ライン初期化ブロックのエリア（Os_Lcfg.c）
 */
extern const INTINIB				intinib_table[];

/*
 *  ISRCBから割込み番号を割出すマクロ
 */
#define GET_INTNO(p_isrcb)	((p_isrcb)->p_isrinib->p_intinib->intno)

/*
 *  ISRCBから割込み優先度を割出すマクロ
 */
#define GET_INTPRI(p_isrcb)	((p_isrcb)->p_isrinib->p_intinib->intpri)

#endif /* OMIT_INITIALIZE_INTERRUPT */

typedef struct isr_initialization_block {
	const INTINIB	*p_intinib;     /* 割込み要求ライン初期化ブロックへのポインタ */
	OSAPCB			*p_osapcb;      /* 所属するOSアプリケーションの管理ブロック */
	uint32			acsbtmp;        /* アクセス許可OSアプリケーション ビットマップ */
	TFTIME			time_frame;     /* 最小到着時間 */
} ISRINIB;

typedef struct isr_control_block {
	const ISRINIB	*p_isrinib;
	RESCB			*p_lastrescb;   /* 最後に獲得したリソース管理ブロックへのポインタ */
#ifdef CFG_USE_PROTECTIONHOOK
	boolean	calltfn;                /* 信頼関数呼び出し中フラグ */
#endif /* CFG_USE_PROTECTIONHOOK */
	TPACB 			tpacb;          /* 到着間隔保護保護管理ブロック */
} ISRCB;

/*
 *  C2ISR数を保持する変数の宣言（Os_Lcfg.c）
 */
extern const ISRType	tnum_isr2;                         /* C2ISRの数 */

/*
 *  C2ISRの初期化ブロックのエリア（Os_Lcfg.c）
 */
extern const ISRINIB	isrinib_table[];

/*
 *  C2ISRの管理ブロックのエリア（Os_Lcfg.c）
 */
extern ISRCB			isrcb_table[];

/*
 *  実行中のC2ISR
 *
 *  C2ISRを実行していない時は，NULL にする
 */
extern ISRCB			*p_runisr;

/*
 *  SuspendAllInterrupts のネスト回数
 */
extern uint8			sus_all_cnt;

/*
 *  SuspendOSInterrupts のネスト回数
 */
extern uint8			sus_os_cnt;

/*
 *  SuspendOSInterruptsの最初の呼び出し時の割込み優先度
 */
extern PriorityType		sus_os_prevpri;

/*
 * OS割り込み禁止時間バジェット
 */
extern const TickType	osinterruptlock_budget;

/*
 *  OS割り込み禁止時間バジェットと元の監視項目の残り時間との差分
 */
extern TickType			os_difftime;

/*
 *  一つ前の監視項目
 */
extern MonitoringType	os_saved_watchtype;

/*
 *  割込み管理機能の初期化
 */
extern void interrupt_initialize(void);

/*
 *  割込み禁止の強制解除
 */

extern void release_interrupts(OSServiceIdType serviceId);

/*
 *  C2ISR終了時のチェック関数
 */
extern void exit_isr2(void);

#endif /* TOPPERS_INTERRUPT_H */
