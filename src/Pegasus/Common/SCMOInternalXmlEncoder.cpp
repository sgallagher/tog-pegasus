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

#include "SCMOInternalXmlEncoder.h"
#include <Pegasus/Common/SCMOXmlWriter.h>

PEGASUS_NAMESPACE_BEGIN

void SCMOInternalXmlEncoder::_putXMLInstance(
    CIMBuffer& out,
    const SCMOInstance& ci)
{

    if (ci.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.
        SCMOXmlWriter::appendInstanceElement(buf, ci);
        buf.append('\0');

        out.putUint32(buf.size());
        out.putBytes(buf.getData(), buf.size());
        buf.clear();

        if (0 == ci.getClassName())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            // Serialize object path as XML.
            SCMOXmlWriter::appendValueReferenceElement(buf, ci, true);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());

            // Write hostname and namespace in UTF-16 format
            Uint64 len=0;
            const char* hn = ci.getHostName_l(len);
            out.putUTF8AsString(hn, len);
            const char * ns = ci.getNameSpace_l(len);
            out.putUTF8AsString(ns, len);
        }
    }
}

void SCMOInternalXmlEncoder::_putXMLNamedInstance(
    CIMBuffer& out,
    const SCMOInstance& ci)
{
    if (ci.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.

        SCMOXmlWriter::appendInstanceElement(buf, ci);
        buf.append('\0');

        out.putUint32(buf.size());
        out.putBytes(buf.getData(), buf.size());
        buf.clear();

        if (0 == ci.getClassName())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            // Serialize object path as XML.
            SCMOXmlWriter::appendInstanceNameElement(buf, ci);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());

            // Write hostname and namespace in UTF-16 format
            Uint64 len=0;
            const char* hn = ci.getHostName_l(len);
            out.putUTF8AsString(hn, len);
            const char * ns = ci.getNameSpace_l(len);
            out.putUTF8AsString(ns, len);
        }
    }
}

void SCMOInternalXmlEncoder::_putXMLInstanceName(
    CIMBuffer& out,
    const SCMOInstance& cop)
{
    if (0 == cop.getClassName())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize object path as XML.
        SCMOXmlWriter::appendValueReferenceElement(buf, cop, true);
        buf.append('\0');

        out.putUint32(buf.size());
        out.putBytes(buf.getData(), buf.size());

        // Write hostname and namespace in UTF-16 format
        Uint64 len=0;
        const char* hn = cop.getHostName_l(len);
        out.putUTF8AsString(hn, len);
        const char * ns = cop.getNameSpace_l(len);
        out.putUTF8AsString(ns, len);
    }
}

void SCMOInternalXmlEncoder::_putXMLObject(
    CIMBuffer& out,
    const SCMOInstance& co)
{
    if (co.isUninitialized())
    {
        out.putUint32(0);
        out.putUint32(0);
        out.putString(String());
        out.putNamespaceName(CIMNamespaceName());
    }
    else
    {
        Buffer buf(4096);

        // Serialize instance as XML.

        SCMOXmlWriter::appendObjectElement(buf, co);
        buf.append('\0');

        out.putUint32(buf.size());
        out.putBytes(buf.getData(), buf.size());
        buf.clear();

        if (0 == co.getClassName())
        {
            out.putUint32(0);
            out.putString(String());
            out.putNamespaceName(CIMNamespaceName());
        }
        else
        {
            // Serialize object path as XML.
            SCMOXmlWriter::appendValueReferenceElement(buf, co, true);
            buf.append('\0');

            out.putUint32(buf.size());
            out.putBytes(buf.getData(), buf.size());

            // Write hostname and namespace in UTF-16 format
            Uint64 len=0;
            const char* hn = co.getHostName_l(len);
            out.putUTF8AsString(hn, len);
            const char * ns = co.getNameSpace_l(len);
            out.putUTF8AsString(ns, len);
        }
    }
}

PEGASUS_NAMESPACE_END
