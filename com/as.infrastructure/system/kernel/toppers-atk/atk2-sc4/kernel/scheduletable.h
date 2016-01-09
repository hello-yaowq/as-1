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
 *  $Id: scheduletable.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		スケジュールテーブル機能
 */

#ifndef TOPPERS_SCHEDULETABLE_H
#define TOPPERS_SCHEDULETABLE_H

#include "counter.h"

/*
 *  満了点テーブル制御用特殊な満了点インデックス
 */
#define EXPPTINDEX_TOP			((uint8) 0x00)
#define EXPPTINDEX_INITIAL		((uint8) 0xff)

/*
 *  スケジュールテーブルIDからスケジュールテーブル管理ブロックを取り出すためのマクロ
 */
#define get_schtblcb(schtblid)	(&(schtblcb_table[(schtblid)]))

/*
 *  暗黙同期スケジュールテーブルに関する定義
 */
#define is_implschtbl(schtblid)	((schtblid) < tnum_implscheduletable)

/*
 *  個々の満了点テーブル型
 */
typedef struct scheduletable_expire_point_block {
	TickType		offset;                                 /* オフセット値 */
	FunctionRefType	expptfnt;                               /* 満了点処理関数のポインタ */
} SCHTBLEXPPTCB;


/*
 *  スケジュールテーブル初期化ブロック
 */
typedef struct scheduletable_initialization_block {
	CNTCB				*p_cntcb;                           /* 駆動カウンタ管理ブロックのポインタ */
	TickType			length;                             /* 周期の長さ */
	AppModeType			autosta;                            /* 起動するアプリケーションモード */
	AttributeType		actatr;                             /* 自動起動の属性 */
	TickType			staval;                             /* 自動起動ティック値 */
	const SCHTBLEXPPTCB	*p_exppt;                           /* 満了点テーブルの先頭ポインタ */
	boolean				repeat;                             /* 周期制御の有無 */
	uint8				tnum_exppt;                         /* 満了点数 */
	OSAPCB				*p_osapcb;                          /* 所属するOSアプリケーションの管理ブロック */
	uint32				acsbtmp;                            /* アクセス許可OSアプリケーション ビットマップ */
} SCHTBLINIB;

/*
 *  スケジュールテーブル管理ブロック
 */
typedef struct scheduletable_control_block {
	CNTEXPINFO							cntexpinfo;         /* カウンタ満了情報(構造体の先頭に入る必要) */
	const SCHTBLINIB					*p_schtblinib;      /* 初期化ブロックへのポインタ */
	struct scheduletable_control_block	*p_prevschtblcb;    /* 自分をNextにしたスケジュールテーブル管理ブロックへのポインタ */
	struct scheduletable_control_block	*p_nextschtblcb;    /* Nextスケジュールテーブル管理ブロックへのポインタ */
	ScheduleTableStatusType				status;             /* スケジュールテーブル状態 */
	uint8								expptindex;         /* 満了点インデックス */
} SCHTBLCB;

/*
 *  満了処理実行用管理情報
 */
typedef struct scheduletable_expire_info {
	SCHTBLCB *p_schtblcb;                                   /* スケジュールテーブル管理ブロックのアドレス */
} SCHTBLEXPINFO;

/*
 *  スケジュールテーブル数を保持する変数の宣言
 */
extern const ScheduleTableType	tnum_scheduletable;         /* 全スケジュールテーブルの数 */
extern const ScheduleTableType	tnum_implscheduletable;     /* 暗黙同期スケジュールテーブル数 */

/*
 *  スケジュールテーブル初期化ブロックのエリア（Os_Lcfg.c）
 */
extern const SCHTBLINIB			schtblinib_table[];
/*
 *  スケジュールテーブル管理ブロックのエリア（Os_Lcfg.c）
 */
extern SCHTBLCB					schtblcb_table[];

/*
 *  スケジュールテーブルオブジェクトの初期化
 */
extern void schtbl_initialize(void);

/*
 *  スケジュールテーブル満了処理関数
 */
extern void schtbl_expire(CNTEXPINFO *p_cntexpinfo, const CNTCB *p_cntcb);

/*
 *  満了処理関数から各タイミング処理の実行
 */
extern void schtbl_expiry_process(SCHTBLEXPINFO *p_schtblexpinfo, const CNTCB *p_cntcb);

/*
 *  スケジュールテーブルの開始処理
 */
extern boolean schtbl_head(SCHTBLCB *p_schtblcb, const CNTCB *p_cntcb);

/*
 *  スケジュールテーブルの各満了点処理
 */
extern boolean schtbl_exppoint_process(SCHTBLCB *p_schtblcb, const CNTCB *p_cntcb);

/*
 *  スケジュールテーブルの終端処理
 */
extern boolean schtbl_tail(SCHTBLCB *p_schtblcb, SCHTBLEXPINFO *p_schtblexpinfo, const CNTCB *p_cntcb);

/*
 *  OSAP所属するスケジュールテーブルの強制終了
 */
extern void force_term_osap_schtbl(OSAPCB *p_osapcb);

#endif /* TOPPERS_SCHEDULETABLE_H */
