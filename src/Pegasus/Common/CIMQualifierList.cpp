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
// Modified By:	Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMQualifierList.h"
#include "DeclContext.h"
#include "CIMQualifierDecl.h"
#include "CIMName.h"
#include "Indentor.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

CIMQualifierList::CIMQualifierList()
{

}

CIMQualifierList::~CIMQualifierList()
{

}

CIMQualifierList& CIMQualifierList::add(const CIMQualifier& qualifier)
{
    if (!qualifier)
	throw UnitializedHandle();

    if (find(qualifier.getName()) != PEG_NOT_FOUND)
	throw AlreadyExists();

    _qualifiers.append(qualifier);

    return *this;
}
//ATTN: Why do we not do the outofbounds check here. KS 18 May 2k
CIMQualifier& CIMQualifierList::getQualifier(Uint32 pos)
{
    return _qualifiers[pos];
}

//ATTN: added ks 18 may 2001. Should we have outofbounds?
void CIMQualifierList::removeQualifier(Uint32 pos)
{
    _qualifiers.remove(pos);
}

Uint32 CIMQualifierList::find(const String& name) const
{
    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
    {
	if (CIMName::equal(_qualifiers[i].getName(), name))
	    return i;
    }

    return PEG_NOT_FOUND;
}

Uint32 CIMQualifierList::findReverse(const String& name) const
{
    for (Uint32 i = _qualifiers.size(); i; --i)
    {
	if (CIMName::equal(_qualifiers[i - 1].getName(), name))
	    return i - 1;
    }

    return PEG_NOT_FOUND;
}

