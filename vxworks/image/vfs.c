#include "xbdBlkDev.h" 

static int _exists(const char* path)
{
    int fd = open(path, O_RDONLY, 0777);

    if (fd == -1)
        return 0;

    close(fd);
    return 1;
}

static BLK_DEV* _vfs_blk_dev;
static device_t _vfs_device;

/*
 * Shutdown function, executed atexit().
 */
static void pegasus_vxsim_shutdown()
{
    fprintf(stderr, "****************************\n");
    fprintf(stderr, "** pegasus_vxsim_shutdown **\n");
    fprintf(stderr, "****************************\n");
    xbdBlkDevDelete(_vfs_device, &_vfs_blk_dev);
    virtualDiskClose(_vfs_blk_dev);
}

/*
 * Add a call to this function to end of usrIosExtraInit()
 */
static void pegasus_vxsim_init()
{
    const char PATH[] = "/tmp/pegasus.vfs";
    const size_t BYTES_PER_BLOCK = 512;
    const size_t NUM_BLOCKS = 131072;
    int exists;

    exists = _exists(PATH);

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

    atexit(pegasus_vxsim_shutdown);
}
