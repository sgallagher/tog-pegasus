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

#ifndef _Pegasus_Repository_MRR_h
#define _Pegasus_Repository_MRR_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Buffer.h>
#include "MRRTypes.h"
#include "Linkage.h"

PEGASUS_NAMESPACE_BEGIN

// Installs the initialize callback that is called when the repository is 
// initially created (from the MemoryResidentRepository constructor).
PEGASUS_REPOSITORY_LINKAGE void MRRInstallInitializeCallback(
    void (*callback)(class CIMRepository* repository, void * data),
    void *data);

// Installs the global save callback that is called when the memory-resident
// instance repository is modified. The buffer argument is a serialized
// copy of the memory-resident instance repository. The callback can do
// things such as save the buffer on disk for later use.
PEGASUS_REPOSITORY_LINKAGE void MRRInstallSaveCallback(
    void (*callback)(const Buffer& buffer, void* data),
    void* data);

// Installs the global load callback that is called when an instance of
// MemoryResidentRepository is created in order to load the initial set
// of instances (if any).
PEGASUS_REPOSITORY_LINKAGE void MRRInstallLoadCallback(
    void (*callback)(Buffer& buffer, void* data),
    void* data);

// Add the given namespace of qualifier declarations and classes.
PEGASUS_REPOSITORY_LINKAGE Boolean MRRAddNameSpace(
    const MRRNameSpace* nameSpace);

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Repository_MRR_h */
