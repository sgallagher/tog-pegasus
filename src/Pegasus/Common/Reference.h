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
// $Log: Reference.h,v $
// Revision 1.3  2001/01/28 07:05:18  mike
// added instance name/reference converters
//
// Revision 1.2  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.1.1.1  2001/01/14 19:53:12  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_Reference_h
#define Pegasus_Reference_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// KeyBinding
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE KeyBinding
{
public:

    enum Type { BOOLEAN, STRING, NUMERIC };

    KeyBinding();

    KeyBinding(const KeyBinding& x);

    KeyBinding(const String& name, const String& value, Type type);

    ~KeyBinding();

    KeyBinding& operator=(const KeyBinding& x);

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name) 
    { 
	_name = name; 
    }

    const String& getValue() const 
    { 
	return _value; 
    }

    void setValue(const String& value) 
    { 
	_value = value; 
    }

    Type getType() const 
    { 
	return _type; 
    }

    void setType(Type type) 
    { 
	_type = type; 
    }

    static const char* typeToString(Type type);

private:

    String _name;
    String _value;
    Type _type;

    friend Boolean operator==(const KeyBinding& x, const KeyBinding& y);
};

inline Boolean operator==(const KeyBinding& x, const KeyBinding& y)
{
    return x._name == y._name && x._value == y._value && x._type == y._type;
}

////////////////////////////////////////////////////////////////////////////////
//
// Reference
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE Reference 
{
public:

    Reference();

    Reference(const Reference& x);

    Reference(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = Array<KeyBinding>());

    virtual ~Reference();

    Reference& operator=(const Reference& x);

    // ATTN: figure out why this trick is necessary using MSVC 5.0.

    static Array<KeyBinding> _getDefaultArray()
    {
	return Array<KeyBinding>();
    }

    void clear();

    void set(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = _getDefaultArray());

    const String& getHost() const 
    {
	return _host; 
    }

    void setHost(const String& host)
    {
	_host = host;
    }

    const String& getNameSpace() const 
    {
	return _nameSpace; 
    }

    void setNameSpace(const String& nameSpace);

    const String& getClassName() const 
    { 
	return _className; 
    }

    void setClassName(const String& className);

    const Array<KeyBinding>& getKeyBindings() const 
    { 
	return _keyBindings; 
    }

    void setKeyBindings(const Array<KeyBinding>& keyBindings);

    Boolean identical(const Reference& x) const;

    void toXml(Array<Sint8>& out) const;

    void print() const;

    void nameSpaceToXml(Array<Sint8>& out) const;

    void localNameSpaceToXml(Array<Sint8>& out) const;

    void instanceNameToXml(Array<Sint8>& out) const;

    void classNameToXml(Array<Sint8>& out) const;

    /** 
	Converts an instance name of the form

	    "ClassName.key1=value1,...keyN=valueN"

	to a Reference.
    */
    static void instanceNameToReference(
	const String& str,
	Reference& reference);

    /**
	Converts a reference to an instance name of the form:

	    "ClassName.key1=value1,...keyN=valueN"
    */
    static void referenceToInstanceName(
	const Reference& reference,
	String& instanceName);

private:

    String _host;
    String _nameSpace;
    String _className;
    Array<KeyBinding> _keyBindings;
};

inline Boolean operator==(const Reference& x, const Reference& y)
{
    return x.identical(y);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Reference_h */
