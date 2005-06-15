//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Adrian Schuur (schuur@de.ibm.com) - PEP 164
//
// Modified By: 
//     Dave Sudlik (dsudlik@us.ibm.com)
//     David Dillard, VERITAS Software Corp. (david.dillard@veritas.com)
//     Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3666
//     Michael Brasher (mike-brasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "XmlWriter.h"
#include "XmlReader.h"
#include "XmlParser.h"
#include "CIMName.h"
#include "BinaryStreamer.h"
#include "CIMClassRep.h"
#include "CIMInstanceRep.h"
#include "CIMMethodRep.h"
#include "CIMParameterRep.h"
#include "CIMPropertyRep.h"
#include "CIMQualifierRep.h"
#include "CIMValue.h"
#include "CIMValueRep.h"
#include "Packer.h"

#define MAGIC_BYTE Uint8(0x11)
#define VERSION_NUMBER Uint8(1)

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

enum BinaryObjectType
{
    BINARY_CLASS,
    BINARY_INSTANCE,
    BINARY_QUALIFIER_DECL
};

static inline void _packMagicByte(Array<char>& out)
{
    Packer::packUint8(out, MAGIC_BYTE);
}

static void _checkMagicByte(const Array<char>& in, Uint32& pos)
{
    Uint8 magicByte;
    Packer::unpackUint8(in, pos, magicByte);

    if (magicByte != MAGIC_BYTE)
	throw BinException("Bad magic byte");
}

struct Header
{
    // A version number for this message.
    Uint8 versionNumber;

    // The object type (see BinaryObjectType enum).
    Uint8 objectType; 
};

static void _packHeader(Array<char>& out, Uint8 objectType)
{
    Packer::packUint8(out, VERSION_NUMBER);
    Packer::packUint8(out, objectType);
}

static void _checkHeader(
    const Array<char>& in, Uint32& pos, Uint8 expectedObjectType)
{
    Header header;
    Packer::unpackUint8(in, pos, header.versionNumber);
    Packer::unpackUint8(in, pos, header.objectType);

    if (header.objectType != expectedObjectType)
	throw BinException("Unexpected object type");

    if (header.versionNumber != VERSION_NUMBER)
	throw BinException("Unsupported version");
}

