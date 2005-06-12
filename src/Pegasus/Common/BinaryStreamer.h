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
//     David Dillard, VERITAS Software Corp.  (david.dillard@veritas.com)
//     Mike Brasher (mike-brasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef BinaryStreamer_h
#define BinaryStreamer_h

#include <Pegasus/Common/ObjectStreamer.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMFlavor.h>
#include <Pegasus/Common/CIMScope.h>

#define BINREP_MARKER 0x11

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE BinaryStreamer : public ObjectStreamer 
{
public:

    BinaryStreamer() { }

    ~BinaryStreamer() { }

    void encode(Array<char>& out, const CIMClass& x);

    void encode(Array<char>& out, const CIMInstance& x);

    void encode(Array<char>& out, const CIMQualifierDecl& x);

    void decode(const Array<char>& in, unsigned int pos, CIMClass& x);

    void decode(const Array<char>& in, unsigned int pos, CIMInstance& x);

    void decode(const Array<char>& in, unsigned int pos, CIMQualifierDecl& x);

public:

    static void _unpackQualifier(
	const Array<char>& in, Uint32& pos, CIMQualifier& x);

    static void _packQualifier(Array<char>& out, const CIMQualifier& x);

    static void _unpackProperty(
	const Array<char>& in, Uint32& pos, CIMProperty& x);

    static void _unpackMethod(
	const Array<char>& in, Uint32& pos, CIMMethod& x);

private:

    static void _packName(Array<char>& out, const CIMName& x);

    static void _unpackName(const Array<char>& in, Uint32& pos, CIMName& x);


    static void _packValue(Array<char>& out, const CIMValue& x);

    static void _unpackValue(const Array<char>& in, Uint32& pos, CIMValue& x);

    static void _packProperty(Array<char>& out, const CIMProperty& x);

    static void _packParameter(Array<char>& out, const CIMParameter& x);

    static void _unpackParameter(
	const Array<char>& in, Uint32& pos, CIMParameter& x);

    static void _packParameters(Array<char>& out, CIMMethodRep* rep);

    static void _unpackParameters(
	const Array<char>& in, Uint32& pos, CIMMethod& x);

    static void _packMethod(Array<char>& out, const CIMMethod& x);

    static void _packObjectPath(Array<char>& out, const CIMObjectPath& x);

    static void _unpackObjectPath(
	const Array<char>& in, Uint32& pos, CIMObjectPath& x);

    static void _packProperties(Array<char>& out, CIMObjectRep* rep);

    static void _packMethods(Array<char>& out, CIMClassRep* rep);

    static void _packScope(Array<char>& out, const CIMScope& x);

    static void _unpackScope(
	const Array<char>& in, Uint32& pos, CIMScope& x);

    static void _packFlavor(Array<char>& out, const CIMFlavor& x);

    static void _unpackFlavor(
	const Array<char>& in, Uint32& pos, CIMFlavor& x);

    static void _packType(Array<char>& out, const CIMType& x);

    static void _unpackType(
	const Array<char>& in, Uint32& pos, CIMType& x);
};

class BinException : public Exception
{
public:
    BinException(const String& message) : Exception(message) { }
};

PEGASUS_NAMESPACE_END

#endif
