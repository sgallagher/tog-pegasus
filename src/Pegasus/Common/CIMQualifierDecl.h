//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QualifierDecl_h
#define Pegasus_QualifierDecl_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifierDeclRep.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class CIMConstQualifierDecl;
class CIMClassRep;
/** Class CIMQualifierDecl
  NOTE: Clarify difference between qualifier and qualiferdeclaration
  ATTN: Important work required here.
*/
class PEGASUS_COMMON_LINKAGE CIMQualifierDecl
{
public:
    /// Constructor - ATTN:
    CIMQualifierDecl() : _rep(0)
    {

    }
    /// Constructor - ATTN:

    CIMQualifierDecl(const CIMQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }
    /** Constructor
    Throws IllegalName if name argument not legal CIM identifier.
    ATTN:
    */
    
    CIMQualifierDecl(
	const String& name, 
	const CIMValue& value, 
	Uint32 scope,
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
    }
    /// Destructor
    ~CIMQualifierDecl()
    {
	Dec(_rep);
    }
    /// Operator
    CIMQualifierDecl& operator=(const CIMQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }
    /** CIMMethod ATTN:

    
    */
    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.
    /** CIMMethod	ATTN:
    
    */
    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }
    /** CIMMethod ATTN:

    
    */
    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }
    /** CIMMethod  ATTN:

    
    */
    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }
    /** CIMMethod
    
    */
    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }
    /** CIMMethod
    
    */
    void setValue(const CIMValue& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }
    /** CIMMethod
    
    */
    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }
    /** CIMMethod

    */

    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }
    /** CIMMethod

    */

    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }
    /** CIMMethod
    
    */

    operator int() const { return _rep != 0; }
    /** CIMMethod
    
    */

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }
    /** CIMMethod
    
    */

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }
    /** CIMMethod
    
    */

    Boolean identical(const CIMConstQualifierDecl& x) const;
    /** CIMMethod
    
    */

    CIMQualifierDecl clone() const
    {
	return CIMQualifierDecl(_rep->clone());
    }

private:

    CIMQualifierDecl(CIMQualifierDeclRep* rep) : _rep(rep)
    {
    }

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierDeclRep* _rep;
    friend class CIMConstQualifierDecl;
    friend class CIMClassRep;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstQualifierDecl
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstQualifierDecl
{
public:

    CIMConstQualifierDecl() : _rep(0)
    {

    }

    CIMConstQualifierDecl(const CIMConstQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

    CIMConstQualifierDecl(const CIMQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

    // Throws IllegalName if name argument not legal CIM identifier.

    CIMConstQualifierDecl(
	const String& name, 
	const CIMValue& value, 
	Uint32 scope,
	Uint32 flavor = CIMFlavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new CIMQualifierDeclRep(name, value, scope, flavor, arraySize);
    }

    ~CIMConstQualifierDecl()
    {
	Dec(_rep);
    }

    CIMConstQualifierDecl& operator=(const CIMConstQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    CIMConstQualifierDecl& operator=(const CIMQualifierDecl& x)
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

    CIMType getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    Boolean isArray() const 
    {
	_checkRep();
	return _rep->isArray();
    }

    const CIMValue& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }

    const Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const
    {
	_checkRep();
	_rep->print(o);
    }

    Boolean identical(const CIMConstQualifierDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

    CIMQualifierDecl clone() const
    {
	return CIMQualifierDecl(_rep->clone());
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    CIMQualifierDeclRep* _rep;
    friend class CIMQualifierDecl;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierDecl_h */
