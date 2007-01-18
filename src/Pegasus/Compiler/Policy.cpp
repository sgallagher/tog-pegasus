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

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/FileSystem.h>
#include <cstdio>
#include <cstdarg>
#include "Policy.h"
#include <fcntl.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// trustedDirs[]
//
//     When providers are registered, the corresponding MOF file must reside 
//     in an "trusted directory" to ensure the MOF could not have been 
//     created by a hacker without write access to that directory. If this
//     array is empty, then no restriction applies.
//
//==============================================================================

static const char* _trustedDirs[] =
{
    NULL,
};

//==============================================================================
//
// _throwEx()
//
//==============================================================================

static void _throwEx(const char* format, ...)
{
    char buffer[1024];

    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);

    throw CIMException(CIM_ERR_FAILED,
        String("failed to update policy file: ") + String(buffer));
}

//==============================================================================
//
// CheckTrustedDirs()
//     
//==============================================================================

static Array<String> _dirnames;
static Array<String> _basenames;

void CheckTrustedDirs()
{
    if (_trustedDirs[0] == NULL)
        return;

    for (size_t i = 0; i < _dirnames.size(); i++)
    {
        bool found = false;

        for (size_t j = 0; _trustedDirs[j]; j++)
        {
            if (_dirnames[i] == String(_trustedDirs[j]))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            String path = _dirnames[i] + String("/") + _basenames[i];

            _throwEx(
                "Provider module registration error. File does not reside in a "
                "trusted directory: %s", (const char*)path.getCString());
        }
    }
}

//==============================================================================
//
// ReverseFind()
//     
//     Find the given charcter in the string starting from the rear of the
//     string. This function is like strrchr().
//
//==============================================================================

Uint32 ReverseFind(const String& s, char c)
{
    for (Uint32 i = s.size(); i--; )
    {
        if (s[i] == c)
            return i;
    }

    return Uint32(-1);
}

//==============================================================================
//
// _LockFile()
//
//     Obtain an exclusive lock on the given file.
//
//==============================================================================

static int _LockFile(int fd)
{
    static struct flock lock;
    lock.l_type = F_WRLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0; 
    return fcntl(fd, F_SETLKW, &lock);
}

//==============================================================================
//
// _UnlockFile()
//
//     Release the lock on the given file.
//
//==============================================================================

static int _UnlockFile(int fd)
{
    static struct flock lock;
    lock.l_type = F_UNLCK; 
    lock.l_whence = SEEK_SET; 
    lock.l_start = 0; 
    lock.l_len = 0; 
    return fcntl(fd, F_SETLKW, &lock);
}

//==============================================================================
//
// ExpandPath()
//
//     Expand the given path into a dirname and basename (use getcwd() if 
//     necessary).
//
//==============================================================================

static void ExpandPath(const String& path, String& dirname, String& basename)
{
    String cwd;
    FileSystem::getCurrentDirectory(cwd);

    // Prepend current-working-directory if necessary.

    {
        String tmp;

        if (path[0] == '/')
            tmp = path;
        else
            tmp = cwd + String("/") + path;

        Uint32 slash = ReverseFind(tmp, '/');
        dirname = tmp.subString(0, slash);
        basename = tmp.subString(slash + 1);
    }

    // Normalize dirname:

    if (FileSystem::changeDirectory(dirname))
    {
        FileSystem::getCurrentDirectory(dirname);
        FileSystem::changeDirectory(cwd);
    }
}

//==============================================================================
//
// AddFile()
//
//==============================================================================

void AddPath(const String& path)
{
    String dirname;
    String basename;

    ExpandPath(path, dirname, basename);
    _dirnames.append(dirname);
    _basenames.append(basename);
}

//==============================================================================
//
// _getHomedPath()
//
//==============================================================================

static String _getHomedPath(const String& name)
{
    if (name[0] == '/')
        return name;

    const char* home = getenv("PEGASUS_HOME");

    if (home)
        return String(home) + String("/") + name;

    return name;
}

//==============================================================================
//
// struct PolicyFileLine
//
//     Represents a line in of the policy file (either a comment or rule).
//
//         # rule on next line (this is a comment).
//         MyModule:smith
//
//==============================================================================

struct PolicyFileLine
{
    enum Type { COMMENT, RULE };
    Type type;
    String comment;
    String moduleName;
    String providerUser;
};

//==============================================================================
//
// UpdatePolicyFile()
//
//==============================================================================

