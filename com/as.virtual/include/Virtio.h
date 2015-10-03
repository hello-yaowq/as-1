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
#ifndef RELEASE_ASCORE_VIRTUAL_AS_VIRTUAL_INCLUDE_VIRTIO_H_
#define RELEASE_ASCORE_VIRTUAL_AS_VIRTUAL_INCLUDE_VIRTIO_H_
/* ============================ [ INCLUDES  ] ====================================================== */
#include <stdint.h>
#include <stdbool.h>
#include <QThread>
#include <QDebug>
#ifdef __WINDOWS__
#include <windows.h>
#else
#include <pthread.h>
#include <dlfcn.h>
#endif
#include <QList>
#include <assert.h>
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define IPC_FIFO_SIZE 1024

#define RPROC_RSC_NUM  1
#define RPROC_RPMSG_CFG_SIZE  16
#define RPROC_NUM_OF_VRINGS 2

#define RPROC_RPMSG_FEATURES (1 << 0)

/* This marks a buffer as continuing via the next field. */
#define VRING_DESC_F_NEXT	1
/* This marks a buffer as write-only (otherwise read-only). */
#define VRING_DESC_F_WRITE	2
/* This means the buffer contains a list of buffer descriptors. */
#define VRING_DESC_F_INDIRECT	4

/* The Host uses this in used->flags to advise the Guest: don't kick me when
 * you add a buffer.  It's unreliable, so it's simply an optimization.  Guest
 * will still kick if it's out of buffers. */
#define VRING_USED_F_NO_NOTIFY	1
/* The Guest uses this in avail->flags to advise the Host: don't interrupt me
 * when you consume a buffer.  It's unreliable, so it's simply an
 * optimization.  */
#define VRING_AVAIL_F_NO_INTERRUPT	1

/* We support indirect buffer descriptors */
#define VIRTIO_RING_F_INDIRECT_DESC	28

/* The Guest publishes the used index for which it expects an interrupt
 * at the end of the avail ring. Host should ignore the avail->flags field. */
/* The Host publishes the avail index for which it expects a kick
 * at the end of the used ring. Guest should ignore the used->flags field. */
#define VIRTIO_RING_F_EVENT_IDX		29

/* Alignment requirements for vring elements.
 * When using pre-virtio 1.0 layout, these fall out naturally.
 */
#define VRING_AVAIL_ALIGN_SIZE 2
#define VRING_USED_ALIGN_SIZE 4
#define VRING_DESC_ALIGN_SIZE 16

#define RPMSG_NAME_SIZE			32
#define RPMSG_DATA_SIZE         512

extern unsigned long Ipc_BaseAddress;
#define IPC_MAP_PA_TO_VA(addr) ((void*)(unsigned long)(Ipc_BaseAddress+addr))
/* ============================ [ TYPES     ] ====================================================== */
/* Virtio ring descriptors: 16 bytes.  These can chain together via "next". */
typedef struct {
    /* Address (guest-physical). */
	uint32_t addr;
    /* Length. */
    uint32_t len;
    /* The flags as indicated above. */
    uint16_t flags;
    /* We chain unused descriptors via this, too */
    uint16_t next;
}Vring_DescType;

typedef struct {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[];
}Vring_AvailType;

/* u32 is used here for ids for padding reasons. */
typedef struct {
    /* Index of start of used descriptor chain. */
    uint32_t id;
    /* Total length of the descriptor chain which was used (written to) */
    uint32_t len;
}Vring_UsedElemType;

typedef struct {
    uint16_t flags;
    uint16_t idx;
    Vring_UsedElemType ring[];
}Vring_UsedType;

typedef struct {
    uint32_t num;	/* must be 2^n */

    Vring_DescType *desc;

    Vring_AvailType *avail;

    Vring_UsedType *used;
}Vring_Type;

typedef uint8_t Ipc_ChannelType;
typedef uint32_t VirtQ_IdxType;
typedef uint32_t VirtQ_IdxSizeType;
typedef struct
{
    VirtQ_IdxSizeType count;
    VirtQ_IdxType     idx[IPC_FIFO_SIZE];
}Ipc_FifoType;

