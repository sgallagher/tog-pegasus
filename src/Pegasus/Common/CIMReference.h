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
// $Log: CIMReference.h,v $
// Revision 1.3  2001/02/26 04:33:28  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.2  2001/02/20 14:05:24  karl
// Comments for Document
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 02:49:00  mike
// Removed ugly workarounds for MSVC++ 5.0 (using SP3 now)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.9  2001/02/11 05:42:33  mike
// new
//
// Revision 1.8  2001/01/30 08:00:43  karl
// DOC++ Documentation update for header files
//
// Revision 1.7  2001/01/29 02:23:44  mike
// Added support for GetInstance operation
//
// Revision 1.6  2001/01/28 19:07:07  karl
// add DOC++ comments
//
// Revision 1.5  2001/01/28 17:44:55  karl
// Doc++ Comments
//
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
CIMReference.h defines the KeyBinding and CIMReference Classes
*/

#ifndef Pegasus_Reference_h
#define Pegasus_Reference_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
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

    enum CIMType { BOOLEAN, STRING, NUMERIC };

    /// KeyBinding - ATTN:
    KeyBinding();
    /// KeyBinding - ATTN:
    KeyBinding(const KeyBinding& x);
    ///  KeyBinding - TBD 3
    KeyBinding(const String& name, const String& value, CIMType type);
    /// Keybinding Destructor
    ~KeyBinding();
    /// KeyBinding -- ATTN:
    KeyBinding& operator=(const KeyBinding& x);

    /** getName - ATTN:
	@return String with the name ???ATTN:
    */
    const String& getName() const 
    { 
	return _name; 
    }

    /** setName
	ATTN:
    */  
    void setName(const String& name) 
    { 
	_name = name; 
    }

    ///  getValue - ATTN:
    const String& getValue() const 
    { 
	return _value; 
    }
    /// setValue - ATTN:
    void setValue(const String& value) 
    { 
	_value = value; 
    }
    /// getType - ATTN:
    CIMType getType() const 
    { 
	return _type; 
    }
    /// method getType - ATTN:
    void setType(CIMType type) 
    { 
	_type = type; 
    }
    /// typeToString - ATTN
    static const char* typeToString(CIMType type);

private:

    String _name;
    String _value;
    CIMType _type;

    friend Boolean operator==(const KeyBinding& x, const KeyBinding& y);
};

inline Boolean operator==(const KeyBinding& x, const KeyBinding& y)
{
    return 
	CIMName::equal(x._name, y._name) && 
	String::equal(x._value, y._value) && 
	x._type == y._type;
}

typedef Array<KeyBinding> KeyBindingArray;

/** Class CIMReference Path to the specified CIM class or CIM instance or CIM 
    qualifier.  The CIM object path is a reference to CIM elements.  It is only 
    valid in context of an active connection to a CIM Object Manager on a host.  
    In order to uniquely identify a given object on a host, it includes the 
    namespace, object name, and keys (if the object is an instance).  The 
    namespace is taken to be relative to the namespace that the CIMClient is 
    currently connected to.  A key is a property or set of properties used to 
    uniquely identify an instance of a class.  Key properties are marked with the 
    KEY qualifier.
      
    For example, the object path
    
    <TT>\\Server\Root\cimv2\CIM_ComputerSystem.CIMName=mycomputer: 
    CreationClassName=CIM_ComputerSystem</TT>
    
    has two parts:
     
    <TT>\\server\Root\cimv2</TT> - The default CIM namespace on host Server  
    <TT>CIM_ComputerSystem.CIMName=mycomputer</TT>, 
    
    <TT>CreationClassName=Solaris_ComputerSystem</TT> - A specific Solaris 
    Computer System object in the default namespace on host myserver.  This 
    computer system is uniquely identified by two key property values in 
    the format (key property = value): CIMName=mycomputer 
    CreationClassName=CIM_ComputerSystem 
*/ 

