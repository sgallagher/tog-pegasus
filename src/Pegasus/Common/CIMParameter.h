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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Parameter_h
#define Pegasus_Parameter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMType.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMParameter
//
////////////////////////////////////////////////////////////////////////////////

class DeclContext;
class CIMConstParameter;
class CIMParameterRep;

class PEGASUS_COMMON_LINKAGE CIMParameter
{
public:

    CIMParameter();

    CIMParameter(const CIMParameter& x);

    // Throws IllegalName if name argument not legal CIM identifier.
    CIMParameter(
	const String& name, 
	CIMType type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY);

    ~CIMParameter();

    CIMParameter& operator=(const CIMParameter& x);

    const String& getName() const ;

    // Throws IllegalName if name argument not legal CIM identifier.
    void setName(const String& name);

    Boolean isArray() const;

    Uint32 getArraySize() const;

    const String& getReferenceClassName() const ;

    CIMType getType() const ;

    void setType(const CIMType type);

    // Throws AlreadyExists.
    CIMParameter& addQualifier(const CIMQualifier& x);

    Uint32 findQualifier(const String& name) const;

    CIMQualifier getQualifier(Uint32 pos);

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

#ifdef PEGASUS_INTERNALONLY
    void resolve(DeclContext* declContext, const String& nameSpace);

    Boolean isNull() const;
#endif

    Boolean identical(const CIMConstParameter& x) const;

    CIMParameter clone() const;

private:

    CIMParameter(CIMParameterRep* rep);

    void _checkRep() const;

    CIMParameterRep* _rep;

    friend class CIMConstParameter;
    friend class XmlWriter;
    friend class MofWriter;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMConstParameter
//
////////////////////////////////////////////////////////////////////////////////

class PEGASUS_COMMON_LINKAGE CIMConstParameter
{
public:

    CIMConstParameter();

    CIMConstParameter(const CIMConstParameter& x);

    CIMConstParameter(const CIMParameter& x);

    // Throws IllegalName if name argument not legal CIM identifier.
    CIMConstParameter(
	const String& name, 
	CIMType type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY);

    ~CIMConstParameter();

    CIMConstParameter& operator=(const CIMConstParameter& x);

    CIMConstParameter& operator=(const CIMParameter& x);

    const String& getName() const;

    Boolean isArray() const;

    Uint32 getArraySize() const;

    const String& getReferenceClassName() const;

    CIMType getType() const;

    Uint32 findQualifier(const String& name) const;

    CIMConstQualifier getQualifier(Uint32 pos) const;

    Uint32 getQualifierCount() const;

#ifdef PEGASUS_INTERNALONLY
    Boolean isNull() const;
#endif

    Boolean identical(const CIMConstParameter& x) const;

    CIMParameter clone() const;

private:

    void _checkRep() const;

    CIMParameterRep* _rep;
    friend class CIMParameter;
    friend class XmlWriter;
    friend class MofWriter;
};

#define PEGASUS_ARRAY_T CIMParameter
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Parameter_h */
