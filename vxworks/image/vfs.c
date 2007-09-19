/* 
#include "xbdBlkDev.h" 
void usrIosExtraInit (void)
*/

    /* Create virtual file */
#if 1
    {
        BLK_DEV* bd;
        device_t d;
        
        if (!(bd = virtualDiskCreate("/tmp/peghome", 512, 4000, 4000)))
            fprintf(stderr, "***************** virtualDiskCreate() failed\n");

        if ((d = xbdBlkDevCreate(bd, "/peg")) == 0)
            fprintf(stderr, "***************** xbdBlkDevCreate() failed\n");

int fd = open("/peg", O_RDONLY, 0777);
printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX fd=%d\n", fd);

        if (hrfsFormat("/peg:0", 0, 0, 0) != 0)
            fprintf(stderr, "***************** failed to format RAM disk\n");
    }
#else
    {
        BLK_DEV* bd;
        device_t d;

        if (!(bd = virtualDiskCreate("/tmp/peghome", 512, 32, 416)))
            fprintf(stderr, "***************** virtualDiskCreate() failed\n");

        if ((d = xbdBlkDevCreate(bd, "/mmm")) == 0)
            fprintf(stderr, "***************** xbdBlkDevCreate() failed\n");

        if (dosFsVolFormat("/mmm:0", DOS_OPT_BLANK, NULL) != 0)
            fprintf(stderr, "***************** failed to format RAM disk\n");
    }
#endif
