#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "xbdBlkDev.h"
#include "virtualDiskLib.h"
#include "hrFsLib.h"

static void pegasusVFSInit()
{
    BLK_DEV* blk_dev;
    device_t device;
    const size_t BYTES_PER_BLOCK = 1024;
    const size_t NUM_BLOCKS = (64 * 1024 * 1024) / BYTES_PER_BLOCK;
    int exists = 0;

    {
        int fd = open(PEGASUS_VFS_NAME, O_RDONLY, 0777);

        if (fd >= 0)
        {
            exists = 1;
            close(fd);
        }
    }

    if (!(blk_dev = virtualDiskCreate((char*)PEGASUS_VFS_NAME,
        BYTES_PER_BLOCK, NUM_BLOCKS, NUM_BLOCKS)))
    {
        fprintf(stderr, "********************************\n");
        fprintf(stderr, "**                            **\n");
        fprintf(stderr, "** virtualDiskCreate() failed **\n");
        fprintf(stderr, "**                            **\n");
        fprintf(stderr, "********************************\n");
        return;
    }

    if (!exists)
    {
        fprintf(stderr, "==== Created virtual file system: \"%s\"\n", 
            PEGASUS_VFS_NAME);
    }

    if ((device = xbdBlkDevCreateSync(blk_dev, PEGASUS_DEV_NAME)) == 0)
    {
        fprintf(stderr, "**********************************\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "** xbdBlkDevCreateSync() failed **\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "**********************************\n");
        return;
    }

    fprintf(stderr, "==== Created device: \"%s\"\n", PEGASUS_DEV_NAME);

    if (!exists)
    {
        if (hrfsFormat(PEGASUS_DEV_NAME ":0", 0, 0, 0) != 0)
        {
            fprintf(stderr, "*************************\n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "** hrfsFormat() failed **\n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "*************************\n");
            return;
        }

        fprintf(stderr, "==== Formatted virtual file system: \"%s\"\n", 
            PEGASUS_DEV_NAME ":0");
    }
}

void pegasusRamFsInit()
{
    BLK_DEV* blk_dev;
    device_t device;
    const size_t BYTES_PER_BLOCK = 1024;
    const size_t NUM_BLOCKS = (64 * 1024 * 1024) / BYTES_PER_BLOCK;
    int exists = 0;

    /*
     * Create RAM block device.
     */

    blk_dev = ramDevCreate (NULL, BYTES_PER_BLOCK, NUM_BLOCKS, NUM_BLOCKS,  0);

    if (!blk_dev)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** ramDevCreate() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Created RAM disk\n");
    taskDelay(10);

    if ((device = xbdBlkDevCreateSync(blk_dev, PEGASUS_DEV_NAME)) == 0)
    {
        fprintf(stderr, "**********************************\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "** xbdBlkDevCreateSync() failed **\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "**********************************\n");
        return;
    }

    fprintf(stderr, "==== Created device: \"%s\"\n", PEGASUS_DEV_NAME);

    if (!exists)
    {
        if (hrfsFormat(PEGASUS_DEV_NAME ":0", 0, 0, 0) != 0)
        {
            fprintf(stderr, "*************************\n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "** hrfsFormat() failed **\n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "*************************\n");
            return;
        }

        fprintf(stderr, "==== Formatted virtual file system: \"%s\"\n", 
            PEGASUS_DEV_NAME ":0");
    }
}

void pegasusInit()
{
    pegasusRamFsInit();
}

