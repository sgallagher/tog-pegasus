
#include <cctype>
#include "Reference.h"
#include "Indentor.h"
#include "Name.h"
#include "Destroyer.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// KeyValuePair
//
////////////////////////////////////////////////////////////////////////////////

KeyValuePair::KeyValuePair()
{

}

KeyValuePair::KeyValuePair(const String& key_, const String& value_)
    : key(key_), value(value_) 
{ 

}

KeyValuePair& KeyValuePair::operator=(const KeyValuePair& x)
{
    if (&x != this)
    {
	key = x.key;
	value = x.value;
    }
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
    : _className(x._className), _keyValuePairs(x._keyValuePairs)
{

}

Reference::Reference(const String& referencePath) 
{
    setFromReference(referencePath);
}

Reference::Reference(
    const String& className, 
    KeyValuePairList& keyValuePairs)
    : 
    _className(className), 
    _keyValuePairs(keyValuePairs)
{

}

Reference::~Reference()
{

}

Reference& Reference::operator=(const Reference& x)
{
    if (&x != this)
    {
	_className = x._className;
	_keyValuePairs = x._keyValuePairs;
    }
    return *this;
}

void Reference::setClassName(const String& className)
{
    if (!Name::legal(className))
	throw IllegalName();
}

void Reference::setKeyValuePairs(const KeyValuePairList& keyValuePairs) 
{ 
    _keyValuePairs = keyValuePairs; 
}

String Reference::getReference() const
{
    String str = _className;
    str.append('.');

    Uint32 size = _keyValuePairs.getSize();

    for (Uint32 i = 0; i < size; i++)
    {
	str.append(_keyValuePairs[i].key);
	str.append('=');
	str.append(_keyValuePairs[i].value);

	if (i + 1 != size)
	    str.append(",");
    }

    return str;
}

// Get a legal CIM name of the form: "[A-Za-z_][A-Za-z_0-9]*"

static Boolean _getIdent(const char*& p)
{
    // The first character should be in [A-Za-z_]:

    if (!*p || !(isalpha(*p) || *p == '_'))
	return false;

    // Subsequent characters should be in [A-Za-z_0-9]:

    while (*p && (isalnum(*p) || *p == '_'))
	p++;

    return true;
}

void Reference::setFromReference(const String& referencePath)
{
    _className.clear();
    _keyValuePairs.clear();

    CString referencePathCString = referencePath.getCString();
    const char* start = referencePathCString;
    const char* current = start;

    // Parse the class name:

    if (!_getIdent(current))
	throw BadReference("Malformed class name");

    _className.assign(start, current - start);

    // The next character must be either null (in which case it refers to
    // a class) or '.' (in which case it refers to a instance).

    if (!*current)
	return;

    if (*current != '.')
	throw BadReference("Expected '.' after class name");

    current++;

    // While more key-value pairs:

    for (;;)
    {
	//////////////////////////
	// Get key:
	//////////////////////////

	start = current;

	if (!_getIdent(current))
	    throw BadReference("Malformed key name");

	String key(start, current - start + 1);

	if (!*current)
	    throw BadReference("Premature end of reference");

	current++;

	//////////////////////////
	// Get the value:
	//////////////////////////

	String value;

	if (*current == '"')
	{
	    value.append('"');
	    current++;

	    while (*current && *current != '"')
	    {
		if (*current == '\\')
		{
		    value.append(*current++);

		    if (!*current)
			break;
		}

		value.append(*current++);
	    }

	    if (*current != '"')
		throw BadReference("Malformed string literal");

	    value.append('"');
	    current++;
	}
	else if (isdigit(*current))
	{
	    while (*current && isdigit(*current))
		value.append(*current++);
	}
	else
	    throw BadReference("Illegal key");

	//////////////////////////////
	// Append the key-value pair:
	//////////////////////////////

	_keyValuePairs.append(KeyValuePair(key, value));

	//////////////////////////
	// Skip over the comma:
	//////////////////////////

	if (!*current)
	    break;

	if (*current++ != ',')
	    throw BadReference("Expected end of string or another key");
    }
}

Boolean operator==(const Reference& x, const Reference& y)
{
    return 
    x._className == y._className && 
    x._keyValuePairs == y._keyValuePairs;
}

ostream& operator<<(ostream& os, const Reference& x)
{
    return os << x.getReference();
}

PEGASUS_NAMESPACE_END
