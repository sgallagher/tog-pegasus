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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:	Karl Schopmeyer (k.schopmeyer@opengroup.org)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMQualifierList.h"
#include "DeclContext.h"
#include "Resolver.h"
#include "CIMQualifierDecl.h"
#include "CIMName.h"
#include "Indentor.h"
#include "XmlWriter.h"
#include "MofWriter.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

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
    if (qualifier.isUninitialized())
	throw UninitializedObjectException();

    if (find(qualifier.getName()) != PEG_NOT_FOUND){
    	//l10n
		//throw AlreadyExistsException
            //("qualifier \"" + qualifier.getName().getString () + "\"");
        MessageLoaderParms parms("Common.CIMQualifierList.QUALIFIER",
        						 "qualifier \"$0\"",
        						 qualifier.getName().getString());
        throw AlreadyExistsException(parms);
    }

    _qualifiers.append(qualifier);

    return *this;
}
//ATTN: Why do we not do the outofbounds check here. KS 18 May 2k
CIMQualifier& CIMQualifierList::getQualifier(Uint32 index)
{
    return _qualifiers[index];
}

//ATTN: added ks 18 may 2001. Should we have outofbounds?
void CIMQualifierList::removeQualifier(Uint32 index)
{
    _qualifiers.remove(index);
}