#if 0
void pegasusRamDriveInit()
{
    BLK_DEV* blk_dev;
    device_t device;
    int status;
    const size_t TOTAL_BLOCKS = 32 * 1024;

    /*
     * Create RAM block device.
     */

    blk_dev = ramDevCreate (NULL, 1024, TOTAL_BLOCKS, TOTAL_BLOCKS,  0);

    if (!blk_dev)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** ramDevCreate() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Created RAM disk\n");
    taskDelay(10);

    /*
     * Create RAM device.
     */

    device = xbdBlkDevCreateSync(blk_dev, "/ramfs");

    if (device == 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** ramDevCreate() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Created device: \"%s\"\n", "/ramfs");

    /*
     * Format RAM drive.
     */

    status = hrfsFormat("/ramfs:0", 0, 0, 0);

    if (status != 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** hrfsFormat() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Formatted ram drive: \"%s\"\n", 
        "/ramfs" ":0");

    if (chdir("/ramfs:0") != 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** chdir() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

}

void pegasusRamDriveInit()
{
    BLK_DEV* blk_dev;
    device_t device;
    int status;
    const size_t TOTAL_BLOCKS = 32 * 1024;

    /*
     * Create RAM block device.
     */

    blk_dev = ramDevCreate (NULL, 1024, TOTAL_BLOCKS, TOTAL_BLOCKS,  0);

    if (!blk_dev)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** ramDevCreate() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Created RAM disk\n");

    /*
     * Create RAM device.
     */

    device = xbdBlkDevCreateSync(blk_dev, "/ramfs");

    if (device == 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** ramDevCreate() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Created device: \"%s\"\n", "/ramfs");

    /*
     * Format RAM drive.
     */

    status = hrfsFormat("/ramfs:0", 0, 0, 0);

    if (status != 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** hrfsFormat() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

    fprintf(stderr, "==== Formatted ram drive: \"%s\"\n", 
        "/ramfs" ":0");

    if (chdir("/ramfs:0") != 0)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "**** chdir() failed\n");
        fprintf(stderr, "****\n");
        fprintf(stderr, "\n");
        return;
    }

}
#endif

#if 0
static int _copy_file(const char* src, const char* dest)
{
    char buffer[4096];
    size_t n;
    FILE* is;
    FILE* os;
    
    if ((is = fopen(src, "rb")) == NULL)
        return -1;

    if ((os = fopen(dest, "wb")) == NULL)
    {
        fclose(os);
        return -1;
    }

    while ((n = fread(buffer, 1, sizeof(buffer), is)) > 0)
    {
        if (fwrite(buffer, 1, n, os) != n)
        {
            fclose(is);
            fclose(os);
            return -1;
        }
    }

    fclose(is);
    fclose(os);
    return 0;
}

static int _recursive_copy(const char* src_path, const char* dest_path)
{
    DIR* dir;
    struct dirent* ent;
    char* dirs_src[MAX_DIRS];
    char* dirs_dest[MAX_DIRS];
    size_t size = 0;
    size_t i;

    /* Create destination directory if it does not already exist. */
    {
        struct stat st;

        if (stat(dest_path, &st) == 0)
        {
            if (!S_ISDIR(st.st_mode))
                return -1;
        }
        else if (mkdir(dest_path) != 0)
            return -1;
    }

    dir = opendir(src_path);

    if (!dir)
        return -1;

    while ((ent = readdir(dir)))
    {
        struct stat st;
        char src[4096];
        char dest[4096];

        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        sprintf(src, "%s/%s", src_path, ent->d_name);
        sprintf(dest, "%s/%s", dest_path, ent->d_name);

#if 0
        printf("%s -> %s\n", src, dest);
#endif

        if (stat(src, &st) != 0)
        {
            closedir(dir);
            return-1;
        }

        if (S_ISDIR(st.st_mode))
        {
            if (size == MAX_DIRS)
                return -1;

            if (mkdir(dest) != 0)
                return -1;

            dirs_src[size] = strdup(src);
            dirs_dest[size] = strdup(dest);
            size++;
        }
        else
        {
            if (_copy_file(src, dest) != 0)
                return -1;
        }
    }

    closedir(dir);

    for (i = 0; i < size; i++)
    {
        int status;
        
        status = _recursive_copy(dirs_src[i], dirs_dest[i]);
        free(dirs_src[i]);
        free(dirs_dest[i]);

        if (status != 0)
            return -1;
    }

    return 0;
}

void pegasusRepositoryInit()
{
    taskDelay(10);

    if (chdir("/ramfs:0") != 0)
    {
        fprintf(stderr, "pegasusRepositoryInit(): chdir() failed\n");
        return;
    }

    printf("+++++ Copying repository from rom to ram\n");

    if (_recursive_copy("/romfs/repository", "/ramfs:0/repository") != 0)
    {
        fprintf(stderr, "pegasusRepositoryInit(): failed\n");
        return;
    }

    printf("+++++ DONE\n");
}
#endif
