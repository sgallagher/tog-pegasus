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

/* This module defines a class EmbeddedServer that provides functions for
   defining the setup of embedded servers where the providers are statically
   defined and the pegasus memory-resident CIM repository is used.
   See PEP 305 and the embeddedsystembuild readmes for more detailed
   information.
*/

#ifndef Pegasus_EmbeddedServer_h
#define Pegasus_EmbeddedServer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/MetaRepository.h>
#include <Pegasus/Repository/MemoryResidentRepository.h>
#include "Linkage.h"
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE EmbeddedServer
{
public:

    /**
     *  Install the provider table defined by the parameter.
     *  This function passes the pointer to the table of providers
     *  that will be installed as part of the server startup.
     *  @param ProviderTableEntry pointer defining a table of
     *  provider definitions that represents the providers to be
     *  registered for this cimserver.
     *  See the file Pegasus/Server/ProviderTable.h for more
     *  information on the definition of this table
     */
    static void installProviderTable(ProviderTableEntry* providerTable);

    /**
     * Install the memory-resident class repository defined by list 
     * of namespaces defined by the input parameter. 
     *  
     * @param namespaces pointer to MetaNameSpace objects that 
     * define the namespaces/classes/qualifiers that define the 
     * class repository to be installed.  This is a pointer to an 
     * array of MetaNameSpace entities each of which represents one 
     * namespace.  The last entry in this array MUST BE a zero entry 
     * to terminate the array. 
     */

    static void installNameSpaces(const MetaNameSpace* const* nameSpaces);

    /**
     * define the callback for a particular implementation of the
     * function to save the instance repository.
     * @param callback defines the function that will be called.
     * @param data void* pointer to data which will be passed to the
     * function defined by callback on each call.
     */
    static void installSaveRepositoryCallback(
        void (*callback)(const Buffer& buffer, void* data),
        void* data);

    /** Define the callback function for a particular implementation
     * of the function to load the instance repsository from
     * persistent storage and provide that set of instances to
     * Repository implementation.
     * @param callback defines the function that will be called
     * @param data void* pointer to data which will be passed to the
     * function defined by callback on each call.
     * Example: 
     *  
     *   static void _loadCallback(Buffer& buffer, void* data)
     *   { // Function that implements the instance repository load
     *   }
     *   ...
     *   EmbeddedServer::installLoadRepositoryCallback(_loadCallback,
     *   0); 
     */
    static void installLoadRepositoryCallback(
        void (*callback)(Buffer& buffer, void* data),
        void* data);

private:

    EmbeddedServer();
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_EmbeddedServer_h
