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
// $Log: Reference.cpp,v $
// Revision 1.1  2001/01/14 19:53:11  mike
// Initial revision
//
//
//END_HISTORY

#include <cctype>
#include "Reference.h"
#include "Indentor.h"
#include "Name.h"
#include "Destroyer.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// KeyBinding
//
////////////////////////////////////////////////////////////////////////////////

KeyBinding::KeyBinding() { }

KeyBinding::KeyBinding(const KeyBinding& x)
    : _name(x._name), _value(x._value), _type(x._type) 
{ 

}

KeyBinding::KeyBinding(const String& name, const String& value, Type type)
    : _name(name), _value(value), _type(type) 
{ 

}

KeyBinding::~KeyBinding() 
{ 

}

KeyBinding& KeyBinding::operator=(const KeyBinding& x)
{
    _name = x._name;
    _value = x._value;
    _type = x._type;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
//
// Reference
//
////////////////////////////////////////////////////////////////////////////////

Reference::Reference()
{

}

Reference::Reference(const Reference& x)
    : _host(x._host), _nameSpace(x._nameSpace), 
    _className(x._className), _keyBindings(x._keyBindings)
{

}

Reference::Reference(
    const String& host,
    const String& nameSpace,
    const String& className, 
    const Array<KeyBinding>& keyBindings)
    : 
    _host(host), 
    _nameSpace(nameSpace), 
    _className(className), 
    _keyBindings(keyBindings)
{

}

Reference::~Reference()
{

}

Reference& Reference::operator=(const Reference& x)
{
    if (&x != this)
    {
	_host = x._host;
	_nameSpace = x._nameSpace;
	_className = x._className;
	_keyBindings = x._keyBindings;
    }
    return *this;
}

void Reference::set(
    const String& host,
    const String& nameSpace,
    const String& className, 
    const Array<KeyBinding>& keyBindings)
{
    _host = host;
    _nameSpace = nameSpace;
    _className = className;
    _keyBindings = keyBindings;
}

void Reference::setNameSpace(const String& nameSpace)
{
    if (!Name::legal(nameSpace))
	throw IllegalName();
}

void Reference::setClassName(const String& className)
{
    if (!Name::legal(className))
	throw IllegalName();
}

void Reference::setKeyBindings(const Array<KeyBinding>& keyBindings) 
{
    _keyBindings = keyBindings; 
}

Boolean Reference::identical(const Reference& x) const
{
    return 
	_host == x._host && 
	_nameSpace == x._nameSpace && 
	_className == x._className && 
	_keyBindings == x._keyBindings;
}

void Reference::nameSpaceToXml(Array<Sint8>& out) const
{
    if (_host.getLength())
    {
	out << "<NAMESPACEPATH>\n";
	out << "<HOST>" << _host << "</HOST>\n";
    }
    
    XmlWriter::appendLocalNameSpaceElement(out, _nameSpace);

    if (_host.getLength())
	out << "</NAMESPACEPATH>\n";
}

void Reference::localNameSpaceToXml(Array<Sint8>& out) const
{
    out << "<LOCALNAMESPACEPATH>\n";

    char* tmp = _nameSpace.allocateCString();

    for (char* p = strtok(tmp, "/"); p; p = strtok(NULL, "/"))
    {
	out << "<NAMESPACE NAME=\"" << p << "\"/>\n";
    }

    out << "</LOCALNAMESPACEPATH>\n";
}

void Reference::classNameToXml(Array<Sint8>& out) const
{
    out << "<CLASSNAME NAME=\"" << _className << "\"/>\n";
}

void Reference::instanceNameToXml(Array<Sint8>& out) const
{
    out << "<INSTANCENAME CLASSNAME=\"" << _className << "\">\n";

    for (Uint32 i = 0, n = _keyBindings.getSize(); i < n; i++)
    {
	out << "<KEYBINDING NAME=\"" << _keyBindings[i].getName() << "\">\n";

	out << "<KEYVALUE VALUETYPE=\"";
	out << KeyBinding::typeToString(_keyBindings[i].getType());
	out << "\">";

	out << _keyBindings[i].getValue();
	out << "</KEYVALUE>\n";

	out << "</KEYBINDING>\n";
    }

    out << "</INSTANCENAME>\n";
}

void Reference::toXml(Array<Sint8>& out) const
{
    out << "<VALUE.REFERENCE>\n";

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).

    if (_keyBindings.getSize())
    {
	if (_host.getLength())
	{
	    out << "<INSTANCEPATH>\n";
	    nameSpaceToXml(out);
	    instanceNameToXml(out);
	    out << "</INSTANCEPATH>";
	}
	else if (_nameSpace.getLength())
	{
	    out << "<LOCALINSTANCEPATH>\n";
	    localNameSpaceToXml(out);
	    instanceNameToXml(out);
	    out << "</LOCALINSTANCEPATH>";
	}
	else
	    instanceNameToXml(out);
    }
    else
    {
	if (_host.getLength())
	{
	    out << "<CLASSPATH>\n";
	    nameSpaceToXml(out);
	    classNameToXml(out);
	    out << "</CLASSPATH>";
	}
	else if (_nameSpace.getLength())
	{
	    out << "<LOCALCLASSPATH>\n";
	    nameSpaceToXml(out);
	    classNameToXml(out);
	    out << "</LOCALCLASSPATH>";
	}
	else
	    classNameToXml(out);
    }

    out << "</VALUE.REFERENCE>\n";
}

void Reference::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

const char* KeyBinding::typeToString(Type type)
{
    switch (type)
    {
	case KeyBinding::BOOLEAN: 
	    return "boolean"; 

	case KeyBinding::STRING:
	    return "string"; 

	case KeyBinding::NUMERIC:
	    return "numeric"; 
    }

    return "unknown";
}

PEGASUS_NAMESPACE_END