typedef enum {
    RSC_CARVEOUT	= 0,
    RSC_DEVMEM	= 1,
    RSC_TRACE	= 2,
    RSC_VDEV	= 3,
    RSC_LAST	= 4,
}Rproc_ReourceType;

typedef struct {
    /* For 64 bit ECU, address is 64 bit */
	uint32_t da;
    uint32_t align;
    uint32_t num;
    uint32_t notifyid;
    uint32_t reserved;
}Rproc_ReseouceVdevVringType;

typedef struct {
    uint32_t type;
    uint32_t id;
    uint32_t notifyid;
    uint32_t dfeatures;
    uint32_t gfeatures;
    uint32_t config_len;
    uint8_t status;
    uint8_t num_of_vrings;
    uint8_t reserved[2];
    Rproc_ReseouceVdevVringType vring[RPROC_NUM_OF_VRINGS];
}Rproc_ResourceVdevType;

typedef struct
{
    uint32_t version;
    uint32_t num;
    uint32_t reserved[2];
    uint32_t offset[RPROC_RSC_NUM];  /* Should match 'num' in actual definition */
    Rproc_ResourceVdevType rpmsg_vdev;
#if RPROC_RPMSG_CFG_SIZE > 0 /* @config_len of Rproc_ResourceVdevType */
    uint8_t                 rpmsg_cfg[RPROC_RPMSG_CFG_SIZE];
#endif
}Rproc_ResourceTableType;

typedef struct{
    uint32_t src;
    uint32_t dst;
    uint32_t reserved;
    uint16_t len;
    uint16_t flags;
    uint8_t data[RPMSG_DATA_SIZE];
} RPmsg_HandlerType;

typedef struct rpmsg_ns_msg {
    char name[RPMSG_NAME_SIZE];
    uint32_t addr;
    uint32_t flags;
} RPmsg_NsMsgType;

typedef struct {
    // the CAN ID, 29 or 11-bit
    uint32_t 	id;
    // Length, max 8 bytes
    uint8_t		length;
    // data ptr
    uint8_t 		sdu[8];
} Can_RPmsgPduType;

typedef bool (*PF_IPC_IS_READY)(Ipc_ChannelType chl);
extern void Virtio_SetIpcBaseAddress(unsigned long base);
/* ============================ [ CLASS     ] ====================================================== */
class Vring{
private:
    Rproc_ReseouceVdevVringType* ring;
    Vring_Type vr;
    /* Last available index */
    VirtQ_IdxType           last_avail_idx;

    /* Last available index */
    VirtQ_IdxType           last_used_idx;

