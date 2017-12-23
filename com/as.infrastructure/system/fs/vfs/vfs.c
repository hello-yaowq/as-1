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
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#ifdef USE_FATFS
extern const struct vfs_filesystem_ops fatfs_ops;
#endif
/* ============================ [ DATAS     ] ====================================================== */
static const struct vfs_filesystem_ops* vfs_ops[] =
{
#ifdef USE_FATFS
	&fatfs_ops,
#endif
	NULL
};

static char vfs_cwd[FILENAME_MAX] = "/";
/* ============================ [ LOCALS    ] ====================================================== */
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

	return ops;
}

static char* abs_path(const char * path)
{
	char* abspath;

	abspath = malloc(FILENAME_MAX);

	if(path[0] == '/')
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
			snprintf(abspath, FILENAME_MAX, "%s/%s", vfs_cwd, path);
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

	abspath = abs_path(filename);

	if(NULL != abspath)
	{
		ops = search_ops(abspath);
		if(NULL != ops)
		{
			rc = ops->chdir(abspath);
		}
		free(abspath);
	}

	return rc;
}

char * vfs_getcwd  (char *buffer, size_t size)
{
	size_t rsize = strlen(vfs_cwd);

	if(NULL == buffer)
	{
		size = rsize;
		buffer = malloc(strlen(vfs_cwd));
	}

	if(size < rsize)
	{
		buffer = NULL;
	}

	if(NULL != buffer)
	{
		strncpy(buffer,vfs_cwd,rsize);
		buffer[rsize-1] = '\0';
	}

	return buffer;
}

int vfs_mkdir (const char *filename, uint32_t mode)
{
	char* abspath;
	int rc = EACCES;
	const struct vfs_filesystem_ops *ops;

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
