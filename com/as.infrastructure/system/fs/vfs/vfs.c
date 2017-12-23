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
/* ============================ [ INCLUDES  ] ====================================================== */
#include "vfs.h"
#include <string.h>
#include <stdlib.h>
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_VFS 1
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#ifdef USE_FATFS
extern const struct vfs_filesystem_ops fatfs_ops;
#endif
static VFS_FILE* lvfs_fopen (const char *filename, const char *opentype);
static int lvfs_fclose (VFS_FILE* stream);
static int lvfs_fread (void *data, size_t size, size_t count, VFS_FILE *stream);
static int lvfs_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream);
static int lvfs_fflush (VFS_FILE *stream);
static int lvfs_fseek (VFS_FILE *stream, long int offset, int whence);

static int lvfs_unlink (const char *filename);
static int lvfs_stat (const char *filename, struct vfs_stat *buf);

static VFS_DIR * lvfs_opendir (const char *dirname);
static struct vfs_dirent * lvfs_readdir (VFS_DIR *dirstream);
static int lvfs_closedir (VFS_DIR *dirstream);

static int lvfs_chdir (const char *filename);
static int lvfs_mkdir (const char *filename, uint32_t mode);
static int lvfs_rmdir (const char *filename);
static int lvfs_rename (const char *oldname, const char *newname);
/* ============================ [ DATAS     ] ====================================================== */
static const struct vfs_filesystem_ops lvfs_ops =
{
	.name = "/",
	.fopen = lvfs_fopen,
	.fclose = lvfs_fclose,
	.fread = lvfs_fread,
	.fwrite = lvfs_fwrite,
	.fflush = lvfs_fflush,
	.fseek = lvfs_fseek,
	.unlink = lvfs_unlink,
	.stat = lvfs_stat,
	.opendir = lvfs_opendir,
	.readdir = lvfs_readdir,
	.closedir = lvfs_closedir,
	.chdir = lvfs_chdir,
	.mkdir = lvfs_mkdir,
	.rmdir = lvfs_rmdir,
	.rename = lvfs_rename
};

static const struct vfs_filesystem_ops* vfs_ops[] =
{
#ifdef USE_FATFS
	&fatfs_ops,
#endif
	/* must be the last one */
	&lvfs_ops,
	NULL
};

static char vfs_cwd[FILENAME_MAX] = "/";

/* ============================ [ LOCALS    ] ====================================================== */
static VFS_FILE* lvfs_fopen (const char *filename, const char *opentype)
{
	(void)filename;
	(void)opentype;

	return NULL;
}

static int lvfs_fclose (VFS_FILE* stream)
{
	(void)stream;

	return EACCES;
}

static int lvfs_fread (void *data, size_t size, size_t count, VFS_FILE *stream)
{
	(void)data;
	(void)size;
	(void)count;
	(void)stream;

	return EACCES;
}

static int lvfs_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream)
{
	(void)data;
	(void)size;
	(void)count;
	(void)stream;

	return EACCES;
}

static int lvfs_fflush (VFS_FILE *stream)
{
	(void)stream;

	return EACCES;
}

static int lvfs_fseek (VFS_FILE *stream, long int offset, int whence)
{
	(void)stream;
	(void)offset;
	(void)whence;

	return EACCES;
}

static int lvfs_unlink (const char *filename)
{
	(void)filename;

	return EACCES;
}

static int lvfs_stat (const char *filename, struct vfs_stat *buf)
{
	(void)filename;

	buf->st_mode = S_IFDIR;
	buf->st_size = 0;

	return 0;
}

static VFS_DIR * lvfs_opendir (const char *dirname)
{
	VFS_DIR* dir;

	dir = malloc(sizeof(VFS_DIR));

	if(NULL != dir)
	{
		dir->fops = &lvfs_ops;
		dir->priv = vfs_ops;
	}

	return dir;

}

static struct vfs_dirent * lvfs_readdir (VFS_DIR *dirstream)
{
	static struct vfs_dirent dirent;
	const struct vfs_filesystem_ops** ops;

	ops = dirstream->priv;

	if((&lvfs_ops) != (*ops))
	{
		dirent.d_namlen = strlen((*ops)->name);
		strcpy(dirent.d_name, (*ops)->name);
		dirstream->priv = ops+1;

		return &dirent;
	}

	return NULL;

}

static int lvfs_closedir (VFS_DIR *dirstream)
{
	free(dirstream);

	return 0;
}

static int lvfs_chdir (const char *filename)
{
	strncpy(vfs_cwd, filename, FILENAME_MAX);

	return 0;
}

static int lvfs_mkdir (const char *filename, uint32_t mode)
{
	(void)filename;
	(void)mode;

	return EACCES;
}

static int lvfs_rmdir (const char *filename)
{
	(void)filename;

	return EACCES;
}

static int lvfs_rename (const char *oldname, const char *newname)
{
	(void)oldname;
	(void)newname;

	return EACCES;
}

