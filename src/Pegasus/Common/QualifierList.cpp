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
// $Log: QualifierList.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:09  mike
// Pegasus import
//
//
//END_HISTORY

#include "QualifierList.h"
#include "DeclContext.h"
#include "QualifierDecl.h"
#include "Name.h"
#include "Indentor.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

QualifierList& QualifierList::add(const Qualifier& qualifier)
{
    if (!qualifier)
	throw UnitializedHandle();

    if (find(qualifier.getName()) != Uint32(-1))
	throw AlreadyExists();

    _qualifiers.append(qualifier);

    return *this;
}

Uint32 QualifierList::find(const String& name) const
{
    for (Uint32 i = 0, n = _qualifiers.getSize(); i < n; i++)
    {
	if (Name::equal(_qualifiers[i].getName(), name))
	    return i;
    }
    
    return Uint32(-1);
}

void QualifierList::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    Uint32 scope,
    Boolean isInstancePart,
    QualifierList& inheritedQualifiers)
{
    // For each qualifier in the qualifiers array, the following 
    // is checked:
    //
    //     1. Whether it is declared (can be obtained from the declContext).
    //
    //     2. Whether it has the same type as the declaration.
    //
    //	   3. Whether the the qualifier is valid for the given scope.
    //
    //	   4. Whether the qualifier can be overriden (the flavor is
    //	      ENABLEOVERRIDE on the corresponding reference qualifier).
    //
    //	   5. Whether the qualifier should be propagated to the subclass.
    //
    // If the qualifier should be overriden, then it is injected into the 
    // qualifiers array (from the inheritedQualifiers array).

    for (Uint32 i = 0, n = _qualifiers.getSize(); i < n; i++)
    {
	Qualifier q = _qualifiers[i];

	//----------------------------------------------------------------------
	// 1. Check to see if it's declared.
	//----------------------------------------------------------------------

	ConstQualifierDecl qd = declContext->lookupQualifierDecl(
	    nameSpace, q.getName());

	if (!qd)
	    throw UndeclaredQualifier(q.getName());

	//----------------------------------------------------------------------
	// 2. Check the type:
	//----------------------------------------------------------------------

	if (q.getType() != qd.getType())
	    throw BadQualifierType(q.getName());

	//----------------------------------------------------------------------
	// 3. Check the scope:
	//----------------------------------------------------------------------

	if (!(qd.getScope() & scope))
	    throw BadQualifierScope(qd.getName());

	//----------------------------------------------------------------------
	// See if this qualifier is contained in the inheritedQualifiers. If
	// so then we must handle the OVERRIDABLE flavor.
	//----------------------------------------------------------------------

	Uint32 pos = inheritedQualifiers.find(q.getName());

	if (pos != Uint32(-1))
	{
	    ConstQualifier iq = inheritedQualifiers.getQualifier(pos);

	    if (!(iq.getFlavor() & Flavor::OVERRIDABLE))
		throw BadQualifierOverride(q.getName());
	}
    }

    //--------------------------------------------------------------------------
    // Propagate qualifiers to subclass or to instance that do not have 
    // already have those qualifiers:
    //--------------------------------------------------------------------------

    for (Uint32 i = 0, n = inheritedQualifiers.getCount(); i < n; i++)
    {
	Qualifier iq = inheritedQualifiers.getQualifier(i);

	if (isInstancePart)
	{
	    if (!(iq.getFlavor() & Flavor::TOINSTANCE))
		continue;
	}
	else
	{
	    if (!(iq.getFlavor() & Flavor::TOSUBCLASS))
		continue;
	}

	// If the qualifiers list does not already contain this qualifier,
	// then propagate it (and set the propagated flag to true).

	if (find(iq.getName()) != Uint32(-1))
	    continue;

	_qualifiers.prepend(iq);
    }
}

void QualifierList::toXml(Array<Sint8>& out) const
{
    for (Uint32 i = 0, n = _qualifiers.getSize(); i < n; i++)
	_qualifiers[i].toXml(out);
}

void QualifierList::print() const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    std::cout << tmp.getData() << std::endl;
}

Boolean QualifierList::identical(const QualifierList& x) const
{
    Uint32 count = getCount();

    if (count != x.getCount())
	return false;

    for (Uint32 i = 0; i < count; i++)
	return _qualifiers[i].identical(x._qualifiers[i]);

    return true;
}

void QualifierList::cloneTo(QualifierList& x) const
{
    x._qualifiers.clear();
    x._qualifiers.reserve(_qualifiers.getSize());

    for (Uint32 i = 0, n = _qualifiers.getSize(); i < n; i++)
	x._qualifiers.append(_qualifiers[i].clone());
}

PEGASUS_NAMESPACE_END
