//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

////////////////////////////////////////////////////////////////////////////////
//
// Reference.h
//
//	This class resents the value of a CIM reference. Recall CIM 
//	references contain a class name and one or more key-value pairs.
//	In string form it has this form (CIM reference).
//
//		ClassName.Key1=Value1,Key2=Value2,...,KeyN=ValueN
//
//	For example:
//
//		Employee.id=6578
//
//	At the point this object is constructed, the given class may not
//	be available. Hence the validity of the reference cannot be
//	determined. For example, it cannot be known whether a class with
//	the given name really exists or whether keys with the given names
//	exist. And further, the types of the keys cannot be know. Therefore, 
//	the vadidity of the reference will be determined at some later time.
//
//	References are built by specifying a class name and one or more
//	key value pairs. To extend the above example:
//
//		String className = "Employee";
//		KeyValuePairList keyValuePairs;
//		keyValuePairs.append(KeyValuePair("id", "6578"));
//
//	Again, since validation occurs later, the actual types of the keys
//	are not know when the object is built so strings are used. These
//	are converted to actual values at a later time.
//
//	The Reference(const String&) constructor initializes a Reference 
//	object from a string of the form:
//
//		ClassName.Key1=Value1,Key2=Value2,...,KeyN=ValueN
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Reference_h
#define Pegasus_Reference_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// KeyValuePair
//
////////////////////////////////////////////////////////////////////////////////

struct PEGASUS_LINKAGE KeyValuePair
{
    String key;
    String value;

    KeyValuePair();

    KeyValuePair(const String& key, const String& value);

    KeyValuePair& operator=(const KeyValuePair& x);
};

inline Boolean operator==(const KeyValuePair& x, const KeyValuePair& y)
{
    return x.key == y.key && x.value == y.value;
}

typedef Array<KeyValuePair> KeyValuePairList;

////////////////////////////////////////////////////////////////////////////////
//
// Reference
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_LINKAGE Reference 
{
public:

    Reference();

    Reference(const Reference& x);

    Reference(const String& referencePath);

    Reference(
	const String& className, 
	KeyValuePairList& keyValuePairs);

    virtual ~Reference();

    Reference& operator=(const Reference& x);

    const String& getClassName() const 
    { 
	return _className; 
    }

    void setClassName(const String& className);

    const KeyValuePairList& getKeyValuePairs() const 
    { 
	return _keyValuePairs; 
    }

    void setKeyValuePairs(const KeyValuePairList& keyValuePairs);

    String getReference() const;

    void setFromReference(const String& referencePath);

    friend Boolean operator==(const Reference& x, const Reference& y);

private:

    String _className;
    KeyValuePairList _keyValuePairs;
};

PEGASUS_LINKAGE Boolean operator==(const Reference& x, const Reference& y);

PEGASUS_LINKAGE ostream& operator<<(ostream& os, const Reference& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Reference_h */
