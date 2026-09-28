/*
* COPYRIGHT 2025 Realtek Semiconductor Corporation.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <errno.h>
#include <zephyr/init.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/fs_sys.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/romfs.h>

LOG_MODULE_DECLARE(fs, CONFIG_FS_LOG_LEVEL);

/* Memory pool for ROMFS file objects */
K_MEM_SLAB_DEFINE(romfs_filep_pool, sizeof(struct romfs_fd),
            CONFIG_FS_ROMFS_NUM_FILES, 4);

static void *romfs_addr = (void *)0x04400000;

static int translate_error(int error)
{
    switch (error) {
    case 0:
        return 0;
    case -ENOENT:
        return -ENOENT;
    case -EIO:
        return -EIO;
    case -FS_EINVAL:
        return -EINVAL;
    default:
        return -EIO;
    }
}

static int translate_disk_error(int error)
{
    switch (error) {
    case 0:
        return 0;
    default:
        return -EIO;
    }
}

/* Converts a zephyr path like /SD:/foo into a path digestible by ROMFS by stripping the
* leading slash, i.e. /foo.
*/
static const char *translate_path(const char *path)
{
    /* this is guaranteed by the fs subsystem */
    __ASSERT_NO_MSG(path[0] == '/');
    /*
     * /SD:/dir1 -> /dir1
     * /SD/dir1 -> /dir1
     */
    if (path[0] == '/')
    {
        const char *first_slash = path;
        const char *next_slash = strchr(first_slash + 1, '/');
        const char *colon_pos = strchr(first_slash + 1, ':');
        
        if (next_slash != NULL)
        {
            if (next_slash > first_slash + 1)
            {
                // if /xxx/yyy/...，skip root path
                return next_slash;
            }
        }
        else if (colon_pos != NULL)
        {
            // 检查冒号后是否紧跟'/'
            if (colon_pos[1] == '/')
            {
                // if /xxx:/yyy/...，skip root path and colon
                return colon_pos + 1;
            }
        }
    }
    
    return &path[1];
}

static uint8_t translate_flags(fs_mode_t flags)
{
    uint8_t romfs_mode = 0;

    romfs_mode |= (flags & FS_O_READ) ? O_RDONLY : 0;
    romfs_mode |= (flags & FS_O_WRITE) ? O_WRONLY : 0;
    romfs_mode |= (flags & FS_O_CREATE) ? O_CREAT : 0;
    
    return romfs_mode;
}

static void *romfs_get_root_addr(void)
{
    /* In a real implementation, this would return the actual root address of the ROMFS */
    /* For now, we'll return a default address as defined in romfs.c */
    return (void *)romfs_addr;
}

static int _romfs_open(struct fs_file_t *zfp, const char *file_name,
            fs_mode_t mode)
{
    int res = 0;
    uint8_t fs_mode;
    struct romfs_fd *filep;

    if (k_mem_slab_alloc(&romfs_filep_pool, (void **)&filep, K_NO_WAIT) == 0) {
        (void)memset((void *)filep, 0, sizeof(struct romfs_fd));
        zfp->filep = (void *)filep;
    } else {
        return -ENOMEM;
    }

    fs_mode = translate_flags(mode);

    filep->flags = fs_mode;
    filep->size  = 0;
    filep->pos   = 0;
    filep->data  = (void *)romfs_get_root_addr();
    filep->path = (char *)translate_path(file_name);

    res = romfs_open(zfp->filep);

    if (res != 0) {
        k_mem_slab_free(&romfs_filep_pool, (void *)filep);
        zfp->filep = NULL;
    }

    return translate_error(res);
}

static int _romfs_close(struct fs_file_t *zfp)
{
    int res;

    res = romfs_close(zfp->filep);

    /* Free file ptr memory */
    k_mem_slab_free(&romfs_filep_pool, zfp->filep);
    zfp->filep = NULL;

    return translate_error(res);
}

static ssize_t _romfs_read(struct fs_file_t *zfp, void *ptr, size_t size)
{
    int res;

    res = romfs_read(zfp->filep, ptr, size);
    if (res < 0) {
        return translate_error(res);
    }

    return res;
}

static int _romfs_seek(struct fs_file_t *zfp, off_t offset, int whence)
{
    int res = 0;
    off_t pos;
    struct romfs_fd *filep = zfp->filep;

    switch (whence) {
    case FS_SEEK_SET:
        pos = offset;
        break;
    case FS_SEEK_CUR:
        pos = filep->pos + offset;
        break;
    case FS_SEEK_END:
        pos = filep->size + offset;
        break;
    default:
        return -EINVAL;
    }

    if ((pos < 0) || (pos > filep->size)) {
        return -EINVAL;
    }

    res = romfs_lseek(zfp->filep, pos);

    return translate_error(res);
}

static off_t _romfs_tell(struct fs_file_t *zfp)
{
    struct romfs_fd *filep = zfp->filep;
    return filep->pos;
}

