/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2015  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "VirtQ.h"
#include "Ipc.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_VIRTQ 1
/* ============================ [ TYPES     ] ====================================================== */
typedef struct
{
	const VirtQ_ConfigType* config;
	VirtQ_QueueType vq[VIRTQ_CHL_NUM];
	boolean initialized;
}virtq_t;
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static virtq_t virtq =
{
	.initialized = FALSE
};
/* ============================ [ LOCALS    ] ====================================================== */
static void *virtqueue_get_avail_buf(VirtQ_QueueType *vq, VirtQ_IdxType *idx, uint16 *len)
{
	void* buf;
#if defined(__LINUX__) || defined(__WINDOWS__)
    try_next:
#endif
    /* There's nothing available? */
	ASLOG(VIRTQ,"VirtQ ring avail address is %X\n",vq->vring.avail);
	ASLOG(VIRTQ,"VirtQ get last_avail_idx=%d vring.avail->idx=%d\n",vq->last_avail_idx, vq->vring.avail->idx);
    if (vq->last_avail_idx == vq->vring.avail->idx) {
        /* We need to know about added buffers */
        vq->vring.used->flags &= ~VRING_USED_F_NO_NOTIFY;

        buf = NULL;
    }
    else
    {
		/*
		 * Grab the next descriptor number they're advertising, and increment
		 * the index we've seen.
		 */
    	*idx = vq->vring.avail->ring[vq->last_avail_idx++ % vq->vring.num];

		buf = IPC_MAP_PA_TO_VA(vq->vring.desc[*idx].addr);
		*len = (uint16)vq->vring.desc[*idx].len;
    }
#if defined(__LINUX__) || defined(__WINDOWS__)
    if( ((uint32_t)buf < 0xFFFF) && (buf != NULL) )
    {	 /* TODO: sometimes, the buffer pointer is abnormal */
    	ASWARNING("VirtQ get buf(%X) len(%X) idx(%d), the pointer is abnormal\n",(uint32_t)buf,*len,*idx);
    	ASLOG(STDOUT,"VirtQ get last_avail_idx=%d vring.avail->idx=%d\n",vq->last_avail_idx, vq->vring.avail->idx);
    	goto try_next;
    }
#endif
    return buf;
}

static void virtqueue_set_used_buf(VirtQ_QueueType *vq, VirtQ_IdxType idx, uint32 len)
{
   Vring_UsedElemType *used;

   ASLOG(VIRTQ,"VirtQ set used buf idx=%d vq->vring.used->idx=%d\n",idx, vq->vring.used->idx);

   if (idx > vq->vring.num) {
       asAssert(0);
   }
   else
   {
	   /*
	   * The virtqueue contains a ring of used buffers.  Get a pointer to the
	   * next entry in that used ring.
	   */
	   used = &vq->vring.used->ring[vq->vring.used->idx % vq->vring.num];
	   used->id = idx;
	   used->len = len;

	   vq->vring.used->idx++;
   }
}
/* ============================ [ FUNCTIONS ] ====================================================== */
void VirtQ_InitVq(VirtQ_ChannerlType chl)
{
	vring_init(	&virtq.vq[chl].vring,
				virtq.config->queueConfig[chl].vring->num,
				IPC_MAP_PA_TO_VA(virtq.config->queueConfig[chl].vring->da),
				virtq.config->queueConfig[chl].vring->align
			);

	virtq.vq[chl].last_avail_idx = 0;
}
Std_ReturnType VirtQ_GetAvailiableBuffer(VirtQ_ChannerlType chl,VirtQ_IdxType* idx,void** buf,uint16* len)
{
	Std_ReturnType ercd;

	asAssert(chl < VIRTQ_CHL_NUM);
	asAssert(virtq.initialized);

	ASLOG(VIRTQ,"VirtQ_GetAvailiableBuffer(chl=%d)\n",chl);

	*buf = virtqueue_get_avail_buf(&virtq.vq[chl],idx,len);

	if(*buf)
	{
		ercd = E_OK;
	}
	else
	{
		ercd = E_NOT_OK;
	}

	return ercd;
}

void VirtQ_AddUsedBuffer(VirtQ_ChannerlType chl,VirtQ_IdxType idx,uint16 len)
{
	virtqueue_set_used_buf(&virtq.vq[chl],idx,len);
}

void VirtQ_Kick(VirtQ_ChannerlType chl)
{
	if (virtq.vq[chl].vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT) {
		/* do nothing */
	}
	else
	{
		/* trigger IPC interrupt */
		Ipc_WriteIdx(virtq.config->queueConfig[chl].chl,virtq.config->queueConfig[chl].idx);
	}
}
void VirtQ_Init(const VirtQ_ConfigType *config)
{
	if(FALSE == virtq.initialized)
	{
		virtq.config = config;
		virtq.initialized = TRUE;
	}
	else
	{
		asAssert(0);
	}
}
void VirtQ_RxNotificatin(VirtQ_ChannerlType chl)
{
	asAssert(virtq.initialized);
	if(virtq.config->queueConfig[chl].rxNotification)
	{
		virtq.config->queueConfig[chl].rxNotification(virtq.config->queueConfig[chl].handler);
	}
}
void VirtQ_TxConfirmation(VirtQ_ChannerlType chl)
{
	(void)chl;
}
