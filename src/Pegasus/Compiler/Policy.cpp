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
#include <fstream>
#include <cstdarg>
#include "Policy.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

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

    // Write the policy file (adding or replacing new rule).

    ofstream os(path.getCString());

    for (Uint32 i = 0; i < lines.size(); i++)
    {
        if (lines[i].type == PolicyFileLine::COMMENT)
            os << lines[i].comment;
        else if (lines[i].moduleName != moduleName)
            os << lines[i].moduleName << ":" << lines[i].providerUser << endl;
    }

    os << moduleName << ":" << providerUser << endl;
    os.close();

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
