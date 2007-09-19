/* 
#include "xbdBlkDev.h" 
*/

#if 0
    BLK_DEV* blk_dev = virtualDiskCreate("/tmp/peghome", 512, 4000, 4000);
    device_t dev = xbdBlkDevCreate(blk_dev, "/ph");
#endif

    const size_t SECTOR_SIZE = 512;
    const size_t DISK_SIZE = 32 * (1024 * 1024);
        /* 512 * 1024; */

    /* Create RAM disk */
    {

        /* 512 byte sectors, 1024 sectors, no partition support */

        if (xbdRamDiskDevCreate(SECTOR_SIZE, DISK_SIZE, 0, "/ph") == 0)
            fprintf(stderr, "***************** failed to create RAM disk\n");
    }

    /* Format the entire RAM disk. Allow for 100 files */
    {
        if (hrfsFormat("/ph", 0, 0, 0) != 0)
            fprintf(stderr, "***************** failed to format RAM disk\n");
    }

