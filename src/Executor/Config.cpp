#include <string.h>
#include <stdio.h>
#include "Config.h"
#include "Strlcpy.h"
#include "Strlcat.h"
#include "Path.h"
#include "Defines.h"

//==============================================================================
//
// GetConfigParamFromCommandLine()
//
//     Attempt to find a command line configuratin parameter of the form 
//     name=value. For example: 
//         cimservermain repositoryDir=/opt/pegasus/repository. 
//     Return 0 if found.
//
//==============================================================================

int GetConfigParamFromCommandLine(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    size_t n = strlen(name);

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], name, n) == 0 && argv[i][n] == '=')
        {
            const char* p = argv[i] + n + 1;
            Strlcpy(value, argv[i] + n + 1, EXECUTOR_BUFFER_SIZE);
            return 0;
        }
    }

    return -1;
}

//==============================================================================
//
// GetConfigParamFromFile()
//
//     Attempt to find the named option in the configuration file. If found,
//     set value and return 0.
//
//==============================================================================

int GetConfigParamFromFile(
    const char* path,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    FILE* is = fopen(path, "r");

    if (!is)
        return -1;

    char buffer[EXECUTOR_BUFFER_SIZE];
    const size_t n = strlen(name);

    while (fgets(buffer, sizeof(buffer), is) != 0)
    {
        // Skip comments.

        if (buffer[0] == '#')
            continue;

        // Remove trailing whitespace.

        size_t r = strlen(buffer);

        while (r--)
        {
            if (isspace(buffer[r]))
                buffer[r] = '\0';
        }

        // Skip blank lines.

        if (buffer[0] == '\0')
            continue;

        // Check option.

        if (strncmp(buffer, name, n) == 0 &&  buffer[n] == '=')
        {
            Strlcpy(value, buffer + n + 1, EXECUTOR_BUFFER_SIZE);
            fclose(is);
            return 0;
        }
    }

    // Not found!
    fclose(is);
    return -1;
}

//==============================================================================
//
// GetConfigParam()
//
//     Attempt to find a configuration setting for the given name. First,
//     search the command line and then the config file.
//
//==============================================================================

int GetConfigParam(
    int argc,
    char** argv,
    const char* name,
    char value[EXECUTOR_BUFFER_SIZE])
{
    // (1) First check command line.

    if (GetConfigParamFromCommandLine(argc, argv, name, value) == 0)
        return 0;

    // (2) Next check config file.

    char path[EXECUTOR_BUFFER_SIZE];

    if (GetHomedPath(PEGASUS_PLANNED_CONFIG_FILE_PATH, path) == 0 &&
        GetConfigParamFromFile(path, name, value) == 0)
        return 0;

    // Not found!
    return -1;
}

