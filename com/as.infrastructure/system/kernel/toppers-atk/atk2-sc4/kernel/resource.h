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
 *  $Id: resource.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		リソース管理機能
 */

#ifndef TOPPERS_RESOURCE_H
#define TOPPERS_RESOURCE_H

/*
 *  リソース管理初期化ブロック（Os_Lcfg.c）
 */
typedef struct resource_initialization_block {
	PriorityType	ceilpri;
	uint32			acsbtmp;                /* アクセス許可OSアプリケーション ビットマップ */
	TickType		res_lockbudget;         /* リソース占有時間バジェット */
} RESINIB;

/*
 *  リソース管理ブロック（Os_Lcfg.c）
 */
typedef struct resource_control_block {
	const RESINIB					*p_resinib;             /* 初期化ブロックへのポインタ */
	PriorityType					prevpri;                /* リソース獲得前の優先度 */
	struct resource_control_block	*p_prevrescb;           /* 前に獲得したリソース管理ブロックへのポインタ */
	boolean							lockflg;                /* リソースが取得されていればTRUE */
	MonitoringType					res_saved_watchtype;    /* 一つ前の監視項目 */
	TickType						res_difftime;           /* リソース占有時間と元の監視項目との差分 */

} RESCB;

/*
 *  リソース数を保持する変数の宣言（Os_Lcfg.c）
 */
extern const ResourceType	tnum_stdresource;           /* 内部リソースを除くリソース数 */

/*
 *  リソース初期化ブロックのエリア（Os_Lcfg.c）
 */
extern const RESINIB		resinib_table[];

/*
 *  リソース管理ブロックのエリア（Os_Lcfg.c）
 */
extern RESCB				rescb_table[];

/*
 *  リソース管理機能の初期化
 */
extern void resource_initialize(void);

/*
 *  リソースID から リソース管理ブロックを取り出すためのマクロ
 */
#define get_rescb(resid)	(&(rescb_table[(resid)]))

#endif /* TOPPERS_RESOURCE_H */
