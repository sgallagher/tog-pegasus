#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "xbdBlkDev.h"
#include "virtualDiskLib.h"
#include "hrFsLib.h"

void usrAppInit()
{
    BLK_DEV* _vfs_blk_dev;
    device_t _vfs_device;
    const char PATH[] = "/tmp/pegasus.vfs";
    const size_t BYTES_PER_BLOCK = 512;
    const size_t NUM_BLOCKS = 131072;
    int exists = 0;

    {
        int fd = open(PATH, O_RDONLY, 0777);

        if (fd >= 0)
        {
            exists = 1;
            close(fd);
        }
    }

    if (!(_vfs_blk_dev = virtualDiskCreate((char*)PATH, 
        BYTES_PER_BLOCK, NUM_BLOCKS, NUM_BLOCKS)))
    {
        fprintf(stderr, "********************************\n");
        fprintf(stderr, "**                            **\n");
        fprintf(stderr, "** virtualDiskCreate() failed **\n");
        fprintf(stderr, "**                            **\n");
        fprintf(stderr, "********************************\n");
    }

    if ((_vfs_device = xbdBlkDevCreateSync(_vfs_blk_dev, "/peg")) == 0)
    {
        fprintf(stderr, "**********************************\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "** xbdBlkDevCreateSync() failed **\n");
        fprintf(stderr, "**                              **\n");
        fprintf(stderr, "**********************************\n");
    }

    if (!exists)
    {
        if (hrfsFormat("/peg:0", 0, 0, 0) != 0)
        {
            fprintf(stderr, "*************************n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "** hrfsFormat() failed **\n");
            fprintf(stderr, "**                     **\n");
            fprintf(stderr, "*************************n");
        }
    }

#if 0
    xbdBlkDevDelete(_vfs_device, &_vfs_blk_dev);
    virtualDiskClose(_vfs_blk_dev);
#endif
}