static int _romfs_opendir(struct fs_dir_t *zdp, const char *path)
{
    int res = 0;
    struct romfs_fd *filep;
    struct fs_file_t zfp;
    struct romfs_dirent *dirent;
    size_t size;

    if (k_mem_slab_alloc(&romfs_filep_pool, (void **)&filep, K_NO_WAIT) == 0) {
        (void)memset((void *)filep, 0, sizeof(struct romfs_fd));
        zdp->dirp = (void *)filep;
    } else {
        return -ENOMEM;
    }

    /* Prepare file descriptor for directory opening */
    filep->flags = O_RDONLY | O_DIRECTORY;
    filep->size = 0;
    filep->pos = 0;
    filep->data = romfs_get_root_addr();
    filep->path = (char *)translate_path(path);

    /* Try to open the directory */
    dirent = romfs_lookup((struct romfs_dirent *)filep->data, filep->path, &size);
    if (dirent == NULL) {
        k_mem_slab_free(&romfs_filep_pool, filep);
        zdp->dirp = NULL;
        return -ENOENT;
    }

    /* Check if it's actually a directory */
    if (dirent->type != ROMFS_DIRENT_DIR) {
        k_mem_slab_free(&romfs_filep_pool, filep);
        zdp->dirp = NULL;
        return -ENOTDIR;
    }

    filep->data = dirent;
    filep->size = size;

    return res;
}

static int _romfs_readdir(struct fs_dir_t *zdp, struct fs_dirent *entry)
{
    struct romfs_fd *filep = (struct romfs_fd *)zdp->dirp;
    struct romfs_dirent *dirent;
    static struct dirent rom_entry; /* Static to maintain state */

    /* Check if we've reached the end of the directory entries */
    if (filep->pos >= filep->size) {
        return -ENOENT;
    }

    /* Get the current directory entry */
    dirent = (struct romfs_dirent *)((uint8_t *)filep->data + filep->pos);

    /* Basic validation */
    if (check_dirent(dirent) != 0) {
        return -EIO;
    }

    /* Prepare the Zephyr directory entry */
    memset(entry, 0, sizeof(*entry));
    strncpy(entry->name, dirent->name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';

    /* Set file type */
    if (dirent->type == ROMFS_DIRENT_DIR) {
        entry->type = FS_DIR_ENTRY_DIR;
    } else if (dirent->type == ROMFS_DIRENT_FILE) {
        entry->type = FS_DIR_ENTRY_FILE;
    } else {
        entry->type = 0;
    }

    /* Set file size */
    entry->size = dirent->size;

    /* Move to next directory entry */
    filep->pos += sizeof(struct romfs_dirent);

    return 0;
}

static int _romfs_closedir(struct fs_dir_t *zdp)
{
    /* Free the memory allocated for ROMFS directory descriptor */
    k_mem_slab_free(&romfs_filep_pool, zdp->dirp);
    zdp->dirp = NULL;

    return 0;
}

static int _romfs_mount(struct fs_mount_t *mountp)
{
    int res = 0;

    /* Mount the ROMFS using ROMFS API */
    romfs_mount(mountp->fs_data);
    
    mountp->flags |= FS_MOUNT_FLAG_USE_DISK_ACCESS;

    romfs_addr = mountp->fs_data;

    return translate_error(res);

}

static int _romfs_stat(struct fs_mount_t *mountp, const char *path, struct fs_dirent *entry)
{
    struct romfs_dirent *dirent;
    size_t size;

    /* Use romfs_lookup to find the file/directory */
    dirent = romfs_lookup((struct romfs_dirent *)romfs_get_root_addr(), 
                         translate_path(path), &size);
    if (dirent == NULL) {
        return -ENOENT;
    }

    /* Basic validation */
    if (check_dirent(dirent) != 0) {
        return -EIO;
    }
    if(entry != NULL)
    {
        /* Convert ROMFS directory entry to Zephyr dirent */
        memset(entry, 0, sizeof(struct fs_dirent));
    }

    /* Set file type */
    if (dirent->type == ROMFS_DIRENT_DIR) {
        entry->type = FS_DIR_ENTRY_DIR;
    } else if (dirent->type == ROMFS_DIRENT_FILE) {
        entry->type = FS_DIR_ENTRY_FILE;
    } else {
        entry->type = 0;
    }

    /* Set file size */
    entry->size = dirent->size;

    /* Set file name */
    strncpy(entry->name, dirent->name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';

    return 0;
}

/* File system interface */
static const struct fs_file_system_t _romfs_fs = {
    .open = _romfs_open,
    .close = _romfs_close,
    .read = _romfs_read,
    .write = NULL,
    .lseek = _romfs_seek,
    .tell = _romfs_tell,
    .truncate = NULL,
    .sync = NULL,
    .opendir = _romfs_opendir,
    .readdir = _romfs_readdir,
    .closedir = _romfs_closedir,
    .mount = _romfs_mount,
    .unmount = NULL,
    .unlink = NULL,
    .rename = NULL,
    .mkdir = NULL,
    .stat = _romfs_stat,
    .statvfs = NULL,
};

static int _romfs_init(void)
{
    return fs_register(FS_TYPE_EXTERNAL_BASE, &_romfs_fs);
}

SYS_INIT(_romfs_init, POST_KERNEL, CONFIG_FILE_SYSTEM_INIT_PRIORITY);
