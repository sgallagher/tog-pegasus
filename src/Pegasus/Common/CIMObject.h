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

#ifndef Pegasus_CIMObject_h
#define Pegasus_CIMObject_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

/** This class contains either a class or an instance (both CIM objects).
    Initializers are provided for both CIMClass and CIMInstance. The
    isClass() and isInstance() methods are provided for determining the
    type of contained object. Methods are also provided for getting
    the internal object (into a CIMClass or CIMInstance).
*/
class PEGASUS_COMMON_LINKAGE CIMObject
{
public:

    CIMObject() : _rep(0), _type(TYPE_NONE)
    {

    }

    CIMObject(const CIMObject& x) : _type(x._type)
    {
	Inc(_rep = x._rep);
    }

    CIMObject(const CIMClass& x) : _type(TYPE_CLASS)
    {
	Inc(_rep = x._rep);
    }

    CIMObject(const CIMInstance& x) : _type(TYPE_INSTANCE)
    {
	Inc(_rep = x._rep);
    }

    CIMObject& operator=(const CIMObject& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	    _type = x._type;
	}
	return *this;
    }

    CIMObject& operator=(const CIMClass& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	    _type = TYPE_CLASS;
	}
	return *this;
    }

    CIMObject& operator=(const CIMInstance& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	    _type = TYPE_INSTANCE;
	}
	return *this;
    }

    ~CIMObject()
    {
	Dec(_rep);
    }

    Boolean isClass() const
    {
	return _type == TYPE_CLASS;
    }

    Boolean isInstance() const
    {
	return _type == TYPE_INSTANCE;
    }

    /** Returns the class contained by this object (if an class).
	@return CIMClass
	@exception throws TypeMismatch if object does not contain a CIMClass.
    */
    CIMClass getClass();

    /** Const version of getClass() */

    CIMConstClass getClass() const;

    /** Returns the instance contained by this object (if an instance).
	@return CIMInstance
	@exception throws TypeMismatch if object does not contain a CIMInstance.
    */
    CIMInstance getInstance();

    /** Const version of getInstance() */

    CIMConstInstance getInstance() const;

    operator int() const 
    { 
	return _rep != 0; 
    }

    void toXml(Array<Sint8>& out) const;

private:

    void _checkRep() const
    {
	if (!_rep)
	    ThrowUnitializedHandle();
    }

    // Point to either a CIMClass or CIMInstance:

    Sharable* _rep;

    enum Type { TYPE_CLASS, TYPE_INSTANCE, TYPE_NONE };
    Type _type;
};

/** The CIMObjectWithPath encapsulates a CIMReference and CIMObject.
    Accessors are provided for getting the two parts. Constructors are
    provided for initializing it from a CIMObject.
*/
class PEGASUS_COMMON_LINKAGE CIMObjectWithPath
{
public:

    /**	Constructor
    */
    CIMObjectWithPath();

    /** constructor
    */
    CIMObjectWithPath(const CIMReference& reference, const CIMObject& object);

    /** Constructor - Constructs a CIMObjectWithPath Object from
        another CimObjectWithPath
        @param - ATTN
    */
    CIMObjectWithPath(const CIMObjectWithPath& x);

    ~CIMObjectWithPath();

    CIMObjectWithPath& operator=(const CIMObjectWithPath& x);

    /** set - 
    */
    void set(const CIMReference& reference, const CIMObject& object);

    /**
    */
    const CIMReference& getReference() const { return _reference; }

    /**
    */
    const CIMObject& getObject() const { return _object; }

    /**
    */
    CIMReference& getReference() { return _reference; }

    /**
    */
    CIMObject& getObject() { return _object; }

    /**
    */
    void toXml(Array<Sint8>& out) const;

private:

    CIMReference _reference;
    CIMObject _object;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMObject_h */
