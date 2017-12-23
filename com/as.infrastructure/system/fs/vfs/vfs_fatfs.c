/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
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
#ifdef USE_FATFS
/* ============================ [ INCLUDES  ] ====================================================== */
#ifdef _WIN32
#undef _WIN32
#endif
#include "ff.h"
#ifdef __WINDOWS__
#define _WIN32
#endif
#include "vfs.h"
#include <string.h>
#include <stdlib.h>
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern const struct vfs_filesystem_ops fatfs_ops;
/* ============================ [ DATAS     ] ====================================================== */
/* dirent that will be given to callers;
 * note: both APIs assume that only one dirent ever exists
 */
static struct vfs_dirent dir_ent;
/* ============================ [ LOCALS    ] ====================================================== */
static VFS_FILE* fatfs_fopen (const char *filename, const char *opentype)
{
	VFS_FILE *f;
	BYTE flags = 0;
	FRESULT r;

	f = malloc(sizeof(VFS_FILE));
	if(NULL == f)
	{
		return NULL;
	}

	f->priv = malloc(sizeof(FIL));
	if(NULL == f->priv)
	{
		free(f);
		return NULL;
	}

	while (*opentype != '\0')
	{
		if (*opentype == 'r') flags |= FA_READ;
		if (*opentype == 'w') flags |= FA_WRITE | FA_CREATE_ALWAYS;
		opentype++;
	}

	r = f_open(f->priv, &filename[5], flags);
	if (FR_OK != r)
	{
		free(f->priv);
		free(f);
		return NULL;
	}
	else
	{
		f->fops = &fatfs_ops;
	}

	return f;
}

static int fatfs_fclose(VFS_FILE* stream)
{
	FRESULT r;

	r = f_close(stream->priv);
	if (FR_OK != r)
	{
		return EOF;
	}

	free(stream->priv);
	free(stream);

	return 0;
}

static int fatfs_fread (void *data, size_t size, size_t count, VFS_FILE *stream)
{
	UINT bytesread;
	FRESULT r;

	r = f_read(stream->priv, data, size*count, &bytesread);
	if (FR_OK != r)
	{
		return 0;
	}

	return bytesread;
}

static int fatfs_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream)
{
	UINT byteswritten;
	FRESULT r;

	r = f_write(stream->priv, data, size*count, &byteswritten);
	if (FR_OK != r)
	{
		return 0;
	}

	return byteswritten;
}

static int fatfs_fflush (VFS_FILE *stream)
{
	FRESULT r;

	r = f_sync(stream->priv);
	if (FR_OK == r)
	{
		return 0;
	}

	return EOF;
}

static int fatfs_fseek (VFS_FILE *stream, long int offset, int whence)
{
	FRESULT r;

	if(SEEK_SET != whence) return EOF;

	r = f_lseek(stream->priv, offset);
	if (FR_OK == r)
	{
		return 0;
	}

	return EOF;
}

static int fatfs_unlink (const char *filename)
{
	FRESULT r;

	r = f_unlink(filename);
	if (FR_OK == r)
	{
		return 0;
	}

	return EOF;
}

static int fatfs_stat (const char *filename, struct vfs_stat *buf)
{
	FILINFO f;
	FRESULT r;

	r = f_stat(filename, &f);

	if (FR_OK != r)
	{
		return ENOENT;
	}

	buf->st_size = f.fsize;

	buf->st_mode = 0;

	if(f.fattrib&AM_DIR)
	{
		buf->st_mode |= S_IFDIR;
	}
	else
	{
		buf->st_mode |= S_IFREG;
	}

	if(f.fattrib&AM_RDO)
	{
		buf->st_mode |= S_IEXEC|S_IREAD;
	}
	else
	{
		buf->st_mode |= S_IEXEC|S_IREAD|S_IWRITE;
	}

	buf->st_mtime = f.ftime;

	return 0;
}

static VFS_DIR * fatfs_opendir (const char *dirname)
{

	VFS_DIR* dir;
	FRESULT r;
	dir = malloc(sizeof(VFS_DIR));

	if(NULL != dir)
	{
		return NULL;
	}

	dir->priv = malloc(sizeof(DIR));
	if(NULL != dir)
	{
		free(dir);
		return NULL;
	}

	r = f_opendir(dir->priv, dirname);
	if (FR_OK != r)
	{
		free(dir->priv);
		free(dir);
		return NULL;
	}

	return dir;
}
static struct vfs_dirent* fatfs_readdir(VFS_DIR* dir)
{
	FILINFO fi;
	FRESULT r;

	r = f_readdir(dir->priv, &fi);

	if (FR_OK != r)
	{
		return NULL;
	}

	if (0 == fi.fname[0])
	{
		return NULL;
	}

	dir_ent.d_namlen = strnlen(fi.fname,sizeof(fi.fname));

	memcpy(dir_ent.d_name, fi.fname, dir_ent.d_namlen);

	return &dir_ent;
}

static int fatfs_closedir(VFS_DIR* dir)
{
	FRESULT r;

	r = f_closedir(dir->priv);

	if (FR_OK == r)
	{
		free(dir->priv);
		free(dir);
		return 0;
	}

	return EBADF;
}

/* ============================ [ FUNCTIONS ] ====================================================== */
const struct vfs_filesystem_ops fatfs_ops =
{
	.name = "/vfat",

};
#endif