Uint32 CIMQualifierList::find(const CIMName& name) const
{
    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
    {
	if (name.equal(_qualifiers[i].getName()))
	    return i;
    }

    return PEG_NOT_FOUND;
}
Boolean CIMQualifierList::isTrue(const CIMName& name) const
{
    Uint32 index = find(name);

    if (index == PEG_NOT_FOUND)
	return false;

    Boolean flag;
    const CIMValue& value = getQualifier(index).getValue();

    if (value.getType() != CIMTYPE_BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

Uint32 CIMQualifierList::findReverse(const CIMName& name) const
{
    for (Uint32 i = _qualifiers.size(); i; --i)
    {
	if (name.equal(_qualifiers[i-1].getName()))
	    return i - 1;
    }

    return PEG_NOT_FOUND;
}

void CIMQualifierList::resolve(
    DeclContext* declContext,
    const CIMNamespaceName & nameSpace,
    CIMScope scope, 			 // Scope of the entity being resolved.
    Boolean isInstancePart,
    CIMQualifierList& inheritedQualifiers,
    Boolean propagateQualifiers)	// Apparently not used ks 24 mar 2002
{
    PEG_METHOD_ENTER(TRC_OBJECTRESOLUTION, "CIMQualifierList::resolve()");
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
		// set this back to  CIMConstQualifierDecl
		CIMQualifierDecl qd = declContext->lookupQualifierDecl(
			nameSpace, q.getName());
	
		if (qd.isUninitialized())
			throw UndeclaredQualifier(q.getName().getString ());
	
		//----------------------------------------------------------------------
		// 2. Check the type and isArray.  Must be the same:
		//----------------------------------------------------------------------
	
		if (!(q.getType() == qd.getType() && q.isArray() == qd.isArray()))
			throw BadQualifierType(q.getName().getString ());
	
		//----------------------------------------------------------------------
		// 3. Check the scope: Must be legal for this qualifier
		//----------------------------------------------------------------------
//#if 0
			// ATTN:  These lines throw a bogus exception if the qualifier has
			// a valid scope (such as Scope::ASSOCIATION) which is not Scope::CLASS
			// ks Mar 2002. Reinstalled 23 March 2002 to test.

		if (!(qd.getScope().hasScope (scope)))
			throw BadQualifierScope
                            (qd.getName().getString (), scope.toString ());
//#endif
		//----------------------------------------------------------------------
		// Resolve the qualifierflavor. Since Flavors are a combination of inheritance
		// and input characteristics, resolve the inherited characteristics
		// against those provided with the creation.  If there is a superclass
		// the flavor is resolved against that superclass.  If not, it is resolved
		// against the declaration. If the flavor is disableoverride and tosubclass
		// the resolved qualifier value must be identical to the original
		//----------------------------------------------------------------------
		// Test for Qualifier found in SuperClass. If found and qualifier
		// is not overridable.
		// Abstract (not Overridable and restricted) can be found in subclasses
		// Can have nonabstracts below abstracts. That is function of nottosubclass
		// Association (notOverridable and tosubclass) can be found in subclasses but
		// cannot be changed. No non-associatons below associations. In other words
		// once a class becomes an association, no subclass can override that definition
		// apparently
		// Throw exception if DisableOverride and tosubclass and different value.
		// Gets the source from superclass if qualifier exists or from declaraction
		// If we do not throw the exception, resolve the flavor from the inheritance
		// point and resolve against current input.
		// Diableoverride is defined in the CIM Spec to mean that the value cannot change
		// The other characteristics including the flavor can change apparently. Thus, we
		// need only confirm that the value is the same (2.2 pp 33).  Strange since we 
		// would think that  override implies that you cannot override any of the 
		// characteristics (value, type, flavor, etc.) This also leaves the question
		// of NULL or no values.  The implication is that we must move the value
		// from the superclass or declaration.

		Uint32 index = inheritedQualifiers.find(q.getName());

		//cout << "KSTEST Qualifier resolve inherit test " << q.getName() 
		//<< " Inherited From " << ((index == PEG_NOT_FOUND) ? "Declaration" : "superclass")
		//<< " Flavor " << q.getFlavor() 
		//<< " inherited Flavor ";

		if (index == PEG_NOT_FOUND)
		{   // Qualifier does not exist in superclass
			/* If from declaration, we can override the default value.
			   However, we need some way to get the value if we have a Null.
			if (!(qd.getFlavor ().hasFlavor 
                               (CIMFlavor::OVERRIDABLE))
                            && qd.getFlavor ().hasFlavor 
                               (CIMFlavor::TOSUBCLASS))
			{
				if(!(q.getValue() == qd.getValue()))
					cout << "KSTEST QL err NSCL " << q.getName()
					<< " decl flavor " << qd.getFlavor() << " Flavor " << q.getFlavor()
					<< " Not override " 
                                        << !(qd.getFlavor ().hasFlavor
                                           (CIMFlavor::OVERRIDABLE))
					<< " tosubclass " 
                                        <<  qd.getFlavor ().hasFlavor
                                            (CIMFlavor::TOSUBCLASS) << endl;
				XmlWriter::printQualifierDeclElement(qd);
				XmlWriter::printQualifierElement(q);
					//throw BadQualifierOverride(q.getName());
			}
			//cout <<  qd.getFlavor() << endl;*/
			// Do not allow change from disable override to enable override.
			if ((!qd.getFlavor ().hasFlavor
                               (CIMFlavor::OVERRIDABLE))
                           && (q.getFlavor ().hasFlavor
                               (CIMFlavor::OVERRIDABLE)))
				throw BadQualifierOverride
                                    (q.getName().getString ());

			Resolver::resolveQualifierFlavor 
                            (q, CIMFlavor (qd.getFlavor ()), false);
			/*if(!(q.getValue() == qd.getValue()))
				cout << "KSTEST Flavor resolved from decl. " << q.getName()
				<< " decl flavor " << qd.getFlavor().toString ()
                                << " Flavor " << q.getFlavor().toString ()
				<< " Not override " 
                                << !(qd.getFlavor ().hasFlavor 
                                       (CIMFlavor::OVERRIDABLE))
				<< " tosubclass " <<  qd.getFlavor ().hasFlavor
                                       (CIMFlavor::TOSUBCLASS) << endl;            
			 XmlWriter::printQualifierDeclElement(qd);
			 XmlWriter::printQualifierElement(q); */
		}
		else   			// qualifier exists in superclass 
		{	////// Make Const again
			CIMQualifier iq = inheritedQualifiers.getQualifier(index);
			// don't allow change override to notoverride.
			if (!(iq.getFlavor ().hasFlavor
                               (CIMFlavor::OVERRIDABLE)) 
                           && q.getFlavor ().hasFlavor (CIMFlavor::OVERRIDABLE))
				throw BadQualifierOverride
                                    (q.getName().getString ());
			
			if (!(iq.getFlavor ().hasFlavor
                               (CIMFlavor::OVERRIDABLE)) 
                            && iq.getFlavor ().hasFlavor
                               (CIMFlavor::TOSUBCLASS))
			{
				/*if(!(q.getValue() == iq.getvalue()))
					cout << "KSTEST QL err inherit " << q.getName()
					<< " from superclass " << iq.getName() 
					<< "   Superclass flavor " << iq.getFlavor().toString ()
					<< " Flavor " << q.getFlavor().toString () 
					<< endl;
			        XmlWriter::printQualifierElement(iq);
			        XmlWriter::printQualifierElement(q); */
				// test if values the same.
				CIMValue qv = q.getValue();
				CIMValue iqv = iq.getValue();
				if(!(qv == iqv)) {
					throw BadQualifierOverride
                                            (q.getName().getString ());
			  }
			} 
			//cout << iq.getFlavor()  << endl;
			Resolver::resolveQualifierFlavor 
                            (q, CIMFlavor (iq.getFlavor ()), true);	
		}
    } 					// end of this objects qualifier loop

    //--------------------------------------------------------------------------
    // Propagate qualifiers to subclass or to instance that do not have
    // already have those qualifiers:
    //--------------------------------------------------------------------------
    //cout << "KSTEST. Inherited qualifiers ct = " << inheritedQualifiers.getCount() << endl;

    for (Uint32 i = 0, n = inheritedQualifiers.getCount(); i < n; i++)
    {
		CIMQualifier iq = inheritedQualifiers.getQualifier(i);
		//cout << "KSTEST inherited qualifier propagate loop " <<  iq.getName() 
		//<< " flavor " << iq.getFlavor << " count " << i << endl;
	
		if (isInstancePart)
		{
			if (!(iq.getFlavor ().hasFlavor
                            (CIMFlavor::TOINSTANCE)))
				continue;
		}
		else
		{
			if (!(iq.getFlavor ().hasFlavor
                            (CIMFlavor::TOSUBCLASS)))
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

void CIMQualifierList::toXml(Array<char>& out) const
{
    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
	XmlWriter::appendQualifierElement(out, _qualifiers[i]);
}

/** toMof - Generates MOF output for a list of CIM Qualifiers.
    The qualifiers may be class, property, parameter, etc.
    The BNF for this is:
    <pre>
    qualifierList       = "[" qualifier *( "," qualifier ) "]"
    </pre>
    Produces qualifiers as a string on without nl.
    */
void CIMQualifierList::toMof(Array<char>& out) const
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
	MofWriter::appendQualifierElement(out, _qualifiers[i]);
    }
    
    // Terminating bracket
    out << "]";
}


void CIMQualifierList::print(PEGASUS_STD(ostream) &os) const
{
    Array<char> tmp;
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
    x._qualifiers.reserveCapacity(_qualifiers.size());

    for (Uint32 i = 0, n = _qualifiers.size(); i < n; i++)
	x._qualifiers.append(_qualifiers[i].clone());
}

PEGASUS_NAMESPACE_END
