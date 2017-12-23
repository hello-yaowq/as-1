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
/* ============================ [ DATAS     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
VFS_FILE* vfs_fopen (const char *filename, const char *opentype)
{

}

int vfs_fclose (VFS_FILE* stream)
{

}

int vfs_fread (void *data, size_t size, size_t count, VFS_FILE *stream)
{

}

int vfs_fwrite (const void *data, size_t size, size_t count, VFS_FILE *stream)
{

}

int vfs_fflush (VFS_FILE *stream)
{

}

int vfs_fseek (VFS_FILE *stream, long int offset, int whence)
{

}

int vfs_unlink (const char *filename)
{

}

int vfs_stat (const char *filename, struct vfs_stat *buf)
{

}

VFS_DIR * vfs_opendir (const char *dirname)
{

}

struct vfs_dirent * vfs_readdir (VFS_DIR *dirstream)
{

}

int vfs_closedir (VFS_DIR *dirstream)
{

}

int vfs_chdir (const char *filename)
{

}

char * vfs_getcwd  (char *buffer, size_t size)
{

}

int vfs_mkdir (const char *filename, uint32_t mode)
{

}

int  vfs_rmdir (const char *filename)
{

}

int vfs_rename (const char *oldname, const char *newname)
{

}
