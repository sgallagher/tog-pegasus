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
// Revision 1.4  2001/01/28 10:48:22  karl
// Doc++ Documentation
//
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

/*
Reference.h defines the KeyBinding and Reference Classes
*/

#ifndef Pegasus_Reference_h
#define Pegasus_Reference_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/**
KeyBinding -  This class ATTN:
*/

class PEGASUS_COMMON_LINKAGE KeyBinding
{
public:

    enum Type { BOOLEAN, STRING, NUMERIC };
    // Method KeyBinding - TBD 1
    KeyBinding();
    // Method KeyBinding - TBD 2
    KeyBinding(const KeyBinding& x);
    /// Method  KeyBinding - TBD 3
    KeyBinding(const String& name, const String& value, Type type);
    /// Method Keybinding Destructor
    ~KeyBinding();
    /// KeyBinding -- TBD 4
    KeyBinding& operator=(const KeyBinding& x);
    /** Method getName - ATTN:
    @return ATTN:
    */
    const String& getName() const 
    { 
	return _name; 
    }
    /** method setName
    ATTN:
    */
    void setName(const String& name) 
    { 
	_name = name; 
    }
    /// method getValue - ATTN:
    const String& getValue() const 
    { 
	return _value; 
    }
    /// method setValue - ATTN:
    void setValue(const String& value) 
    { 
	_value = value; 
    }
    /// method getType - ATTN:
    Type getType() const 
    { 
	return _type; 
    }
    /// method getType - ATTN:
    void setType(Type type) 
    { 
	_type = type; 
    }
    /// Method typeToString - ATTN
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

/** Class Reference - ATTN
*/



class PEGASUS_COMMON_LINKAGE Reference 
{
public:
	/// Method  - ATTN
    Reference();
	/// Method	- ATTN
    Reference(const Reference& x);
	/// Method  -ATTN
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
	/// Method Clear - ATTN
    void clear();
	/// Method set - ATTN
    void set(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = _getDefaultArray());
	/// Method getHost - ATTN
    const String& getHost() const 
    {
	return _host; 
    }
	/// Method setHost - ATTN
    void setHost(const String& host)
    {
	_host = host;
    }
	/// Method getNameSpace - ATTN
    const String& getNameSpace() const 
    {
	return _nameSpace; 
    }
	/// Method setNameSpace - ATTN
    void setNameSpace(const String& nameSpace);
	/// Method getClassName - ATTN
    const String& getClassName() const 
    { 
	return _className; 
    }
	/// Method setClassName - ATTN
    void setClassName(const String& className);
	/// Method getKeyBindings -- ATTN
    const Array<KeyBinding>& getKeyBindings() const 
    { 
	return _keyBindings; 
    }
	/// Method setKeyBindings - ATTN
    void setKeyBindings(const Array<KeyBinding>& keyBindings);
	/// Method identical - ATTN
    Boolean identical(const Reference& x) const;
	/// Method toXML - ATTN
    void toXml(Array<Sint8>& out) const;
	/// Method Print -- ATTN
    void print() const;
	/// Method nameSpaceToXML - ATTN
    void nameSpaceToXml(Array<Sint8>& out) const;
	/// Method
    void localNameSpaceToXml(Array<Sint8>& out) const;
	/// Method
    void instanceNameToXml(Array<Sint8>& out) const;
	/// Method
    void classNameToXml(Array<Sint8>& out) const;

    /**  Method instanceNameToReference
	Converts an instance name of the form
	<PRE>
	    "ClassName.key1=value1,...keyN=valueN"
	</PRE>
	to a Reference.
    */
    static void instanceNameToReference(
	const String& str,
	Reference& reference);

    /**	 Method referenceToInstanceName
	Converts a reference to an instance name of the form:
	<PRE>
    "ClassName.key1=value1,...keyN=valueN"
	</PRE>
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
