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
#ifdef USE_LWEXT4
/* ============================ [ INCLUDES  ] ====================================================== */
#include "vfs.h"
#include "ext4.h"
#include "asdebug.h"
/* ============================ [ MACROS    ] ====================================================== */
#define AS_LOG_LWEXT 1
#define TO_LWEXT_PATH(f) (&((f)[4]))
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
extern const struct vfs_filesystem_ops lwext_ops;
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
static VFS_FILE* lwext_fopen (const char *filename, const char *opentype)
{
	(void)filename;
	(void)opentype;

	return NULL;
}

static int lwext_fclose (VFS_FILE* stream)
{
	(void)stream;

	return EACCES;
}

static int lwext_fread (void *data, size_t size, size_t count, VFS_FILE *stream)
{
	(void)data;
	(void)size;
	(void)count;
	(void)stream;

	return EACCES;
}

static int lwext_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream)
{
	(void)data;
	(void)size;
	(void)count;
	(void)stream;

	return EACCES;
}

static int lwext_fflush (VFS_FILE *stream)
{
	(void)stream;

	return EACCES;
}

static int lwext_fseek (VFS_FILE *stream, long int offset, int whence)
{
	(void)stream;
	(void)offset;
	(void)whence;

	return EACCES;
}

static int lwext_unlink (const char *filename)
{
	(void)filename;

	return EACCES;
}

static int lwext_stat (const char *filename, struct vfs_stat *buf)
{
	int r = ENOENT;

	ASLOG(LWEXT, "stat(%s)\n", filename);

	if(('\0' == TO_LWEXT_PATH(filename)[0])
		|| (0 == strcmp(TO_LWEXT_PATH(filename),"/")) )
	{	/* just the root */
		buf->st_mode = S_IFDIR;
		buf->st_size = 0;

		r = 0;
	}
	else
	{
		union {
			ext4_dir dir;
			ext4_file f;
		} var;

		r = ext4_dir_open(&(var.dir), TO_LWEXT_PATH(filename));

		if(0 == r)
		{
			(void) ext4_dir_close(&(var.dir));
			buf->st_mode = S_IFDIR;
			buf->st_size = 0;
		}
		else
		{
			r = ext4_fopen(&(var.f), TO_LWEXT_PATH(filename), "rb");
			if( 0 == r)
			{
				buf->st_mode = S_IFREG;
				buf->st_size = ext4_fsize(&(var.f));
				(void)ext4_fclose(&(var.f));
			}
		}
	}

	return r;
}

static VFS_DIR * lwext_opendir (const char *dirname)
{
	VFS_DIR* dir;

	ASLOG(LWEXT, "opendir(%s)\n", dirname);

	dir = malloc(sizeof(VFS_DIR));

	if(NULL != dir)
	{
		const char* p;
		dir->fops = &lwext_ops;
		dir->priv = malloc(sizeof(ext4_dir));

		if(('\0' == TO_LWEXT_PATH(dirname)[0]))
		{
			p = "/";
		}
		else
		{
			p = TO_LWEXT_PATH(dirname);
		}

		if(NULL != dir->priv)
		{
			int r;
			r = ext4_dir_open(dir->priv, p);

			if(0 != r)
			{
				free(dir->priv);
				free(dir);
				dir = NULL;
				ASLOG(LWEXT, "opendir failed!(%d)\n", r);
			}
		}
		else
		{
			free(dir);
			dir = NULL;
		}
	}

	return dir;

}

static struct vfs_dirent * lwext_readdir (VFS_DIR *dirstream)
{
	const ext4_direntry * rentry;
	struct vfs_dirent * rdirent;

	static struct vfs_dirent dirent;

	rentry = ext4_dir_entry_next(dirstream->priv);

	if(NULL != rentry)
	{
		dirent.d_namlen = rentry->name_length;

		strcpy(dirent.d_name, rentry->name);

		rdirent = &dirent;

	}
	else
	{
		rdirent = NULL;
	}

	return rdirent;

}

static int lwext_closedir (VFS_DIR *dirstream)
{
	(void)ext4_fclose(dirstream->priv);

	free(dirstream->priv);
	free(dirstream);

	return 0;
}

static int lwext_chdir (const char *filename)
{

	return 0;
}

static int lwext_mkdir (const char *filename, uint32_t mode)
{
	(void)filename;
	(void)mode;

	return EACCES;
}

static int lwext_rmdir (const char *filename)
{
	(void)filename;

	return EACCES;
}

static int lwext_rename (const char *oldname, const char *newname)
{
	(void)oldname;
	(void)newname;

	return EACCES;
}

/* ============================ [ FUNCTIONS ] ====================================================== */
const struct vfs_filesystem_ops lwext_ops =
{
	.name = "/ext",
	.fopen = lwext_fopen,
	.fclose = lwext_fclose,
	.fread = lwext_fread,
	.fwrite = lwext_fwrite,
	.fflush = lwext_fflush,
	.fseek = lwext_fseek,
	.unlink = lwext_unlink,
	.stat = lwext_stat,
	.opendir = lwext_opendir,
	.readdir = lwext_readdir,
	.closedir = lwext_closedir,
	.chdir = lwext_chdir,
	.mkdir = lwext_mkdir,
	.rmdir = lwext_rmdir,
	.rename = lwext_rename
};
#endif
