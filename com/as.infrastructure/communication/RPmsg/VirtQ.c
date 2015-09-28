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
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void virtqueue_kick(VirtQ_QueueType *vq)
{
	/* For now, simply interrupt remote processor */
	if (vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT) {
		/* do nothing */
	}
	else
	{
		/* trigger IPC interrupt TODO */
		//Ipc_IsrTrigger(vq->id);
	}
}
void *virtqueue_get_avail_buf(VirtQ_QueueType *vq, VirtQ_IdxType *idx, uint32 *len)
{
	void* buf;
    /* There's nothing available? */
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
		*len = vq->vring.desc[*idx].len;
    }

    return buf;
}

void virtqueue_set_used_buf(VirtQ_QueueType *vq, VirtQ_IdxType idx, uint32 len)
{
   Vring_UsedElemType *used;

   if (idx > vq->vring.num) {
       assert(0);
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

void VirtQ_RxNotificatin(VirtQ_ChannerlType chl)
{

}
void VirtQ_TxConfirmation(VirtQ_ChannerlType chl)
{

}
