//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CIMProperty.h,v $
// Revision 1.4  2001/03/04 21:57:34  bob
// Changed print methods to take a stream instead of hardcoded cout
//
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.3  2001/01/24 17:17:05  karl
// add Doc++ comments
//
// Revision 1.2  2001/01/15 04:31:44  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:53:04  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_Property_h
#define Pegasus_Property_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMPropertyRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMProperty
//
////////////////////////////////////////////////////////////////////////////////

class ConstCIMProperty;
class CIMInstanceRep;
/** CIMProperty Class - ATTN:
*/
class PEGASUS_COMMON_LINKAGE CIMProperty
{
public:

    /// CIMMethod CIMProperty
    CIMProperty() : _rep(0)
    {

    }

    /// CIMMethod CIMProperty
    CIMProperty(const CIMProperty& x)
    {
	Inc(_rep = x._rep);
    }

    /** CIMMethod CIMProperty

    @return
    // Throws IllegalName if name argument not legal CIM identifier.
    */
    CIMProperty(
	const String& name,
	const CIMValue& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMPropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~CIMProperty()
    {
	Dec(_rep);
    }

    /// CIMMethod
    CIMProperty& operator=(const CIMProperty& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    /// CIMMethod
    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    /** CIMMethod setName - Set the property name. Throws IllegalName if name 
        argument not legal CIM identifier.
    */
    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    /// CIMMethod getValue - ATTN:
    const CIMValue& getValue() const
    {
	_checkRep();
	return _rep->getValue();
    }

    /// CIMMethod setValue - ATTN
    void setValue(const CIMValue& value)
    {
	_checkRep();
	_rep->setValue(value);
    }

    /// CIMMethod getArraySize - ATTN:
    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    /// CIMMethod getReferenceClassName - ATTN:
    const String& getReferenceClassName() const
    {
	_checkRep();
	return _rep->getReferenceClassName();
    }

    /// CIMMethod getClassOrigin - ATTN
    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    /// CIMMethod setClassOrigin
    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }

    /// CIMMethod getPropagated - ATTN
    Boolean getPropagated() const
    {
	_checkRep();
	return _rep->getPropagated();
    }

    /// CIMMethod setProgagated - ATTN
    void setPropagated(Boolean propagated)
    {
	_checkRep();
	_rep->setPropagated(propagated);
    }

    /** CIMMethod addQualifier - ATTN
	Throws AlreadyExists.
    */
    CIMProperty& addQualifier(const CIMQualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    /// CIMMethod findQualifier - ATTN
    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    /// CIMMethod findQualifier - ATTN
    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    /// CIMMethod getQualifier - ATTN
    CIMQualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /// CIMMethod getQualifier - ATTN
    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    /// CIMMethod getQualifier - ATTN
    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    /// CIMMethod resolve
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart,
	const ConstCIMProperty& property)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart, property);
    }

    /// CIMMethod resolve - ATTN
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Boolean isInstancePart)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart);
    }

    /// ATTN
    operator int() const { return _rep != 0; }

    /// mthod toXML
    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    /// mthod print -ATTN
    void print(std::ostream &o=std::cout) const
    {
	_checkRep();
	_rep->print(o);
    }

    /// CIMMethod identical - ATTN
    Boolean identical(const ConstCIMProperty& x) const;

    Boolean isKey() const
    {
	_checkRep();
	return _rep->isKey();
    }

    /// CIMMethod clone - ATTN
    CIMProperty clone() const
    {
	return CIMProperty(_rep->clone());
    }

private:

    CIMProperty(CIMPropertyRep* rep) : _rep(rep)
    {
    }

    // This constructor allows the CIMClassRep friend class to cast
    // away constness.

    explicit CIMProperty(const ConstCIMProperty& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMPropertyRep* _rep;
    friend class ConstCIMProperty;
    friend class CIMClassRep;
    friend class CIMInstanceRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// ConstCIMProperty
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE ConstCIMProperty
{
public:

    ConstCIMProperty() : _rep(0)
    {

    }

    ConstCIMProperty(const ConstCIMProperty& x)
    {
	Inc(_rep = x._rep);
    }

    ConstCIMProperty(const CIMProperty& x)
    {
	Inc(_rep = x._rep);
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    ConstCIMProperty(
	const String& name,
	const CIMValue& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new CIMPropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~ConstCIMProperty()
    {
	Dec(_rep);
    }

    ConstCIMProperty& operator=(const ConstCIMProperty& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstCIMProperty& operator=(const CIMProperty& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    const String& getName() const
    {
	_checkRep();
	return _rep->getName();
    }

    const CIMValue& getValue() const
    {
	_checkRep();
	return _rep->getValue();
    }

    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    const String& getReferenceClassName() const
    {
	_checkRep();
	return _rep->getReferenceClassName();
    }

    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const
    {
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print(std::ostream &o=std::cout) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const ConstCIMProperty& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    Boolean isKey() const
    {
	_checkRep();
	return _rep->isKey();
    }

    CIMProperty clone() const
    {
	return CIMProperty(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    CIMPropertyRep* _rep;

    friend class CIMProperty;
    friend class CIMPropertyRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Property_h */
