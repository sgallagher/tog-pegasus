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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_EmbeddedServer_h
#define Pegasus_EmbeddedServer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

/** This module defines a class EmbeddedServer that provides functions for
    defining the setup of embedded servers where the providers are statically
    defined and the pegasus memory-resident CIM repository is used.
    See PEP 305 and the embeddedsystembuild readmes for more detailed
    information.
*/
class EmbeddedServer
{
public:

    /** Constructor.
    */
    EmbeddedServer();

    /** Destructor.
    */
    virtual ~EmbeddedServer();

    /** Load the repository. The derived class may optionally override and
        implement this function; otherwise, this function takes no action.
        The #data# parameter must point to memory obtained with malloc()
        and will be passed to free() after it is consumed.
        @param data array of bytes representing the repository.
    */
    virtual void loadRepository(Array<Uint8>& data);

    /** Save the repository. The derived class may optionally override and
        implement this function; otherwise, this function takes no action.
        @param data array of bytes representing the repository.
    */
    virtual void saveRepository(const Array<Uint8>& data);

    /** Put a record into the log. The derived class may optionally override
        and implement this function; otherwise, this function takes no action.
        @param type log type (1=TRACE, 2=STANDARD, 3=AUDIT, 4=ERROR, 5=DEBUG).
        @param system name of system performing log.
        @param level log level (1=TRACE, 2=INFO, 3=WARNING, 4=SEVERE, 5=FATAL).
        @param message the localized log message.
    */
    virtual void putLog(
        int type,
        const char* system,
        int level,
        const char* message);

    /** Add a provider to the provider table.
        @param moduleName name of provider module.
        @param providerName name of the provider.
        @param providerName namespace provider responds on.
        @param className class provided by this provider.
        @param createProvider provider entry point function.
        @return true on success.
    */
    Boolean addProvider(
        const String& moduleName,
        const String& providerName,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        class CIMProvider* (*createProvider)(const String& providerName));

    /** Add a namespace to the meta-repository.
        @param nameSpace pointer to generated namespace to be added.
        @return true if successful.
    */
    Boolean addNameSpace(const struct MetaNameSpace* nameSpace);

    /** Run the cimserver. This function returns when the server is shut down.
        @param argc
        @param argv
        @return false if server generated an exception.
    */
    Boolean run(int argc, char** argv);

private:
    EmbeddedServer(const EmbeddedServer&);
    EmbeddedServer& operator=(const EmbeddedServer&);
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_EmbeddedServer_h
