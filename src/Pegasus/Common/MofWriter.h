//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
// The Open Group, Tivoli Systems
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MofWriter_h
#define Pegasus_MofWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE MofWriter
{
public:

    static void appendValueElement(
        Array<Sint8>& out,
        const CIMValue& value);

    static void appendValueReferenceElement(
        Array<Sint8>& out,
        const CIMObjectPath& reference);

    static void appendClassElement(
        Array<Sint8>& out,
        const CIMConstClass& cimclass);

    static void printClassElement(
        const CIMConstClass& cimclass,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendInstanceElement(
        Array<Sint8>& out,
        const CIMConstInstance& instance);

    static void appendPropertyElement(
        Array<Sint8>& out,
        const CIMConstProperty& property);

    static void appendMethodElement(
        Array<Sint8>& out,
        const CIMConstMethod& method);

    static void appendParameterElement(
        Array<Sint8>& out,
        const CIMConstParameter& parameter);

    static void appendQualifierElement(
        Array<Sint8>& out,
        const CIMConstQualifier& qualifier);

    static void appendQualifierDeclElement(
        Array<Sint8>& out,
        const CIMConstQualifierDecl& qualifierDecl);

    static String getQualifierFlavor(Uint32 flavor);

    static String getQualifierScope(Uint32 scope);

private:

    MofWriter() { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MofWriter_h */
