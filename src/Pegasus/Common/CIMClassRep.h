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
//              Adriann Schuur (schuur@de.ibm.com) PEP 164
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMClassRep_h
#define Pegasus_CIMClassRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectRep.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMClass;
class CIMConstClass;
class CIMInstanceRep;

// ATTN: KS P3 -document the CIMClass and CIMObjectRep  classes.

class PEGASUS_COMMON_LINKAGE CIMClassRep : public CIMObjectRep
{
public:

    CIMClassRep(
	const CIMName& className,
	const CIMName& superClassName);

    virtual ~CIMClassRep();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const CIMName& getSuperClassName() const { return _superClassName; }

    void setSuperClassName(const CIMName& superClassName);

    virtual void addProperty(const CIMProperty& x);

    void addMethod(const CIMMethod& x);

    Uint32 findMethod(const CIMName& name) const;

    CIMMethod getMethod(Uint32 index);

    CIMConstMethod getMethod(Uint32 index) const
    {
	return ((CIMClassRep*)this)->getMethod(index);
    }

    void removeMethod(Uint32 index);

    Uint32 getMethodCount() const;

    void resolve(
	DeclContext* context,
	const CIMNamespaceName& nameSpace);

    virtual Boolean identical(const CIMObjectRep* x) const;

    void toXml(Array<char>& out) const;

    void toMof(Array<char>& out) const;

    virtual CIMObjectRep* clone() const
    {
	return new CIMClassRep(*this);
    }

    void getKeyNames(Array<CIMName>& keyNames) const;

    Boolean hasKeys() const;

    CIMInstance buildInstance(Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const CIMPropertyList & propertyList) const;

private:

    CIMClassRep();

    CIMClassRep(const CIMClassRep& x);

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMClassRep& operator=(const CIMClassRep& x)
    {
        //PEGASUS_ASSERT(0);
        return *this;
    }

    CIMName _superClassName;
    Array<CIMMethod> _methods;

    friend class CIMClass;
    friend class CIMInstanceRep;
    friend class BinaryStreamer;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMClassRep_h */
