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
 *
 * Refereed to Linux virtio and SYSBIOS-RPMSG source code
 * http://git.omapzoom.org/?p=repo/sysbios-rpmsg.git;a=summary
 */
#ifndef COM_AS_INFRASTRUCTURE_COMMUNICATION_RPMSG_VIRTQ_H_
#define COM_AS_INFRASTRUCTURE_COMMUNICATION_RPMSG_VIRTQ_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include "virtio_ring.h"
/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
typedef uint8 VirtQ_ChannerlType;
typedef void (*VirtQ_NotificationType)(VirtQ_ChannerlType channel);
typedef uint16 VirtQ_IdxType;
typedef uint16 VirtQ_IdxSizeType;
typedef struct virtqueue
{
    /* Shared state */
    Vring_Type            		vring;

    /* Last available index; updated by VirtQueue_getAvailBuf */
    VirtQ_IdxType           last_avail_idx;

    /* Last available index; updated by VirtQueue_addUsedBuf */
    VirtQ_IdxType           last_used_idx;
}VirtQ_QueueType;

typedef struct
{
    /* Id for this VirtQueue_Object */
	VirtQ_IdxType               idx;

    /* The function to call when buffers are consumed (can be NULL) */
	VirtQ_NotificationType      rxNotification;

}VirtQ_QueueConfigType;

typedef struct
{
	const VirtQ_QueueConfigType* queueConfig;
}VirtQ_ConfigType;

#include "VirQ_Cfg.h"
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void virtqueue_kick(VirtQ_QueueType *vq);
void *virtqueue_get_avail_buf(VirtQ_QueueType *vq, VirtQ_IdxType* idx, uint32 *len);
void virtqueue_set_used_buf(VirtQ_QueueType *vq, VirtQ_IdxType idx, uint32 len);

void VirtQ_RxNotificatin(VirtQ_ChannerlType chl);
void VirtQ_TxConfirmation(VirtQ_ChannerlType chl);
#endif /* COM_AS_INFRASTRUCTURE_COMMUNICATION_RPMSG_VIRTQ_H_ */
