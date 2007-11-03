#include <vxWorks.h>
#include <stdio.h>
#include <hostLib.h>
#include <ioLib.h>
#include <loadLib.h>
#include <taskLib.h>
#include <nfsDriver.h>
#include <symLib.h>
#include "prjParams.h"


/******************************************************************************
*
* usrAppInit - initialize the users application
*/ 

IMPORT SYMTAB_ID sysSymTbl;

static void load_cimserver(const char* root)
{
    char path[1024];
    int fd;
    char* value;
    SYM_TYPE type;



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

    if (taskSpawn("cimsrv", 100,0x19, 64536, 
        (FUNCPTR)value, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR)
    {
        printf("***** taskSpawn() failed\n");
        return;
    }
}

void usrAppInit (void)
{
    const char path[] = "/share";
    int rc;

    /* 
     * NFS mount 
     */

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

    /*
     * Load cimserver:
     */

    load_cimserver(path);
}
