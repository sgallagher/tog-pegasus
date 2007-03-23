#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Defines.h"
#include "Strlcpy.h"
#include "PasswordFile.h"

/*
**==============================================================================
**
** CheckPasswordFile()
**
**     Checks whether the *password* is correct for the given *username*, 
**     according to the password file referred to by *path*. The file has
**     the following format.
**
**         <usrname>:<encrypted-password>
**
**     For example (smith's password is "changeme"):
**     
**         smith:AB5bZ.JX9fQzA
**         jones:XMllrzJ80fd.A
**         williams:lM80ffj.jiOiO
**
**     Returns zero if the password matches or if password is null and the
**     user exists.
**
**==============================================================================
*/

int CheckPasswordFile(
    const char* path,
    const char* username,
    const char* password)
{
    FILE* is;
    char line[EXECUTOR_BUFFER_SIZE];

    /* Open file. */

    if ((is = fopen(path, "r")) == NULL)
        return -1;

    /* Lookup encrypted password for this user. */

    while (fgets(line, sizeof(line), is) != NULL)
    {
        char* p;
        char encryptedPassword[14];
        char buffer[EXECUTOR_BUFFER_SIZE];
        char salt[3];

        /* Skip lines starting with '#'. */

        if (line[0] == '#')
            continue;

        /* Replace colon with null-terminator. */

        if ((p = strchr(line, ':')) == NULL)
            continue;

        *p++ = '\0';

        /* Skip this line, if username does not match. */

        if (strcmp(line, username) != 0)
            continue;

        /* If password is null, we are done. */

        if (password == NULL)
            return 0;

        /* Get encrypted password. */

        Strlcpy(encryptedPassword, p, sizeof(encryptedPassword));

        /* Get salt from encrypted password. */

        salt[0] = encryptedPassword[0];
        salt[1] = encryptedPassword[1];
        salt[2] = '\0';

        /* Check password. */

        /* Flawfinder: ignore */
        Strlcpy(buffer, crypt(password, salt), sizeof(buffer));

        if (strcmp(buffer, encryptedPassword) == 0)
        {
            fclose(is);
            return 0;
        }
        else
        {
            fclose(is);
            return -1;
        }
    }

    /* User entry not found. */

    fclose(is);
    return -1;
}