inline void _unpack(const Array<char>& in, Uint32& pos, Boolean& x)
{
    Packer::unpackBoolean(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Uint8& x)
{
    Packer::unpackUint8(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Sint8& x)
{
    Packer::unpackUint8(in, pos, (Uint8&)x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Uint16& x)
{
    Packer::unpackUint16(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Sint16& x)
{
    Packer::unpackUint16(in, pos, (Uint16&)x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Uint32& x)
{
    Packer::unpackUint32(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Sint32& x)
{
    Packer::unpackUint32(in, pos, (Uint32&)x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Uint64& x)
{
    Packer::unpackUint64(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Sint64& x)
{
    Packer::unpackUint64(in, pos, (Uint64&)x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Real32& x)
{
    Packer::unpackReal32(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Real64& x)
{
    Packer::unpackReal64(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, Char16& x)
{
    Packer::unpackChar16(in, pos, x);
}

inline void _unpack(const Array<char>& in, Uint32& pos, String& x)
{
    Packer::unpackString(in, pos, x);
}

void _unpack(const Array<char>& in, Uint32& pos, CIMDateTime& x)
{
    String tmp;
    Packer::unpackString(in, pos, tmp);
    x.set(tmp);
}

void _unpack(const Array<char>& in, Uint32& pos, CIMObjectPath& x)
{
    String tmp;
    Packer::unpackString(in, pos, tmp);
    x.set(tmp);
}

template<class T>
struct UnpackArray
{
    static void func(
	const Array<char>& in, Uint32& pos, size_t n, CIMValue& value)
    {
	Array<T> array;
	array.reserveCapacity(n);

	for (size_t i = 0; i < n; i++)
	{
	    T tmp;
	    _unpack(in, pos, tmp);
	    array.append(tmp);
	}

	value.set(array);
    }
};

template<class T>
struct UnpackScalar
{
    static void func(
	const Array<char>& in, Uint32& pos, CIMValue& value)
    {
	T tmp;
	_unpack(in, pos, tmp);
	value.set(tmp);
    }
};

template<class OBJECT>
struct UnpackQualifiers
{
    static void func(const Array<char>& in, Uint32& pos, OBJECT& x)
    {
	Uint32 n;
	Packer::unpackSize(in, pos, n);

	CIMQualifier q;

	for (size_t i = 0; i < n; i++)
	{
	    BinaryStreamer::_unpackQualifier(in, pos, q);
	    x.addQualifier(q);
	}
    }
};

template<class REP>
struct PackQualifiers
{
    static void func(Array<char>& out, REP* rep)
    {
	Uint32 n = rep->getQualifierCount();
	Packer::packSize(out, n);

	for (Uint32 i = 0; i < n; i++)
	    BinaryStreamer::_packQualifier(out, rep->getQualifier(i));
    }
};

template<class OBJECT>
struct UnpackProperties
{
    static void func(const Array<char>& in, Uint32& pos, OBJECT& x)
    {
	Uint32 n;
	Packer::unpackSize(in, pos, n);

	CIMProperty p;

	for (size_t i = 0; i < n; i++)
	{
	    BinaryStreamer::_unpackProperty(in, pos, p);
	    x.addProperty(p);
	}
    }
};

template<class OBJECT>
struct UnpackMethods
{
    static void func(const Array<char>& in, Uint32& pos, OBJECT& x)
    {
	Uint32 n;
	Packer::unpackSize(in, pos, n);

	CIMMethod m;

	for (size_t i = 0; i < n; i++)
	{
	    BinaryStreamer::_unpackMethod(in, pos, m);
	    x.addMethod(m);
	}
    }
};

void BinaryStreamer::_packName(Array<char>& out, const CIMName& x)
{
    Packer::packString(out, x.getString());
}

void BinaryStreamer::_unpackName(
    const Array<char>& in, Uint32& pos, CIMName& x)
{
    String tmp;
    Packer::unpackString(in, pos, tmp);
    x = tmp.size() ? CIMName(tmp) : CIMName();
}

void BinaryStreamer::_packQualifier(Array<char>& out, const CIMQualifier& x)
{
    CIMQualifierRep* rep = x._rep;

    _packMagicByte(out);
    _packName(out, rep->getName());
    _packValue(out, rep->getValue());
    _packFlavor(out, rep->getFlavor());
    Packer::packBoolean(out, rep->getPropagated());
}

void BinaryStreamer::_unpackQualifier(
    const Array<char>& in, Uint32& pos, CIMQualifier& x)
{
    _checkMagicByte(in, pos);

    CIMName name;
    _unpackName(in, pos, name);

    CIMValue value;
    _unpackValue(in, pos, value);

    CIMFlavor flavor;
    BinaryStreamer::_unpackFlavor(in, pos, flavor);

    Boolean propagated;
    Packer::unpackBoolean(in, pos, propagated);

    x = CIMQualifier(name, value, flavor, propagated);
}

void BinaryStreamer::_packValue(Array<char>& out, const CIMValue& x)
{
    CIMValueRep* rep = x._rep;

    _packMagicByte(out);
    _packType(out, x.getType());
    Packer::packBoolean(out, x.isArray());

    Uint32 n = x.getArraySize();

    if (x.isArray())
	Packer::packSize(out, n);

    Packer::packBoolean(out, x.isNull());

    if (x.isNull())
	return;

    if (x.isArray())
    {
	switch (x.getType()) 
	{
	    case CIMTYPE_BOOLEAN:
		Packer::packBoolean(out, rep->_u._booleanArray->getData(), n);
		break;

	    case CIMTYPE_SINT8:
	    case CIMTYPE_UINT8:
		Packer::packUint8(out, rep->_u._uint8Array->getData(), n);
		break;

	    case CIMTYPE_SINT16:
	    case CIMTYPE_UINT16:
	    case CIMTYPE_CHAR16:
		Packer::packUint16(out, rep->_u._uint16Array->getData(), n);
		break;

	    case CIMTYPE_SINT32:
	    case CIMTYPE_UINT32:
	    case CIMTYPE_REAL32:
		Packer::packUint32(out, rep->_u._uint32Array->getData(), n);
		break;

	    case CIMTYPE_SINT64:
	    case CIMTYPE_UINT64:
	    case CIMTYPE_REAL64:
		Packer::packUint64(out, rep->_u._uint64Array->getData(), n);
		break;

	    case CIMTYPE_STRING:
		Packer::packString(out, rep->_u._stringArray->getData(), n);
		break;

	    case CIMTYPE_DATETIME:
	    {
		for (Uint32 i = 0; i < n; i++) 
	        {
		    Packer::packString(out,
			(*(rep->_u._dateTimeArray))[i].toString());
                }
		break;
	    }

	    case CIMTYPE_REFERENCE:
	    {
		for (Uint32 i = 0; i < n; i++) 
	        {
		    Packer::packString(out,
			(*(rep->_u._referenceArray))[i].toString());
                }
		break;
	    }

	    case CIMTYPE_OBJECT:
		break;
	}
    }
    else
    {
	switch (x.getType()) 
	{
	    case CIMTYPE_BOOLEAN:
		Packer::packBoolean(out, rep->_u._booleanValue ? true : false);
		break;

	    case CIMTYPE_SINT8:
	    case CIMTYPE_UINT8:
		Packer::packUint8(out, rep->_u._uint8Value);
		break;

	    case CIMTYPE_SINT16:
	    case CIMTYPE_UINT16:
	    case CIMTYPE_CHAR16:
		Packer::packUint16(out, rep->_u._uint16Value);
		break;

	    case CIMTYPE_SINT32:
	    case CIMTYPE_UINT32:
	    case CIMTYPE_REAL32:
		Packer::packUint32(out, rep->_u._uint32Value);
		break;

	    case CIMTYPE_SINT64:
	    case CIMTYPE_UINT64:
	    case CIMTYPE_REAL64:
		Packer::packUint64(out, rep->_u._uint64Value);
		break;

	    case CIMTYPE_STRING:
		Packer::packString(out, *rep->_u._stringValue);
		break;

	    case CIMTYPE_DATETIME:
		Packer::packString(out, rep->_u._dateTimeValue->toString());
		break;

	    case CIMTYPE_REFERENCE:
		Packer::packString(out, rep->_u._referenceValue->toString());
		break;

	    case CIMTYPE_OBJECT:
		break;
	}
    }
}

void BinaryStreamer::_unpackValue(
    const Array<char>& in, Uint32& pos, CIMValue& x)
{
    _checkMagicByte(in, pos);

    CIMType type;
    _unpackType(in, pos, type);

    Boolean isArray;
    Packer::unpackBoolean(in, pos, isArray);

    Uint32 arraySize = 0;

    if (isArray)
	Packer::unpackSize(in, pos, arraySize);

    Boolean isNull;
    Packer::unpackBoolean(in, pos, isNull);

    if (isNull)
    {
	x = CIMValue(type, isArray, arraySize);
	return;
    }

    if (isArray)
    {
	CIMValue cimValue(type, isArray, arraySize);

	switch (type) 
	{
	    case CIMTYPE_BOOLEAN:
		UnpackArray<Boolean>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_UINT8:
		UnpackArray<Uint8>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_SINT8:
		UnpackArray<Sint8>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_UINT16:
		UnpackArray<Uint16>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_SINT16:
		UnpackArray<Sint16>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_UINT32:
		UnpackArray<Uint32>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_SINT32:
		UnpackArray<Sint32>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_UINT64:
		UnpackArray<Uint64>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_SINT64:
		UnpackArray<Sint64>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_REAL32:
		UnpackArray<Real32>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_REAL64:
		UnpackArray<Real64>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_CHAR16:
		UnpackArray<Char16>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_STRING:
		UnpackArray<String>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_DATETIME:
		UnpackArray<CIMDateTime>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_REFERENCE:
		UnpackArray<CIMObjectPath>::func(in, pos, arraySize, cimValue);
		break;

	    case CIMTYPE_OBJECT:
		break;
	}

	x = cimValue;
    }
    else
    {
	CIMValue cimValue(type, isArray);

	switch (type) 
	{
	    case CIMTYPE_BOOLEAN:
		UnpackScalar<Boolean>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_UINT8:
		UnpackScalar<Uint8>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_SINT8:
		UnpackScalar<Sint8>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_UINT16:
		UnpackScalar<Uint16>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_SINT16:
		UnpackScalar<Sint16>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_UINT32:
		UnpackScalar<Uint32>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_SINT32:
		UnpackScalar<Sint32>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_UINT64:
		UnpackScalar<Uint64>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_SINT64:
		UnpackScalar<Sint64>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_REAL32:
		UnpackScalar<Real32>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_REAL64:
		UnpackScalar<Real64>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_CHAR16:
		UnpackScalar<Char16>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_STRING:
		UnpackScalar<String>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_DATETIME:
		UnpackScalar<CIMDateTime>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_REFERENCE:
		UnpackScalar<CIMObjectPath>::func(in, pos, cimValue);
		break;

	    case CIMTYPE_OBJECT:
		break;
	}

	x = cimValue;
    }

    return;
}

void BinaryStreamer::_packProperty(Array<char>& out, const CIMProperty& x)
{
    CIMPropertyRep* rep = x._rep;

    _packMagicByte(out);
    _packName(out, rep->getName());
    _packValue(out, rep->getValue());
    Packer::packSize(out, rep->getArraySize());
    _packName(out, rep->getReferenceClassName());
    _packName(out, rep->getClassOrigin());
    Packer::packBoolean(out, rep->getPropagated());
    PackQualifiers<CIMPropertyRep>::func(out, rep);
}

void BinaryStreamer::_unpackProperty(
    const Array<char>& in, Uint32& pos, CIMProperty& x)
{
    _checkMagicByte(in, pos);

    CIMName name;
    _unpackName(in, pos, name);

    CIMValue value;
    _unpackValue(in, pos, value);

    Uint32 arraySize;
    Packer::unpackSize(in, pos, arraySize);

    CIMName referenceClassName;
    _unpackName(in, pos, referenceClassName);

    CIMName classOrigin;
    _unpackName(in, pos, classOrigin);

    Boolean propagated;
    Packer::unpackBoolean(in, pos, propagated);

    CIMProperty cimProperty(
	name, value, arraySize, referenceClassName, classOrigin, propagated);

    UnpackQualifiers<CIMProperty>::func(in, pos, cimProperty);

    x = cimProperty;
}

void BinaryStreamer::_packParameter(Array<char>& out, const CIMParameter& x)
{
    CIMParameterRep* rep = x._rep;

    _packMagicByte(out);
    _packName(out, rep->getName());
    _packType(out, rep->getType());
    Packer::packBoolean(out, rep->isArray());
    Packer::packSize(out, rep->getArraySize());
    _packName(out, rep->getReferenceClassName());
    PackQualifiers<CIMParameterRep>::func(out, rep);
}

void BinaryStreamer::_unpackParameter(
    const Array<char>& in, Uint32& pos, CIMParameter& x)
{
    _checkMagicByte(in, pos);

    CIMName name;
    _unpackName(in, pos, name);

    CIMType type;
    _unpackType(in, pos, type);

    Boolean isArray;
    Packer::unpackBoolean(in, pos, isArray);

    Uint32 arraySize;
    Packer::unpackSize(in, pos, arraySize);

    CIMName referenceClassName;
    _unpackName(in, pos, referenceClassName);

    CIMParameter cimParameter(
	name, type, isArray, arraySize, referenceClassName);

    UnpackQualifiers<CIMParameter>::func(in, pos, cimParameter);

    x = cimParameter;
}

void BinaryStreamer::_packParameters(Array<char>& out, CIMMethodRep* rep)
{
    Uint32 n = rep->getParameterCount();
    Packer::packSize(out, n);

    for (Uint32 i = 0; i < n; i++)
	BinaryStreamer::_packParameter(out, rep->getParameter(i));
}

void BinaryStreamer::_unpackParameters(
    const Array<char>& in, Uint32& pos, CIMMethod& x)
{
    Uint32 n;
    Packer::unpackSize(in, pos, n);

    for (size_t i = 0; i < n; i++)
    {
	CIMParameter q;
	_unpackParameter(in, pos, q);
	x.addParameter(q);
    }
}

void BinaryStreamer::_packMethod(Array<char>& out, const CIMMethod& x)
{
    CIMMethodRep* rep = x._rep;

    _packMagicByte(out);
    _packName(out, rep->getName());
    _packType(out, rep->getType());
    _packName(out, rep->getClassOrigin());
    Packer::packBoolean(out, rep->getPropagated());
    PackQualifiers<CIMMethodRep>::func(out, rep);
    _packParameters(out, rep);
}

void BinaryStreamer::_unpackMethod(
    const Array<char>& in, Uint32& pos, CIMMethod& x)
{
    _checkMagicByte(in, pos);

    CIMName name;
    _unpackName(in, pos, name);

    CIMType type;
    _unpackType(in, pos, type);

    CIMName classOrigin;
    _unpackName(in, pos, classOrigin);

    Boolean propagated;
    Packer::unpackBoolean(in, pos, propagated);

    CIMMethod cimMethod(name, type, classOrigin, propagated);
    UnpackQualifiers<CIMMethod>::func(in, pos, cimMethod);
    _unpackParameters(in, pos, cimMethod);

    x = cimMethod;
}

void BinaryStreamer::_packObjectPath(Array<char>& out, const CIMObjectPath& x)
{
    Packer::packString(out, x.toString());
}

void BinaryStreamer::_unpackObjectPath(
    const Array<char>& in, Uint32& pos, CIMObjectPath& x)
{
    String tmp;
    Packer::unpackString(in, pos, tmp);
    x = CIMObjectPath(tmp);
}

void BinaryStreamer::_packProperties(Array<char>& out, CIMObjectRep* rep)
{
    Uint32 n = rep->getPropertyCount();
    Packer::packSize(out, n);

    for (Uint32 i = 0; i < n; i++)
	BinaryStreamer::_packProperty(out, rep->getProperty(i));
}

void BinaryStreamer::_packMethods(Array<char>& out, CIMClassRep* rep)
{
    Uint32 n = rep->getMethodCount();
    Packer::packSize(out, n);

    for (Uint32 i = 0; i < n; i++)
	BinaryStreamer::_packMethod(out, rep->getMethod(i));
}

void BinaryStreamer::_packScope(Array<char>& out, const CIMScope& x)
{
    Packer::packUint32(out, x.cimScope);
}

void BinaryStreamer::_unpackScope(
    const Array<char>& in, Uint32& pos, CIMScope& x)
{
    Packer::unpackUint32(in, pos, x.cimScope);
}

void BinaryStreamer::_packFlavor(Array<char>& out, const CIMFlavor& x)
{
    Packer::packUint32(out, x.cimFlavor);
}

void BinaryStreamer::_unpackFlavor(
    const Array<char>& in, Uint32& pos, CIMFlavor& x)
{
    Packer::unpackUint32(in, pos, x.cimFlavor);
}

void BinaryStreamer::_packType(Array<char>& out, const CIMType& x)
{
    Packer::packUint8(out, Uint8(x));
}

void BinaryStreamer::_unpackType(
    const Array<char>& in, Uint32& pos, CIMType& x)
{
    Uint8 tmp;
    Packer::unpackUint8(in, pos, tmp);
    x = CIMType(tmp);
}

void BinaryStreamer::encode(
    Array<char>& out, 
    const CIMClass& x)
{
    CIMClassRep* rep = x._rep;
    _packMagicByte(out);
    _packHeader(out, BINARY_CLASS);
    _packName(out, x.getClassName());
    _packName(out, x.getSuperClassName());
    PackQualifiers<CIMClassRep>::func(out, rep);
    _packProperties(out, rep);
    _packMethods(out, rep);
    Packer::packBoolean(out, rep->_resolved);
}

void BinaryStreamer::decode(
    const Array<char>& in, 
    unsigned int pos, 
    CIMClass& x)
{
    _checkMagicByte(in, pos);
    _checkHeader(in, pos, BINARY_CLASS);

    CIMName className;
    _unpackName(in, pos, className);

    CIMName superClassName;
    _unpackName(in, pos, superClassName);

    CIMClass cimClass(className, superClassName);

    UnpackQualifiers<CIMClass>::func(in, pos, cimClass);
    UnpackProperties<CIMClass>::func(in, pos, cimClass);
    UnpackMethods<CIMClass>::func(in, pos, cimClass);

    Boolean resolved;
    Packer::unpackBoolean(in, pos, resolved);
    cimClass._rep->_resolved = resolved;
    x = cimClass;
}

void BinaryStreamer::encode(
    Array<char>& out, 
    const CIMInstance& x)
{
    CIMInstanceRep* rep = x._rep;
    _packMagicByte(out);
    _packHeader(out, BINARY_INSTANCE);
    _packObjectPath(out, x.getPath());
    PackQualifiers<CIMInstanceRep>::func(out, rep);
    _packProperties(out, rep);
    Packer::packBoolean(out, rep->_resolved);
}

void BinaryStreamer::decode(
    const Array<char>& in, 
    unsigned int pos, 
    CIMInstance& x)
{
    _checkMagicByte(in, pos);
    _checkHeader(in, pos, BINARY_INSTANCE);

    CIMObjectPath objectPath;
    _unpackObjectPath(in, pos, objectPath);
    CIMInstance cimInstance(objectPath.getClassName());
    cimInstance.setPath(objectPath);

    UnpackQualifiers<CIMInstance>::func(in, pos, cimInstance);
    UnpackProperties<CIMInstance>::func(in, pos, cimInstance);

    Boolean resolved;
    Packer::unpackBoolean(in, pos, resolved);
    cimInstance._rep->_resolved = resolved;
    x = cimInstance;
}

void BinaryStreamer::encode(
    Array<char>& out, 
    const CIMQualifierDecl& x)
{
    _packMagicByte(out);
    _packHeader(out, BINARY_QUALIFIER_DECL);
    _packName(out , x.getName());
    _packValue(out , x.getValue());
    _packScope(out , x.getScope());
    _packFlavor(out , x.getFlavor());
    Packer::packSize(out, x.getArraySize());
}

void BinaryStreamer::decode(
    const Array<char>& in, 
    unsigned int pos, 
    CIMQualifierDecl& x)
{
    _checkMagicByte(in, pos);
    _checkHeader(in, pos, BINARY_QUALIFIER_DECL);

    CIMName qualifierName;
    _unpackName(in, pos, qualifierName);

    CIMValue value;
    _unpackValue(in, pos, value);

    CIMScope scope;
    _unpackScope(in, pos, scope);

    CIMFlavor flavor;
    BinaryStreamer::_unpackFlavor(in, pos, flavor);

    Uint32 arraySize;
    Packer::unpackSize(in, pos, arraySize);

    x = CIMQualifierDecl(qualifierName, value, scope, flavor, arraySize);
}

PEGASUS_NAMESPACE_END
