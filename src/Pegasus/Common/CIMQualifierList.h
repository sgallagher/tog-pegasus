//%/////////////////////////////////////////////////////////////////////////////
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

    CIMQualifierList();

    ~CIMQualifierList();

    CIMQualifierList& add(const CIMQualifier& qualifier);

    Uint32 getCount() const
    {
	return _qualifiers.size(); 
    }

    CIMQualifier& getQualifier(Uint32 pos);

    const CIMQualifier& getQualifier(Uint32 pos) const
    {
	return _qualifiers[pos]; 
    }
    // ATTN: KS 18 may 2001
    /** removeQualifier - Removes the Qualifier defined by
    the pos parameter
    @exception Throws "OutOfBounds" if pos not within
    range of current qualifiers.
    */
    void removeQualifier(Uint32 pos);
    
    Uint32 find(const String& name) const;

    Uint32 findReverse(const String& name) const;

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Uint32 scope,
	Boolean isInstancePart,
	CIMQualifierList& inheritedQualifiers);

    void toXml(Array<Sint8>& out) const;

    void print(PEGASUS_STD(ostream) &o=PEGASUS_STD(cout)) const;

    Boolean identical(const CIMQualifierList& x) const;

    void cloneTo(CIMQualifierList& x) const;

private:

    Array<CIMQualifier> _qualifiers;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierList_h */