static const struct vfs_filesystem_ops* search_ops(const char *filename)
{
	const struct vfs_filesystem_ops *ops, **o;
	size_t fslen;

	o = vfs_ops;
	ops = NULL;
	while(*o != NULL)
	{
		fslen = strlen((*o)->name);
		if(0 == strncmp((*o)->name, filename, fslen))
		{
			ops = *o;
			break;
		}
		o++;
	}

	ASLOG(VFS, "search_ops(%s) = %s\n", filename, (NULL != ops) ? ops->name : NULL);

	return ops;
}

static char* abs_path(const char * path)
{
	char* abspath;

	abspath = malloc(FILENAME_MAX);

	if('/' == path[0])
	{
		if(NULL != abspath)
		{
			abspath = strncpy(abspath, path, FILENAME_MAX);
		}
	}
	else
	{
		if(NULL != abspath)
		{
			if('\0' == vfs_cwd[1])
			{
				abspath[0] = '/';
				strncpy(&abspath[1], path, FILENAME_MAX);
			}
			else
			{
				snprintf(abspath, FILENAME_MAX, "%s/%s", vfs_cwd, path);
			}
		}
	}

	return abspath;
}
/* ============================ [ FUNCTIONS ] ====================================================== */
VFS_FILE* vfs_fopen (const char *filename, const char *opentype)
{
	char* abspath;
	const struct vfs_filesystem_ops *ops;
	VFS_FILE* file = NULL;

	ASLOG(VFS, "fopen(%s,%s)\n", filename, opentype);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			file = ops->fopen(abspath, opentype);
		}
		free(abspath);
	}

	return file;
}

int vfs_fclose (VFS_FILE* stream)
{
	return stream->fops->fclose(stream);
}

int vfs_fread (void *data, size_t size, size_t count, VFS_FILE *stream)
{
	return stream->fops->fread(data, size, count, stream);
}

int vfs_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream)
{
	return stream->fops->fwrite(data, size, count, stream);
}

int vfs_fflush (VFS_FILE *stream)
{
	return stream->fops->fflush(stream);
}

int vfs_fseek (VFS_FILE *stream, long int offset, int whence)
{
	return stream->fops->fseek(stream, offset, whence);
}

int vfs_unlink (const char *filename)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "unlink(%s)\n", filename);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->unlink(abspath);
		}
		free(abspath);
	}

	return rc;
}

int vfs_stat (const char *filename, struct vfs_stat *buf)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "stat(%s)\n", filename);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->stat(abspath, buf);
		}
		free(abspath);
	}

	return rc;
}

VFS_DIR * vfs_opendir (const char *dirname)
{
	char* abspath;
	const struct vfs_filesystem_ops *ops;
	VFS_DIR* dir = NULL;

	ASLOG(VFS, "opendir(%s)\n", dirname);

	abspath = abs_path(dirname);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			dir = ops->opendir(abspath);
		}
		free(abspath);
	}

	return dir;
}

struct vfs_dirent * vfs_readdir (VFS_DIR *dirstream)
{
	return dirstream->fops->readdir(dirstream);
}

int vfs_closedir (VFS_DIR *dirstream)
{
	return dirstream->fops->closedir(dirstream);
}

int vfs_chdir (const char *filename)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "chdir(%s)\n", filename);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->chdir(abspath);
			if(0 == rc)
			{
				strncpy(vfs_cwd, abspath, FILENAME_MAX);
			}
		}
		free(abspath);
	}

	return rc;
}

char * vfs_getcwd  (char *buffer, size_t size)
{
	size_t rsize = strlen(vfs_cwd);

	ASLOG(VFS, "getcwd(%s)\n", vfs_cwd);

	if(NULL == buffer)
	{
		size = rsize+1;
		buffer = malloc(strlen(vfs_cwd));
	}

	if(size < rsize)
	{
		buffer = NULL;
	}

	if(NULL != buffer)
	{
		strncpy(buffer,vfs_cwd,size);
	}

	return buffer;
}

int vfs_mkdir (const char *filename, uint32_t mode)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "mkdir(%s, 0x%x)\n", filename, mode);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->mkdir(abspath, mode);
		}
		free(abspath);
	}

	return rc;
}

int  vfs_rmdir (const char *filename)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "rmdir(%s)\n", filename);

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->rmdir(abspath);
		}
		free(abspath);
	}

	return rc;
}

int vfs_rename (const char *oldname, const char *newname)
{
	char* abspath_old;
	char* abspath_new;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

	ASLOG(VFS, "rename(%s,%s)\n", oldname, newname);

	abspath_old = abs_path(oldname);
	abspath_new = abs_path(newname);

	if(NULL != abspath_old)
	{
		if(NULL != abspath_new)
		{
			ops = search_ops(abspath_old);
			if(NULL != ops)
			{
				rc = ops->rename(abspath_old,abspath_new);
			}
			free(abspath_new);
		}
		free(abspath_old);
	}

	return rc;
}
