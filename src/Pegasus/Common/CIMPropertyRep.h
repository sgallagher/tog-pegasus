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

#ifndef Pegasus_PropertyRep_h
#define Pegasus_PropertyRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

class CIMClassRep;
class CIMProperty;
class CIMConstProperty;
class DeclContext;

class PEGASUS_COMMON_LINKAGE CIMPropertyRep : public Sharable
{
public:

    CIMPropertyRep(
	const String& name,
	const CIMValue& value,
	Uint32 arraySize,
	const String& referenceClassName,
	const String& classOrigin,
	Boolean propagated);

    ~CIMPropertyRep();

    const String& getName() const
    {
	return _name;
    }

    void setName(const String& name);

    const CIMValue& getValue() const
    {
	return _value;
    }

    void setValue(const CIMValue& value);

    Uint32 getArraySize() const
    {
	return _arraySize;
    }

    const String& getReferenceClassName() const
    {
	return _referenceClassName;
    }

    const String& getClassOrigin() const
    {
	return _classOrigin;
    }

    void setClassOrigin(const String& classOrigin);

    Boolean getPropagated() const
    {
	return _propagated;
    }

    void setPropagated(Boolean propagated)
    {
	_propagated = propagated;
    }

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Boolean existsQualifier(const String& name) const
    {
	return (findQualifier(name) != PEG_NOT_FOUND) ? true : false;
    }

    CIMQualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    void removeQualifier(Uint32 pos)
    {
	_qualifiers.removeQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart,
	const CIMConstProperty& property);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart);

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMPropertyRep* x) const;

    Boolean isKey() const;

    CIMPropertyRep* clone() const
    {
	return new CIMPropertyRep(*this);
    }

private:

    CIMPropertyRep();

    // Cloning constructor:

    CIMPropertyRep(const CIMPropertyRep& x);

    CIMPropertyRep& operator=(const CIMPropertyRep& x);

    String _name;
    CIMValue _value;
    Uint32 _arraySize;
    String _referenceClassName;
    String _classOrigin;
    Boolean _propagated;
    CIMQualifierList _qualifiers;

    friend class CIMClassRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_PropertyRep_h */