    /* Head of free buffer list. */
    uint32_t free_head;
    /* Number we've added since last sync. */
    uint32_t num_added;
    uint32_t heap[1024*1024/4];
public:
    explicit Vring ( Rproc_ReseouceVdevVringType* ring ) : ring(ring)
    {
        init();
    }
    VirtQ_IdxType get_notifyid(void)
    {
        return ring->notifyid;
    }
    void* get_used_buf(VirtQ_IdxType* idx,uint32_t *len)
    {
        void* buf;
        Vring_UsedElemType* used;
        /* There's nothing available? */
        if (last_used_idx == vr.used->idx) {
            /* We need to know about added buffers */
            vr.avail->flags &= ~VRING_AVAIL_F_NO_INTERRUPT;

            buf = NULL;
        }
        else
        {
            /*
             * Grab the next descriptor number they're advertising, and increment
             * the index we've seen.
             */
            used = &(vr.used->ring[last_used_idx++ % vr.num]);
            *idx = used->id;
            *len = used->len;
            buf = IPC_MAP_PA_TO_VA(vr.desc[*idx].addr);

            num_added--;
        }

        return buf;
    }
    void put_used_buf_back(VirtQ_IdxType idx)
    {
       ASLOG(OFF,"put_used_buf_back(idx=%d),vr.avail->idx=%d\n",idx,vr.avail->idx);
       if (idx > vr.num) {
           assert(0);
       }
       else
       {
           vr.avail->ring[vr.avail->idx++ % vr.num] = idx;
           num_added ++;
       }
    }
    bool add_buf(void* data, uint32_t len)
    {
        bool added;
        if(num_added<vr.num)
        {
            if(8 == sizeof(void*))
            {
                Virtio_SetIpcBaseAddress(((unsigned long)data)&0xFFFFFFFF00000000UL);
            }
            vr.desc[free_head].addr = (uint32_t)(unsigned long) data;
            vr.desc[free_head].len  = len;
            vr.desc[free_head].flags = VRING_DESC_F_NEXT;
            free_head = vr.desc[free_head].next;
            vr.avail->idx ++;
            vr.avail->ring[free_head] = free_head;
            num_added ++;
            added = true;
        }
        else
        {
            added = false;
        }
        return added;
    }

private:
    void init(void)
    {
        void *p = malloc(size());
        memset(p,0,size());
        ring->da = (uint32_t)(unsigned long)p;
        vr.num = ring->num;
        vr.desc = (Vring_DescType*)p;
        vr.avail = (Vring_AvailType*)((unsigned long)p + ring->num*sizeof(Vring_DescType));
        vr.used = (Vring_UsedType*)(((unsigned long)(&vr.avail->ring[ring->num]) + sizeof(uint16_t)
            + ring->align-1) & ~((unsigned long)ring->align - 1));
        for(uint32_t i=0;i<(vr.num-1);i++)
        {
            vr.desc[i].next = i+1;
        }

        free_head = 0;
        num_added = 0;

        last_avail_idx = 0;
        last_used_idx  = 0;

        if(8 == sizeof(void*))
        {
            Virtio_SetIpcBaseAddress(((unsigned long)p)&0xFFFFFFFF00000000UL);
        }
        ASLOG(OFF,"vring[idx=%Xh,size=%d]: num=%d, desc=%s, avail=%s, used=%s, da=%Xh\n",
                ring->notifyid,
                size(),vr.num,ASHEX(vr.desc),
                ASHEX(vr.avail),ASHEX(vr.used),ring->da);
    }
    uint32_t size(void)
    {
        /*
         * ((12*256+2*(3+256)+4096-1)&~(4096-1))+2*3+8*256 = 6150
         */
        ASLOG(OFF,"sizeof(Vring_DescType)=%d,sizeof(Vring_UsedElemType)=%d,sizeof(uint16_t)=%d,num=%d,align=%d\n",
              sizeof(Vring_DescType),sizeof(Vring_UsedElemType),sizeof(uint16_t),ring->num,ring->align);
        return ((sizeof(Vring_DescType) * ring->num + sizeof(uint16_t) * (3 + ring->num)
             + ring->align - 1) & ~(ring->align - 1))
            + sizeof(uint16_t) * 3 + sizeof(Vring_UsedElemType) * ring->num;
    }
};
class Vdev: public QObject
{
Q_OBJECT
private:
    Rproc_ResourceVdevType* vdev;
    Vring* r_ring;
    Vring* w_ring;
public:
    explicit Vdev ( Rproc_ResourceVdevType* vdev ): vdev(vdev)
    {
        r_ring = new Vring(&vdev->vring[1]);
        w_ring = new Vring(&vdev->vring[0]);
    }
    void start(void)
    {
        emit kick(w_ring->get_notifyid());
    }

    bool notify(VirtQ_IdxType idx)
    {
          bool notifed = true;
          if(idx==r_ring->get_notifyid())
          {
              rx_noificaton();
          }
          else if(idx==w_ring->get_notifyid())
          {
              tx_confirmation();
          }
          else
          {
              notifed = false;
          }
          return notifed;
    }
public:
    bool provide_a_r_buffer(void* data,uint32_t len)
    {   /* thus the remote proc can send a messsage via this buffer */
        return r_ring->add_buf(data,len);
    }
    void* get_used_r_buf(VirtQ_IdxType* idx,uint32_t *len)
    {
        return r_ring->get_used_buf(idx,len);
    }
    void put_used_r_buf_back(VirtQ_IdxType idx)
    {
        r_ring->put_used_buf_back(idx);
    }
    VirtQ_IdxType get_r_notifyid(void)
    {
        return r_ring->get_notifyid();
    }