class PEGASUS_COMMON_LINKAGE CIMReference 
{
public:

    static Array<KeyBinding> _ArrayKeyBinding()
    {
	return Array<KeyBinding>();
    }

    /** Constructs a default CIM Object Path with empty namespace, 
	objectName and keys
	@return The new CIM Object Path constructed 
    */
    CIMReference();
    /** Constructs a CIM Object Path from the CIMReference parameter
	@param ATTN:
	@return The new CIM Object Path constructed
    */
    CIMReference(const CIMReference& x);
    /** Constructs a CIM Object Path from the individual components input.
	@param String representing the host
	@param String representing the Namespace
	@param String representing the Classname
	@param Array of Keybindings for the key properties
	@Return The new CIM Object
	@exception ATTN:
    */
    CIMReference(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = _ArrayKeyBinding());

    virtual ~CIMReference();
    /// Operator Overload =
    CIMReference& operator=(const CIMReference& x);

    /// CIMMethod Clear - ATTN
    void clear();
    /// CIMMethod set - ATTN
    void set(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = KeyBindingArray());

    /** CIMMethod getHost - Gets the host for this CIMObjectPath.
	@return -  String& with host name
    */
    const String& getHost() const 
    {
	return _host; 
    }
    /** setHost - Sets the host for this CIMObjectPath object.
    */
    void setHost(const String& host)
    {
	_host = host;
    }

    /// getNameSpace - ATTN
    const String& getNameSpace() const 
    {
	return _nameSpace; 
    }

    /** setNameSpace - ATTN:
	@param String representing the Namespace
	@return None
	@exception Throws IllegalName if Namespace not legal form
    */
    void setNameSpace(const String& nameSpace);
    
    /// CIMMethod getClassName - ATTN
    const String& getClassName() const 
    { 
	return _className; 
    }

    /** setClassName - ATTN:
	@param String representation of the Classname.
	@return None
	@exception Throws IllegalName if not legal
    */
    void setClassName(const String& className);

    /// getKeyBindings -- ATTN
    const Array<KeyBinding>& getKeyBindings() const 
    { 
	return _keyBindings; 
    }
	/// CIMMethod setKeyBindings - ATTN
    void setKeyBindings(const Array<KeyBinding>& keyBindings);
	/// CIMMethod identical - ATTN
    Boolean identical(const CIMReference& x) const;
    /** toXML - Returns an XML representation of this CIM object 
	path. 
	@param CIMReference to the CIM Object Path
	@return XML representation of the CIM Object path
    */
    void toXml(Array<Sint8>& out) const;

    /** print() Creates and prints to stdout the XML representation for
	the CIM object path
    */
    void print() const;
    ///  nameSpaceToXML - ATTN:
    void nameSpaceToXml(Array<Sint8>& out) const;
	/// localNameSpaceToXml - ATTN:
    void localNameSpaceToXml(Array<Sint8>& out) const;
	/// instanceeNameToXml - ATTN:
    void instanceNameToXml(Array<Sint8>& out) const;
	/// classNameToXml - ATTN:
    void classNameToXml(Array<Sint8>& out) const;

    /**  CIMMethod instanceNameToReference
	Converts an instance name of the form
	<PRE>
	    "ClassName.key1=value1,...keyN=valueN"
	</PRE>
	to a CIMReference.
    */
    static void instanceNameToReference(
	const String& str,
	CIMReference& reference);

    /**	 CIMMethod referenceToInstanceName
	Converts a reference to an instance name of the form:
	<PRE>
	    "ClassName.key1=value1,...keyN=valueN"
	</PRE>
    */
    static void referenceToInstanceName(
	const CIMReference& reference,
	String& instanceName);

private:

    String _host;
    String _nameSpace;
    String _className;
    Array<KeyBinding> _keyBindings;
};

inline Boolean operator==(const CIMReference& x, const CIMReference& y)
{
    return x.identical(y);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Reference_h */
