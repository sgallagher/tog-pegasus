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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QualifierList_h
#define Pegasus_QualifierList_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Pair.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;

class PEGASUS_COMMON_LINKAGE CIMQualifierList
{
public:

    ///
    CIMQualifierList();

    ///
    ~CIMQualifierList();

    ///
    CIMQualifierList& add(const CIMQualifier& qualifier);

    ///
    Uint32 getCount() const
    {
	return _qualifiers.size(); 
    }

    ///
    CIMQualifier& getQualifier(Uint32 pos);

    ///
    const CIMQualifier& getQualifier(Uint32 pos) const
    {
	return _qualifiers[pos]; 
    }
    /** removeQualifier - Removes the Qualifier defined by
    the pos parameter
    @exception Throws "OutOfBounds" if pos not within
    range of current qualifiers.
    */
    void removeQualifier(Uint32 pos);
    
    ///
    Uint32 find(const String& name) const;

    ///
    Uint32 findReverse(const String& name) const;
    
    /** resolve - Resolves the qualifierList based on the information provided. The resolved
		qualifiers are the result of validating and testing the qualifiers against the
		inherited qualifiers and qualifier declarations.  The qualifier list contains
		the set of resolved qualifiers when the function is complete.
		Resolution includes:
		1. Determinign if the qualifier is declared (obtaniable form declContext).
		2. Same type as declaration
		3. Valid for the scope provided (Qualifier scope contains the provided scope).
		4. Whether qualifier can be overridden.
		5. Whether it should be propagated to subclass
		If a qualifier can be overridden it is put into the qualifiers array.
        @param declContext	- Declaration context for this resolution
        @param nameSpace Namespace in which to find the declaration.
        @param scope - Scope of the entity doing the resolution
        @param isInstancePart - TBD
        @param inheritedQualifiers - CimQualifierList defining List of inherited qualifiers
		@param propagateQualifiers Boolean indicator whether to propagate qualifiers.
        @return 
    	@exception - There are a number of different 
    */ 
    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Uint32 scope,
	Boolean isInstancePart,
	CIMQualifierList& inheritedQualifiers,
	Boolean propagateQualifiers);

    ///
    void toXml(Array<Sint8>& out) const;

    ///
    void toMof(Array<Sint8>& out) const;

    ///
    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    ///
    Boolean identical(const CIMQualifierList& x) const;

    ///
    void cloneTo(CIMQualifierList& x) const;

private:

    Array<CIMQualifier> _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierList_h */
