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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ParameterRep_h
#define Pegasus_ParameterRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMConstParameter;
class CIMParameter;

class PEGASUS_COMMON_LINKAGE CIMParameterRep : public Sharable
{
public:

    CIMParameterRep(
	const CIMName& name, 
	CIMType type,
	Boolean isArray,
	Uint32 arraySize,
	const CIMName& referenceClassName);

    ~CIMParameterRep();

    const CIMName& getName() const 
    { 
	return _name; 
    }

    void setName(const CIMName& name);

    Boolean isArray() const
    {
	return _isArray;
    }

    Uint32 getArraySize() const
    {
	return _arraySize;
    }

    const CIMName& getReferenceClassName() const 
    {
	return _referenceClassName; 
    }

    const CIMType getType() const 
    { 
	return _type; 
    }

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const CIMName& name) const
    {
	return _qualifiers.find(name);
    }

    CIMQualifier getQualifier(Uint32 index)
    {
	return _qualifiers.getQualifier(index);
    }

    CIMConstQualifier getQualifier(Uint32 index) const
    {
	return _qualifiers.getQualifier(index);
    }

    void removeQualifier (Uint32 index);

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(DeclContext* declContext, const CIMNamespaceName& nameSpace);

    void toXml(Array<char>& out) const;

    void toMof(Array<char>& out) const;

    Boolean identical(const CIMParameterRep* x) const;

    CIMParameterRep* clone() const
    {
	return new CIMParameterRep(*this);
    }

private:

    CIMParameterRep();

    CIMParameterRep(const CIMParameterRep& x);

    // This method is declared and made private so that the compiler does
    // not implicitly define a default copy constructor.
    CIMParameterRep& operator=(const CIMParameterRep& x)
    {
        //PEGASUS_ASSERT(0);
        return *this;
    }

    CIMName _name;
    CIMType _type;
    Boolean _isArray;
    Uint32 _arraySize;
    CIMName _referenceClassName;
    CIMQualifierList _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParameterRep_h */
