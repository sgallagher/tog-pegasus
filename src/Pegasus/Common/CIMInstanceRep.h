//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InstanceDeclRep_h
#define Pegasus_InstanceDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectRep.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMPropertyList.h>
PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMInstance;
class CIMConstInstance;
class CIMClass;
class CIMConstClass;

class PEGASUS_COMMON_LINKAGE CIMInstanceRep : public CIMObjectRep
{
public:

    CIMInstanceRep(
	const CIMObjectPath& reference);

    virtual ~CIMInstanceRep();

    void resolve(
	DeclContext* declContext,
	const CIMNamespaceName& nameSpace,
	CIMConstClass& cimClassOut,
	Boolean propagateQualifiers);

    virtual void toXml(Array<char>& out) const;

    virtual void toMof(Array<char>& out) const;

    virtual CIMObjectRep* clone() const
    {
	return new CIMInstanceRep(*this);
    }

    CIMObjectPath buildPath(const CIMConstClass& cimClass) const;

    void filter(Boolean includeQualifiers,
            Boolean includeClassOrigin,
            const CIMPropertyList & propertyList);
private:

    CIMInstanceRep();

    CIMInstanceRep(const CIMInstanceRep& x);

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMInstanceRep& operator=(const CIMInstanceRep& x)
    {
        //PEGASUS_ASSERT(0);
        return *this;
    }

    friend class CIMInstance;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDeclRep_h */
