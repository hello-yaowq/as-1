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
 *  $Id: memory.h 504 2015-12-24 01:22:56Z witz-itoyo $
 */

/*
 *		メモリアクセス関連機能
 */

#ifndef TOPPERS_MEMORY_H
#define TOPPERS_MEMORY_H

#include "interrupt.h"
#include "task.h"

/*
 *  メモリ属性（カーネル内部で用いる）
 */
#define TOPPERS_ATTSEC	UINT_C(0x0100)  /* ATT_SEC/ATT_MODで登録されたメモリオブジェクト */
#define TOPPERS_ATTMEM	UINT_C(0x0200)  /* ATT_MEMで登録されたメモリオブジェクト */
#define TOPPERS_USTACK	UINT_C(0x0400)  /* タスク/C2ISRのユーザスタック領域 */

#ifndef TOPPERS_MACRO_ONLY

typedef	uint32 AccessPatternType;      /* アクセス許可パターン */

/*
 *  アクセス許可パターン
 */
#define TACP_KERNEL		((uint32) 0U)               /* カーネルドメインだけにアクセスを許可 */
#define TACP_SHARED		(~(uint32) 0U)              /* すべてのドメインからアクセスを許可 */

#ifndef OMIT_STANDARD_MEMINIB

/*
 *  メモリオブジェクト初期化ブロック
 */
typedef struct memory_initialization_block {
	AttributeType		mematr;         /* メモリオブジェクト属性 */
	AccessPatternType	acptnr;         /* リード許可 OSアプリケーション ビットパターン */
	AccessPatternType	acptnw;         /* ライト許可 OSアプリケーション ビットパターン */
	AccessPatternType	acptnx;         /* 実行許可   OSアプリケーション ビットパターン */
} MEMINIB;

/*
 *  メモリオブジェクト初期化ブロックの数（kernel_mem.c）
 */
extern const uint32		tnum_meminib;

/*
 *  メモリオブジェクトの先頭番地の領域（kernel_mem.c）
 */
extern void * const	memtop_table[];

/*
 *  メモリオブジェクト初期化ブロックの領域（kernel_mem.c）
 */
extern const MEMINIB	meminib_table[];

/*
 *  メモリオブジェクト初期化ブロックの検索
 *
 *  メモリオブジェクト初期化ブロックから，addrを含むメモリオブジェクト
 *  を検索し，そのインデックスを返す
 */

extern uint32 search_meminib(MemoryStartAddressType addr);

#endif /* OMIT_STANDARD_MEMINIB */

/*
 *  メモリ領域がメモリオブジェクトに含まれているかのチェック
 *
 *  先頭番地がbaseでサイズがsizeのメモリ領域が，先頭番地がmobaseでサイ
 *  ズがmosizeのメモリオブジェクトに含まれている場合にTRUE，そうでない
 *  場合にFALSEを返す
 */

LOCAL_INLINE boolean
within_memobj(MemoryStartAddressType base, MemorySizeType size, MemoryStartAddressType mobase, MemorySizeType mosize)
{
	return((mobase <= base) && (size <= mosize)
		   && (((MemorySizeType) base - (MemorySizeType) mobase) <= (mosize - size)));
}

/*
 *  スタックアクセスチェック本体
 */
extern AccessType check_address_stack(const MemoryStartAddressType base, MemorySizeType size, const MemoryStartAddressType mobase, MemorySizeType mosize);

/*
 *  メモリアクセスチェック
 */
extern AccessType check_osap_memory(OSAPCB *p_osapcb, const MemoryStartAddressType adr, MemorySizeType size);

/*
 *  ISRメモリアクセスチェックシステムサービス(カーネル内部用)
 */
extern AccessType check_isr_memory(ISRCB *p_isrcb, const MemoryStartAddressType adr, MemorySizeType size, boolean *over_region);

/*
 *  タスクメモリアクセスチェックシステムサービス(カーネル内部用)
 */
extern AccessType check_task_memory(const TCB *p_tcb, const MemoryStartAddressType adr, MemorySizeType size, boolean *over_region);

/*
 *  メモリアクセス権のチェック
 */
extern AccessType probe_memory_access(const MemoryStartAddressType base, MemorySizeType size);

/*
 *  メモリへの読込み権のチェック
 */
extern boolean probe_memory_read(const MemoryStartAddressType base, MemorySizeType size);

/*
 *  メモリへの書込み権のチェック
 */
extern boolean probe_memory_write(const MemoryStartAddressType base, MemorySizeType size);

/*
 *  メモリに対する読書き権のチェック
 */
extern boolean probe_memory_read_write(const MemoryStartAddressType base, MemorySizeType size);

/*
 *  DATAセクションとBSSセクションの初期化
 */
extern void initialize_sections(void);

/*
 *  メモリアクセス権チェックのためのマクロ
 */
#ifndef PROBE_MEM_WRITE
#define PROBE_MEM_WRITE(p_var, type) \
	(boolean) ((probe_memory_write((MemoryStartAddressType) (p_var), sizeof(type)) != FALSE) && (ALIGNED_TYPE(p_var, type) != FALSE))
#endif /* PROBE_MEM_WRITE */

#ifndef PROBE_MEM_RW
#define PROBE_MEM_RW(p_var, type) \
	(boolean) ((probe_memory_read_write((MemoryStartAddressType) (p_var), sizeof(type)) != FALSE) && (ALIGNED_TYPE(p_var, type) != FALSE))
#endif /* PROBE_MEM_RW */

/*
 *  dataセクション初期化ブロック
 */
typedef struct {
	void	*start_data;        /* dataセクションの先頭番地 */
	void	*end_data;          /* dataセクションの終了番地 */
	void	*start_idata;       /* 初期化データ領域の先頭番地 */
} DATASECINIB;

/*
 *  dataセクションの数と初期化ブロックのエリア（kernel_mem_2.c）
 */
extern const uint32			tnum_datasec;
extern const DATASECINIB	datasecinib_table[];

/*
 *  bssセクション初期化ブロック
 */
typedef struct {
	void	*start_bss;         /* bssセクションの先頭番地 */
	void	*end_bss;           /* bssセクションの終了番地 */
} BSSSECINIB;

/*
 *  bssセクションの数と初期化ブロックのエリア（kernel_mem_2.c）
 */
extern const uint32		tnum_bsssec;
extern const BSSSECINIB	bsssecinib_table[];

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_MEMORY_H_ */
