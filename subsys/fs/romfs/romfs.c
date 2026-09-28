#include <stdlib.h>
#include <zephyr/fs/romfs.h>
#include <stdarg.h>
#include <zephyr/storage/disk_access.h>

static const char * const pdrv_str[] = {"NOR1"};

// default romfs address
static void *romfs_addr = (void *)0x04400000;

void romfs_mount(void *addr)
{
    printk("romfs_mount: %p\n", addr);
    romfs_addr = addr;
}

int check_dirent(struct romfs_dirent *dirent)
{
    if ((dirent->type != ROMFS_DIRENT_FILE && dirent->type != ROMFS_DIRENT_DIR)
        || dirent->size == ~0)
    {
        return -1;
    }
    return 0;
}

struct romfs_dirent *romfs_lookup(struct romfs_dirent *root_dirent, const char *path, size_t *size)
{
    size_t index, found;
    const char *subpath, *subpath_end;
    struct romfs_dirent *dirent;
    size_t dirent_size;

    /* Check the root_dirent. */
    if (check_dirent(root_dirent) != 0)
    {
        return NULL;
    }

    if (path)
    {
        if (path[0] == '/' && path[1] == '\0')
        {
            *size = root_dirent->size;
            return root_dirent;
        }
    }

    /* goto root directory entries */
    dirent = (struct romfs_dirent *)root_dirent->data;
    dirent_size = root_dirent->size;

    /* get the end position of this subpath */
    subpath_end = path;
    /* skip /// */
    if (subpath_end)
    {
        while (*subpath_end == '/')
        {
            subpath_end ++;
        }
        subpath = subpath_end;
        while ((*subpath_end != '/') && *subpath_end)
        {
            subpath_end ++;
        }
    }

    while (dirent != NULL)
    {
        found = 0;

        /* search in folder */
        for (index = 0; index < dirent_size; index ++)
        {
            if (check_dirent(&dirent[index]) != 0)
            {
                return NULL;
            }
            if (subpath_end && subpath && (subpath_end - subpath) >= 0)
            {
                if (strlen(dirent[index].name) == (subpath_end - subpath) &&
                    strncmp(dirent[index].name, subpath, (subpath_end - subpath)) == 0)
                {
                    dirent_size = dirent[index].size;

                    /* skip /// */
                    if (subpath_end)
                    {
                        while (*subpath_end == '/')
                        {
                            subpath_end ++;
                            if (!subpath_end)
                            {
                                break;
                            }
                        }
                        subpath = subpath_end;
                        while ((*subpath_end != '/') && *subpath_end)
                        {
                            subpath_end ++;
                        }
                    }
                    char sp = 0;
                    if (subpath != NULL)
                    {
                        sp = *subpath;
                    }
                    if (!(sp))
                    {
                        *size = dirent_size;
                        return &dirent[index];
                    }

                    if (dirent[index].type == ROMFS_DIRENT_DIR)
                    {
                        /* enter directory */
                        dirent = (struct romfs_dirent *)dirent[index].data;
                        found = 1;
                        break;
                    }
                    else
                    {
                        /* return file dirent */
                        //if (subpath != NULL)
                        {
                            break;    /* not the end of path */
                        }

                        //return &dirent[index];
                    }
                }
            }
        }

        if (!found)
        {
            break;    /* not found */
        }
    }

    /* not found */
    return NULL;
}

int romfs_read(struct romfs_fd *file, void *buf, size_t count)
{
    size_t length;
    struct romfs_dirent *dirent;

    dirent = (struct romfs_dirent *)file->data;

    if (check_dirent(dirent) != 0)
    {
        return -EIO;
    }

    if (count < file->size - file->pos)
    {
        length = count;
    }
    else
    {
        length = file->size - file->pos;
    }

    if (length > 0)
    {
        memcpy(buf, &(dirent->data[file->pos]), length);
    }

    /* update file current position */
    file->pos += length;

    return length;
}

int romfs_lseek(struct romfs_fd *file, off_t offset)
{
    if (offset <= file->size)
    {
        file->pos = offset;
        return file->pos;
    }

    return -EIO;
}

int romfs_close(struct romfs_fd *file)
{
    file->data = NULL;
    return 0;
}

int romfs_open(struct romfs_fd *file)
{
    size_t size;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;

    if (file)
    {
        root_dirent = file->data;

        if (check_dirent(root_dirent) != 0)
        {
            return -EIO;
        }

        if (file->flags & (O_CREAT | O_WRONLY | O_APPEND | O_TRUNC | O_RDWR))
        {
            return -FS_EINVAL;
        }

        dirent = romfs_lookup(root_dirent, file->path, &size);
        if (dirent == NULL)
        {
            return -ENOENT;
        }

        /* entry is a directory file type */
        if (dirent->type == ROMFS_DIRENT_DIR)
        {
            if (!(file->flags & O_DIRECTORY))
            {
                return -ENOENT;
            }
        }
        else
        {
            /* entry is a file, but open it as a directory */
            if (file->flags & O_DIRECTORY)
            {
                return -ENOENT;
            }
        }

        file->data = dirent;
        file->size = size;
        file->pos = 0;
    }

    return 0;
}
