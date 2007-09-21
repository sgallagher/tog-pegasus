#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "xbdBlkDev.h"
#include "virtualDiskLib.h"
#include "hrFsLib.h"

void pegasusInit()
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