void CIMQualifierList::resolve(
    DeclContext* declContext,
    const String& nameSpace,
    Uint32 scope, 					// Scope of the entity being resolved.
    Boolean isInstancePart,
    CIMQualifierList& inheritedQualifiers,
    Boolean propagateQualifiers)	// Apparently not used ks 24 mar 2002
{
    // For each qualifier in the qualifiers array, the following
    // is checked:
    //
    //     1. Whether it is declared (can be obtained from the declContext).
    //
    //     2. Whether it has the same type as the declaration.
    //
    //	   3. Whether the qualifier is valid for the given scope.
    //
    //	   4. Whether the qualifier can be overriden (the flavor is
    //	      ENABLEOVERRIDE on the corresponding reference qualifier).
    //
    //	   5. Whether the qualifier should be propagated to the subclass.
    //
    // If the qualifier should be overriden, then it is injected into the
    // qualifiers array (from the inheritedQualifiers array).

    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
    {
		CIMQualifier q = _qualifiers[i];
		//----------------------------------------------------------------------
		// 1. Check to see if it's declared.
		//----------------------------------------------------------------------
	
		CIMConstQualifierDecl qd = declContext->lookupQualifierDecl(
			nameSpace, q.getName());
	
		if (!qd)
			throw UndeclaredQualifier(q.getName());
	
		//----------------------------------------------------------------------
		// 2. Check the type and isArray.  Must be the same:
		//----------------------------------------------------------------------
	
		if (!(q.getType() == qd.getType() && q.isArray() == qd.isArray()))
			throw BadQualifierType(q.getName());
	
		//----------------------------------------------------------------------
		// 3. Check the scope: Must be legal for this qualifier
		//----------------------------------------------------------------------
//#if 0
			// ATTN:  These lines throw a bogus exception if the qualifier has
			// a valid scope (such as Scope::ASSOCIATION) which is not Scope::CLASS
			// ks Mar 2002. Reinstalled 23 March 2002 to test.

		if (!(qd.getScope() & scope))
			throw BadQualifierScope(qd.getName(), ScopeToString(scope));
//#endif
		//----------------------------------------------------------------------
		// See if this qualifier is contained in the inheritedQualifiers. If
		// so then we must handle the OVERRIDABLE flavor.
		//----------------------------------------------------------------------
	
		// ATTN: there seems to be a problem with the CIM schema that marks the
		// abstract qualifier as non-overridable but then they override it.
		// For now it is just disabled. This problem exists in both XML and
		// CIM schema.

		// Move the flavor from declaration
		// Always sets flavor to the declaration flavor.
		q.setFlavor(qd.getFlavor());	
//#if 0
		Uint32 pos = inheritedQualifiers.find(q.getName());

		//cout << "KSTEST Qualifier resolve inherit test " << q.getName() 
		//<< " Inherited Position = " << pos << endl;

		// Test for Qualifier found in SuperClass. If found and qualifier
		// is not overridable, Must be identical.
		// Thus - abstract (not Overridable and restricted) can be found in subclasses
		// I can have nonabstracts below abstracts. No propagation.
		// Association (notOverridable and tosubclass) can be found in subclasses but
		// cannot be changed. No non-aswsociatons below associations..
		// Throw exception if DisableOverride and tosubclass and different value
		if (pos != PEG_NOT_FOUND)
		{
			CIMConstQualifier iq = inheritedQualifiers.getQualifier(pos);
			if (!qd.isFlavor(CIMFlavor::OVERRIDABLE) && qd.isFlavor(CIMFlavor::TOSUBCLASS))
				if(!q.identical(iq))
					throw BadQualifierOverride(q.getName());
		}
//#endif
    } 					// end of this objects qualifier loop

    //--------------------------------------------------------------------------
    // Propagate qualifiers to subclass or to instance that do not have
    // already have those qualifiers:
    //--------------------------------------------------------------------------
	//cout << "KSTEST. Loop of inherited qualifiers. Number = " 
	//	<< inheritedQualifiers.getCount() << endl;

    for (Uint32 i = 0, n = inheritedQualifiers.getCount(); i < n; i++)
    {
		CIMQualifier iq = inheritedQualifiers.getQualifier(i);
			//cout << "KSTEST inherited qualifier propagate loop " <<  iq.getName() 
			//<< " flavor " << iq.getFlavor << " count " << i << endl;
		
			// ATTN-DE-P1-This next test is incorrect. It is a temporary, hard-coded
			// HACK to avoid propagating the "Abstract" Qualifier to subclasses
		//if (CIMName::equal(iq.getName(), "Abstract"))
			//   continue;
		//<< " flavor= " << iq.getFlavor()
		//<< " TOSUBCLASS " << (iq.getFlavor() && CIMFlavor::TOSUBCLASS) << endl;
		
		if (isInstancePart)
		{
			if (!iq.isFlavor(CIMFlavor::TOINSTANCE))
				continue;
		}
		else
		{
			if (!iq.isFlavor(CIMFlavor::TOSUBCLASS))
				continue;
		}
		
		// If the qualifiers list does not already contain this qualifier,
		// then propagate it (and set the propagated flag to true).	 Else we
		// keep current value. Note we have already eliminated any possibity that
		// a nonoverridable qualifier can be in the list.
		// Note that there is no exists() function ATTN:KS 25 Mar 2002
		if(find(iq.getName()) != PEG_NOT_FOUND)
			continue;
			
		CIMQualifier q = iq.clone();
		q.setPropagated(true);
		_qualifiers.prepend(q);
    }
}

void CIMQualifierList::toXml(Array<Sint8>& out) const
{
    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
	_qualifiers[i].toXml(out);
}

/** toMof - Generates MOF output for a list of CIM Qualifiers.
    The qualifiers may be class, property, parameter, etc.
    The BNF for this is:
    <pre>
    qualifierList       = "[" qualifier *( "," qualifier ) "]"
    </pre>
    Produces qualifiers as a string on without nl.
    */
void CIMQualifierList::toMof(Array<Sint8>& out) const
{
    // if no qualifiers, return
    if (_qualifiers.size() == 0)
	return;

    // Qualifier leading bracket.
    out <<"[";

    // Loop to list qualifiers
    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
    {
	// if second or greater, add comma separator
	if (i > 0)
	    out << ", \n";
	_qualifiers[i].toMof(out);
    }
    
    // Terminating bracket
    out << "]";
}


void CIMQualifierList::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

Boolean CIMQualifierList::identical(const CIMQualifierList& x) const
{
    Uint32 count = getCount();

    if (count != x.getCount())
	return false;

    for (Uint32 i = 0; i < count; i++)
    {
	if (!_qualifiers[i].identical(x._qualifiers[i]))
	    return false;
    }

    return true;
}

void CIMQualifierList::cloneTo(CIMQualifierList& x) const
{
    x._qualifiers.clear();
    x._qualifiers.reserve(_qualifiers.size());

    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
	x._qualifiers.append(_qualifiers[i].clone());
}

PEGASUS_NAMESPACE_END
