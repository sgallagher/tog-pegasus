/*
//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//%/////////////////////////////////////////////////////////////////////////////
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "File.h"
#include "User.h"
#include "Log.h"
#include "Strlcpy.h"
#include "Strlcat.h"

/*
**==============================================================================
**
** ChangeDirOwnerRecursive()
**
**==============================================================================
*/

int ChangeDirOwnerRecursive(
    const char* path,
    int uid,
    int gid)
{
    struct dirent* ent;
    DIR* dir;

    /* Change permission of this direcotry. */

    /* Flawfinder: ignore */
    if (chown(path, uid, gid) != 0)
        return -1;

    /* Open directory: */

    dir = opendir(path);

    if (dir == NULL)
        return -1;

    /* For each node in this directory: */

    while ((ent = readdir(dir)) != NULL)
    {
        struct stat st;
        char buffer[EXECUTOR_BUFFER_SIZE];

        /* Skip over "." and ".." */

        const char* name = ent->d_name;

        if (strcmp(name, ".")  == 0 || strcmp(name, "..") == 0)
            continue;

        /* Build full path name for this file. */

        Strlcpy(buffer, path, EXECUTOR_BUFFER_SIZE);
        Strlcat(buffer, "/", EXECUTOR_BUFFER_SIZE);
        Strlcat(buffer, name, EXECUTOR_BUFFER_SIZE);

        /* Determine file type (skip soft links and directories). */

        if (lstat(buffer, &st) == -1)
            return -1;

        /* If it's a directory, save the name. */

        if (S_ISDIR(st.st_mode))
        {
            ChangeDirOwnerRecursive(buffer, uid, gid);
            continue;
        }

        /* Skip soft links. */

        if (S_ISLNK(st.st_mode))
            continue;

        /* Process the current file. */

        /* Flawfinder: ignore */
        if (chown(buffer, uid, gid) != 0)
            return -1;
    }

    /* Close this directory. */

    closedir(dir);

    return 0;
}

/*
**==============================================================================
**
** AccessDir()
**
**     Returns 0 if able to stat given path and it is a directory.
**
**==============================================================================
*/

int AccessDir(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
        return -1;

    return 0;
}
