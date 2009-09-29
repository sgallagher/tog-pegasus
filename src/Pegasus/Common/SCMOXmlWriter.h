//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SCMOXmlWriter_h
#define Pegasus_SCMOXmlWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlGenerator.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/SCMO.h>
#include <Pegasus/Common/SCMOInstance.h>
#include <Pegasus/Common/CIMDateTimeInline.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE SCMOXmlWriter : public XmlWriter
{
public:

    static void appendValueSCMOInstanceElement(
        Buffer& out,
        const SCMOInstance& scmoInstance);

    static void appendInstanceNameElement(
        Buffer& out,
        const SCMOInstance& scmoInstance);
    
    static void appendInstanceElement(
        Buffer& out,
        const SCMOInstance& scmoInstance);
    
    static void appendQualifierElement(
        Buffer& out,
        const SCMBQualifier& theQualifier,
        const char* base);

    static void appendPropertyElement(
        Buffer& out,
        const SCMOInstance& scmoInstance,
        Uint32 pos);

    static void appendValueElement(
        Buffer& out,
        SCMBValue & value,
        const char * base);

    static void appendSCMBUnion(
        Buffer& out,
        const SCMBUnion & u,
        const CIMType & valueType,
        const char * base);

    static void appendSCMBUnionArray(
        Buffer& out,
        const SCMBUnion & u,
        const CIMType & valueType,
        Uint32 numElements,
        const char * base);

private:

    SCMOXmlWriter();
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SCMOXmlWriter_h */
