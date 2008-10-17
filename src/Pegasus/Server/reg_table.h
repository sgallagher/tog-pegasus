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

//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_reg_table_h
#define Pegasus_reg_table_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInter.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class RegTableRecord
{
public:
    RegTableRecord(
        const CIMName& className,
        const CIMNamespaceName& namespaceName,
        const String& providerName,
        Uint32 serviceId);
    ~RegTableRecord();

    CIMName className;
    CIMNamespaceName namespaceName;
    String providerName;
    Uint32 serviceId;
private:
    RegTableRecord(const RegTableRecord&);
    RegTableRecord& operator=(const RegTableRecord&);
};

class PEGASUS_SERVER_LINKAGE DynamicRoutingTable
{
public:
    ~DynamicRoutingTable();

    static DynamicRoutingTable* getRoutingTable();

    // get a single service that can route this spec.
    Boolean getRouting(
        const CIMName& className,
        const CIMNamespaceName& namespaceName,
        String& provider,
        Uint32 &serviceId) const;

    void insertRecord(
        const CIMName& className,
        const CIMNamespaceName& namespaceName,
        const String& provider,
        Uint32 serviceId);
#ifdef PEGASUS_DEBUG
    void dumpRegTable();
#endif
private:
    DynamicRoutingTable();
    DynamicRoutingTable(const DynamicRoutingTable& table);
    DynamicRoutingTable& operator=(const DynamicRoutingTable& table);

    String DynamicRoutingTable::_getRoutingKey(
        const CIMName& className,
        const CIMNamespaceName& namespaceName) const;

    String DynamicRoutingTable::_getWildRoutingKey(
        const CIMName& className) const;

    typedef HashTable<String, RegTableRecord*, 
        EqualNoCaseFunc, HashFunc<String> > RoutingTable;
    RoutingTable _routingTable;

    static AutoPtr<DynamicRoutingTable> _this;
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_reg_table_h
