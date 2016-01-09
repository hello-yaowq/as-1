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
 *  $Id: ioc_impl.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		IOC機能
 */

#ifndef TOPPERS_IOC_IMPL_H
#define TOPPERS_IOC_IMPL_H

#include "kernel_impl.h"
#include "osap.h"

/*
 *  Wrapper IDからIOCWRPINIBを取り出すためのマクロ
 */
#define get_iocwrpinib(WrapperId)	(&iocwrpinib_table[(WrapperId)])

/*
 *  IOC IDからIOCCB,IOCINIBを取り出すためのマクロ
 */
#define get_ioccb(iocid)		(&ioccb_table[(iocid)])
#define get_iocinib(iocid)		(&iocinib_table[(iocid)])

/*
 *  IOCWRPINIBからIOC IDを取り出すためのマクロ
 */
#define IOCID(p_iocwrpinib)	((IocType) (((p_iocwrpinib)->p_iocinib) - iocinib_table))



/*
 *  IOC初期化ブロック
 */
typedef struct ioc_initialization_block {
	uint32	maxque;                 /* キュー数 */
	uint32	datasz;                 /* データサイズ */
	uint32	alignsz;                /* アラインサイズ */
#ifdef CFG_USE_ERRORHOOK
	boolean	groupflg;            /* グループ通信フラグ */
#endif /* CFG_USE_ERRORHOOK */
	OSAPCB	*p_rcvosapcb;        /* Receiverが所属するOSAPの初期化ブロックへのポインタ */
	void	*p_iocmb;
} IOCINIB;

/*
 *  IOC管理ブロック（Os_Lcfg.c）
 */
typedef struct ioc_control_block {
	uint32	quecnt;                 /* 現在のキュー数 */
	uint32	head;                   /* キューの先頭 */
	uint32	tail;                   /* キューの最後 */
	boolean	lostflg;                /* キューフルエラー(E_LOST_DATA)用フラグ */
} IOCCB;

/*
 *  IOC Wrapper初期化ブロック
 */
typedef struct ioc_wrapper_initialization_block {
	const IOCINIB	*p_iocinib;         /* IOC初期化ブロックへのポインタ */
	OSAPCB			*p_sndosapcb;       /* Senderが所属するOSAPの初期化ブロックへのポインタ */
	SenderIdType	senderid;           /* SenderID */
} IOCWRPINIB;


extern const IocType	tnum_ioc;       /* IOCの数 */
extern const IocType	tnum_queueioc;  /* キューありIOCの数 */

/*
 *  IOC Wrapper初期化ブロック（Os_Lcfg.c）
 */
extern const IOCWRPINIB	iocwrpinib_table[];

/*
 *  IOC初期化ブロック（Os_Lcfg.c）
 */
extern const IOCINIB	iocinib_table[];

/*
 *  IOC管理ブロック（Os_Lcfg.c）
 */
extern IOCCB			ioccb_table[];

/*
 *  IOC WRAPPER初期化ブロック（Os_Lcfg.c）
 */
extern const IOCWRPINIB	iocwrpinib_table[];

/*
 *  IOC初期化値テーブル
 */
extern void				*ioc_inival_table[];

/*
 *  IOC管理モジュールの初期化
 */
extern void ioc_initialize(void);

#endif /* TOPPERS_IOC_IMPL_H */
