//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InstanceDeclRep_h
#define Pegasus_InstanceDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectRep.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMInstance;
class CIMConstInstance;
class CIMClass;
class CIMConstClass;

class PEGASUS_COMMON_LINKAGE CIMInstanceRep : public CIMObjectRep
{
public:

    CIMInstanceRep(const CIMReference& reference);

    virtual ~CIMInstanceRep();

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	CIMConstClass& cimClassOut);

    virtual void toXml(Array<Sint8>& out) const;

    virtual void print(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const;

    virtual CIMObjectRep* clone() const
    {
	return new CIMInstanceRep(*this);
    }

    CIMReference getInstanceName(const CIMConstClass& cimClass) const;

    String toString() const;

private:

    CIMInstanceRep();

    CIMInstanceRep(const CIMInstanceRep& x);

    CIMInstanceRep& operator=(const CIMInstanceRep& x)
    {
	return *this;
    }

    friend class CIMInstance;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDeclRep_h */
