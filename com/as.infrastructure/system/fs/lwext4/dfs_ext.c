/*
 * File      : dfs_ext.c
 * This file is part of Device File System in RT-Thread RTOS
 * COPYRIGHT (C) 2004-2015, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-11     parai@foxmail.com base porting
 */

#include <rtthread.h>
#include <dfs.h>
#include <dfs_fs.h>
#include <dfs_file.h>

#include "dfs_ext.h"

#include "ext4.h"
#include "ext4_config.h"
#include "ext4_blockdev.h"
#include "ext4_errno.h"

static int blockdev_open(struct ext4_blockdev *bdev);
static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
			 uint32_t blk_cnt);
static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
			  uint64_t blk_id, uint32_t blk_cnt);
static int blockdev_close(struct ext4_blockdev *bdev);
static int blockdev_lock(struct ext4_blockdev *bdev);
static int blockdev_unlock(struct ext4_blockdev *bdev);

EXT4_BLOCKDEV_STATIC_INSTANCE(ext4_blkdev, 4096, 0, blockdev_open,
			      blockdev_bread, blockdev_bwrite, blockdev_close,
			      blockdev_lock, blockdev_unlock);

static rt_device_t disk[RT_DFS_EXT_DRIVES] = {0};

static int get_disk(rt_device_t id)
{
    int index;

    for (index = 0; index < RT_DFS_EXT_DRIVES; index ++)
    {
        if (disk[index] == id)
            return index;
    }

    return -1;
}

int dfs_ext_mount(struct dfs_filesystem* fs, unsigned long rwflag, const void* data)
{
	int rc;
	int index;
	char mp[4] = "/e0"; /*mount point */
	char img[8] = "asblk0";

	struct rt_device_blk_geometry geometry;

	/* get an empty position */
	index = get_disk(RT_NULL);
	if (index == -1)
		return -ENOENT;

	mp[2] = '0' + index;
	img[5] = '0' + index;


	rc = ext4_device_register(&ext4_blkdev, img);
	if(EOK == rc)
	{
		rc = ext4_mount(img, mp, false);

		if(EOK == rc)
		{
			disk[index] = fs->dev_id;
		}
	}

	return rc;
}

int dfs_ext_unmount(struct dfs_filesystem* fs)
{
	int  index;
	int rc;
	char mp[4] = "/e0"; /*mount point */
	/* find the device index and then umount it */
	index = get_disk(fs->dev_id);
	if (index == -1) /* not found */
		return -ENOENT;

	mp[2] = '0' + index;

	rc = ext4_umount(mp);

	return rc;
}

int dfs_ext_mkfs(rt_device_t devid)
{
	int  index;
	int rc;

	if (devid == RT_NULL)
	{
		return -EINVAL;
	}
	/* find the device index and then umount it */
	index = get_disk(devid);
	if (index == -1) /* not found */
		return -ENOENT;

	rc = EPERM; /* FIXME */

	return rc;
}
int dfs_ext_ioctl(struct dfs_fd* file, int cmd, void* args)
{
    return -RT_EIO;
}

int dfs_ext_read(struct dfs_fd *fd, void *buf, size_t count)
{
    return count;
}

int dfs_ext_write(struct dfs_fd *fd, const void *buf, size_t count)
{
	return count;
}

int dfs_ext_flush(struct dfs_fd *fd)
{
	return RT_EOK;
}
int dfs_ext_lseek(struct dfs_fd* file, rt_off_t offset)
{
    return -RT_EIO;
}

int dfs_ext_close(struct dfs_fd* file)
{
    return RT_EOK;
}

int dfs_ext_open(struct dfs_fd* file)
{
    return RT_EOK;
}

int dfs_ext_stat(struct dfs_filesystem* fs, const char *path, struct stat *st)
{
    return RT_EOK;
}

int dfs_ext_getdents(struct dfs_fd* file, struct dirent* dirp, rt_uint32_t count)
{
    return count * sizeof(struct dirent);
}

static const struct dfs_file_ops _ext_fops =
{
    dfs_ext_open,
    dfs_ext_close,
    dfs_ext_ioctl,
    dfs_ext_read,
	dfs_ext_write,
	dfs_ext_flush,
    dfs_ext_lseek,
    dfs_ext_getdents,
};

static const struct dfs_filesystem_ops _ext_fs =
{
    "ext",
    DFS_FS_FLAG_DEFAULT,
    &_ext_fops,

    dfs_ext_mount,
    dfs_ext_unmount,
	dfs_ext_mkfs,
    NULL, /* statfs */

    NULL, /* unlink */
    dfs_ext_stat,
    NULL, /* rename */
};

int dfs_ext_init(void)
{
    /* register rom file system */
    dfs_register(&_ext_fs);
    return 0;
}
INIT_COMPONENT_EXPORT(dfs_ext_init);



static int blockdev_open(struct ext4_blockdev *bdev)
{
	uint32_t size;

	bdev->part_offset = 0;
	bdev->part_size = size;
	bdev->bdif->ph_bcnt = bdev->part_size / bdev->bdif->ph_bsize;

	return 0;

}

static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
			 uint32_t blk_cnt)
{

	return 0;
}


static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
			  uint64_t blk_id, uint32_t blk_cnt)
{
	return 0;
}

static int blockdev_close(struct ext4_blockdev *bdev)
{
	return 0;
}

static int blockdev_lock(struct ext4_blockdev *bdev)
{
	return 0;
}

static int blockdev_unlock(struct ext4_blockdev *bdev)
{
	return 0;
}