void UpdatePolicyFile(
    class CIMClient* client, 
    const CIMNamespaceName& nameSpace,
    const CIMInstance& inst)
{
    // Deduce new policy rule from instance.

    String moduleName;
    String providerUser;
    {
        enum UserContext
        {
            USER_CONTEXT_REQUESTOR = 2,
            USER_CONTEXT_DESIGNATED = 3,
            USER_CONTEXT_PRIVILEGED = 4,
            USER_CONTEXT_CIMSERVER = 5,
        };
        UserContext userContext;

        // PG_ProviderModule.Name:
        {
            Uint32 pos = inst.findProperty("Name");

            if (pos == Uint32(-1))
                _throwEx("PG_ProviderModule.Name property missing");

            CIMValue value = inst.getProperty(pos).getValue();

            if (value.getType() != CIMTYPE_STRING)
                _throwEx("PG_ProviderModule.Name property is not a string");

            value.get(moduleName);

            if (moduleName.size() == 0)
                _throwEx("PG_ProviderModule.Name is empty string");
        }

        // PG_ProviderModule.UserContext:
        {
            Uint32 pos = inst.findProperty("UserContext");

            if (pos == Uint32(-1))
            {
                // This is the default UserContext according to PEP-197.
                userContext = USER_CONTEXT_PRIVILEGED;
            }
            else
            {
                CIMValue value = inst.getProperty(pos).getValue();

                if (value.getType() != CIMTYPE_UINT16)
                    _throwEx("PG_ProviderModule.UserContext is wrong type");

                if (value.isNull())
                {
                    // This is the default UserContext according to PEP-197.
                    userContext = USER_CONTEXT_PRIVILEGED;
                }
                else
                {
                    Uint16 tmp;
                    value.get(tmp);

                    switch (tmp)
                    {
                        case 2:
                            userContext = USER_CONTEXT_REQUESTOR;
                            break;
                        case 3:
                            userContext = USER_CONTEXT_DESIGNATED;
                            break;
                        case 4:
                            userContext = USER_CONTEXT_PRIVILEGED;
                            break;
                        case 5:
                            userContext = USER_CONTEXT_CIMSERVER;
                            break;
                        default:
                            _throwEx(
                                "bad PG_ProviderModule.UserContext value: %u",
                                tmp);
                    }
                }
            }
        }

        // PG_ProviderModule.DesignatedUserContext:

        if (userContext == USER_CONTEXT_DESIGNATED)
        {
            Uint32 pos = inst.findProperty("DesignatedUserContext");

            if (pos == Uint32(-1))
            {
                _throwEx("missing "
                    "PG_ProviderModule.DesignatedUserContext property");
            }

            CIMValue value = inst.getProperty(pos).getValue();

            if (value.getType() != CIMTYPE_STRING)
            {
                _throwEx(
                    "PG_ProviderModule.DesignatedUserContext "
                    "must be a string");
            }

            value.get(providerUser);
        }
        else if (userContext == USER_CONTEXT_REQUESTOR)
            providerUser = "${requestorUser}";
        else if (userContext == USER_CONTEXT_PRIVILEGED)
            providerUser = "${privilegedUser}";
        else if (userContext == USER_CONTEXT_CIMSERVER)
            providerUser = "${cimserverUser}";
    }

    // Determine path to policy file.

    String path = _getHomedPath(PEGASUS_POLICY_CONFIG_FILE_PATH);

    // Open the file.
    
    ifstream is(path.getCString());

    if (!is)
        _throwEx("failed to open \"%s\"", (const char*)path.getCString());

    // Read policy file into memory.

    String buffer;

    Array<PolicyFileLine> lines;

    while (GetLine(is, buffer))
    {
        // Handle comments.

        if (buffer[0] == '#')
        {
            PolicyFileLine line;
            line.type = PolicyFileLine::COMMENT;
            line.comment = buffer;
            continue;
        }

        // Remove trailing spaces.

        while (buffer.size() && isspace(buffer[buffer.size()-1]))
            buffer.remove(buffer.size()-1);

        // Skip blank lines.

        if (buffer.size() == 0)
            continue;

        // Split buffer about ':' character.

        Uint32 pos = buffer.find(':');

        if (pos == Uint32(-1))
        {
            _throwEx("policy config file rule missing separator: %s",
                (const char*)path.getCString());
        }

        String first = buffer.subString(0, pos);
        String second = buffer.subString(pos + 1);

        // Save rule in memory.

        PolicyFileLine line;
        line.type = PolicyFileLine::RULE;
        line.moduleName = first;
        line.providerUser = second;

        lines.append(line);
    }

    is.close();

    // Open the policy file (adding or replacing new rule).

    int fd = open(path.getCString(), O_RDWR | O_CREAT, 0644);

    // Obtain a write lock

    if (_LockFile(fd) != 0)
    {
        _throwEx("failed to obtain write lock on policy configuration file: %s",
            (const char*)path.getCString());
    }

    // Truncate file now that we have a lock.

    ftruncate(fd, 0);

    // Write the file.

    FILE* os = fdopen(fd, "w+");

    if (!os)
    {
        _throwEx("failed to open policy configuration file: %s",
            (const char*)path.getCString());
    }

    for (Uint32 i = 0; i < lines.size(); i++)
    {
        if (lines[i].type == PolicyFileLine::COMMENT)
        {
            fprintf(os, "%s\n", (const char*)lines[i].comment.getCString());
        }
        else if (lines[i].moduleName != moduleName)
        {
            fprintf(os, "%s:%s\n", 
                (const char*)lines[i].moduleName.getCString(),
                (const char*)lines[i].providerUser.getCString());
        }
    }

    fprintf(os, "%s:%s\n", 
        (const char*)moduleName.getCString(),
        (const char*)providerUser.getCString());

    _UnlockFile(fd);
    fclose(os);

    // If no client, then return success.

    if (!client)
        return;

    // Send PG_ProviderModule.refreshPolicy() method to server, so that it will
    // re-read the policy file.

    try
    {
        CIMObjectPath cop("PG_ProviderModule");
        Array<CIMKeyBinding> kbs;
        kbs.append(CIMKeyBinding("Name", moduleName, CIMKeyBinding::STRING));
        cop.setKeyBindings(kbs);
        Array<CIMParamValue> in;
        Array<CIMParamValue> out;

        client->invokeMethod(nameSpace, cop, CIMName("refreshPolicy"), in, out);
    }
    catch (Exception&)
    {
        throw;
    }
}

PEGASUS_NAMESPACE_END