    bool provide_a_w_buffer(void* data,uint32_t len)
    {  /* thus a message in data was transimited to the retome proc */
        return w_ring->add_buf(data,len);
    }
    void* get_used_w_buf(VirtQ_IdxType* idx,uint32_t *len)
    {
        return w_ring->get_used_buf(idx,len);
    }
    void put_used_w_buf_back(VirtQ_IdxType idx)
    {
        w_ring->put_used_buf_back(idx);
    }
    VirtQ_IdxType get_w_notifyid(void)
    {
        return w_ring->get_notifyid();
    }

public:
    virtual void rx_noificaton(void){
        VirtQ_IdxSizeType idx;
        uint32_t len;
        void* buf;
        ASLOG(VDEV,"rx_notification(idx=%Xh)\n",r_ring->get_notifyid());
        buf = r_ring->get_used_buf(&idx,&len);

        assert(buf);
        ASLOG(VDEV,"Message(idx=%d,len=%d)\n",idx,len);
        asmem(buf,len);

        r_ring->put_used_buf_back(idx);

    }
    virtual void tx_confirmation(void){
        ASLOG(VDEV,"tx_confirmation(idx=%Xh)\n",w_ring->get_notifyid());
    }

signals:
    void kick(unsigned int idx);
};

class RPmsg: public Vdev
{
Q_OBJECT
private:

public:
    explicit RPmsg ( Rproc_ResourceVdevType* rpmsg ) : Vdev(rpmsg)
    {
        void* data;
        bool added;
        uint32_t len = sizeof(RPmsg_HandlerType);
        /* add read buffer to full */
        do
        {
            data = malloc(len);
            assert(data);
            memset(data,0,len);
            added = provide_a_r_buffer(data,len);
        } while(added);
        /* the last one must be added failed */
        free(data);
    }
public:
    void rx_noificaton(void){
        VirtQ_IdxSizeType idx;
        uint32_t len;
        RPmsg_HandlerType* buf;
        ASLOG(OFF,"rx_notification(idx=%Xh)\n",get_r_notifyid());
        buf = (RPmsg_HandlerType*)get_used_r_buf(&idx,&len);

        assert(buf);
        ASLOG(OFF,"Message(idx=%d,len=%d)\n",idx,len);
        ASLOG(OFF,"src=%Xh,dst=%Xh,flags=%Xh\n",buf->src,buf->dst,buf->flags);

        if((buf->src == 0xDEAD) && (buf->dst == 0xBEEF))
        {
            Can_RPmsgPduType * msg = (Can_RPmsgPduType *)buf->data;
            ASLOG(VIRTIO,"CAN ID=0x%08X LEN=%d DATA=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
                  msg->id,msg->length,msg->sdu[0],msg->sdu[1],msg->sdu[2],msg->sdu[3],
                    msg->sdu[4],msg->sdu[5],msg->sdu[6],msg->sdu[7]);
        }
        else
        {
            asmem(buf,len);
        }

        put_used_r_buf_back(idx);

    }
};

class Virtio: public QThread
{
Q_OBJECT
private:
    void* hxDll;

    size_t sz_fifo;
    void*  r_lock;
    void*  w_lock;
    void*  r_event;
    void*  w_event;

    Ipc_ChannelType chl;

    Ipc_FifoType* r_fifo;
    Ipc_FifoType* w_fifo;
    uint32_t r_pos;
    uint32_t w_pos;

    PF_IPC_IS_READY pfIsIpcReady;

    Rproc_ResourceTableType* rsc_tbl;

    QList<Vdev*> vdev_list;

#ifdef __LINUX__
    pthread_mutex_t w_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  w_cond  = PTHREAD_COND_INITIALIZER;
    pthread_cond_t  r_cond  = PTHREAD_COND_INITIALIZER;
#endif

public:
    explicit Virtio ( void* dll, QObject *parent = 0);
    ~Virtio ( );
private:
    void run(void);
    bool fifo_read(VirtQ_IdxType* id);
    bool fifo_write(VirtQ_IdxType id);
signals:

protected:

private slots:
    void kick(unsigned int idx);
};

/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
#endif /* RELEASE_ASCORE_VIRTUAL_AS_VIRTUAL_INCLUDE_VIRTIO_H_ */
