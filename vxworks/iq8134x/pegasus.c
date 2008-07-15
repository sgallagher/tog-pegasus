#include <vxWorks.h>
#include <stdio.h>
#include <hostLib.h>
#include <ioLib.h>
#include <loadLib.h>
#include <taskLib.h>
#include <nfsDriver.h>
#include <symLib.h>
#include "prjParams.h"

IMPORT SYMTAB_ID sysSymTbl;

static void _run_cimserver(const char* root)
{
    char path[1024];
    int fd;
    char* value;
    SYM_TYPE type;
    const char CERT[] = "/romfs/ssl/certs/file.pem";

    if ((fd = open(CERT, O_RDONLY, 0)) < 0)
    {
        printf("***** failed to open %s\n", CERT);
        return;
    }

    close(fd);

    /* Load cimserver module */

    sprintf(path, "%s/cimserver", root);

    if ((fd = open(path, O_RDONLY, 0)) < 0)
    {
        printf("***** open() failed\n");
        return;
    }



    if (loadModule(fd, LOAD_ALL_SYMBOLS) == NULL)
    {
        printf("***** loadModule() failed\n");
        return;
    }

    close(fd);

    /* Lookup cimserver entry point. */


    if (symFindByName(sysSymTbl, "cimserver", &value, &type) != 0)
    {
        printf("***** symFindByName() failed\n");
        return;
    }

    /* spawn cimserver task */

    if (taskSpawn("cimsrv", 100,0x19, 2 * 64536, 
        (FUNCPTR)value, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR)
    {
        printf("***** taskSpawn() failed\n");
        return;
    }
}

static void _mount_nfs(const char* path)
{
    int rc;

    rc = hostAdd("nfshost", "192.168.1.20");

    if (rc != 0)
    {
        printf("***** hostAdd() failed\n");
        return;
    }

    rc = nfsMount("nfshost", path, path);

    if (rc != 0)
    {
        printf("***** nfsMount() failed\n");
        return;
    }
}

void pegasusInit()
{
    const char nfs_root[] = "/share";

    _mount_nfs(nfs_root);

    _run_cimserver(nfs_root);
}
