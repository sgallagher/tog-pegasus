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

//==============================================================================
//
// ChangeOwner()
//
//     Change the given file's owner.
//
//==============================================================================

int ChangeOwner(const char* path, const char* owner)
{
    int uid;
    int gid;

    if (GetUserInfo(owner, uid, gid) != 0)
        return -1;

    /* Flawfinder: ignore */
    if (chown(path, uid, gid) != 0)
    {
        Log(LL_TRACE, "chown(%s, %d, %d) failed", path, uid, gid);
        return -1;
    }

    return 0;
}

//==============================================================================
//
// ChangeDirOwnerRecursive()
//
//==============================================================================

int ChangeDirOwnerRecursive(
    const char* path,
    int uid,
    int gid)
{
    // Change permission of this direcotry.

    /* Flawfinder: ignore */
    if (chown(path, uid, gid) != 0)
        return -1;

    // Open directory:

    DIR* dir = opendir(path);

    if (dir == NULL)
        return -1;

    // For each node in this directory:

    dirent* ent;
    
    while ((ent = readdir(dir)) != NULL)
    {
        // Skip over "." and ".."

        const char* name = ent->d_name;

        if (strcmp(name, ".")  == 0 || strcmp(name, "..") == 0)
            continue;

        // Build full path name for this file:

        char buffer[EXECUTOR_BUFFER_SIZE];
        Strlcpy(buffer, path, EXECUTOR_BUFFER_SIZE);
        Strlcat(buffer, "/", EXECUTOR_BUFFER_SIZE);
        Strlcat(buffer, name, EXECUTOR_BUFFER_SIZE);

        // Determine file type (skip soft links and directories).

        struct stat st;

        if (lstat(buffer, &st) == -1)
            return -1;

        // If it's a directory, save the name:

        if (S_ISDIR(st.st_mode))
        {
            ChangeDirOwnerRecursive(buffer, uid, gid);
            continue;
        }

        // Skip soft links:

        if (S_ISLNK(st.st_mode))
            continue;

        // Process the current file.

        /* Flawfinder: ignore */
        if (chown(buffer, uid, gid) != 0)
            return -1;
    }

    // Close this directory:

    closedir(dir);

    return 0;
}

//==============================================================================
//
// AccessDir()
//
//     Returns 0 if able to stat given path and it is a directory.
//
//==============================================================================

int AccessDir(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
        return -1;

    return 0;
}
