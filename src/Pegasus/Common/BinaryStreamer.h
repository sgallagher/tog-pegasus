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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef BinaryStreamer_h
#define BinaryStreamer_h

#include "ObjectStreamer.h"
#include "CIMProperty.h"
#include "CIMQualifier.h"
#include "CIMMethod.h"
#include "CIMFlavor.h"
#include "CIMScope.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE BinaryStreamer : public ObjectStreamer {

public:

   BinaryStreamer() {}

   ~BinaryStreamer() {}

   void encode(Array<char>& out, const CIMClass& cls);
   void encode(Array<char>& out, const CIMInstance& inst);
   void encode(Array<char>& out, const CIMQualifierDecl& qual);

   void decode(const Array<char>& in, unsigned int pos, CIMClass& cls);
   void decode(const Array<char>& in, unsigned int pos, CIMInstance& inst);
   void decode(const Array<char>& in, unsigned int pos, CIMQualifierDecl& qual);

private:

   void append(Array<char>& out, const CIMObjectPath &op);
   void append(Array<char>& out, const CIMName &cn);
   void append(Array<char>& out, const CIMType &typ);
   void append(Array<char>& out, Uint16 ui);
   void append(Array<char>& out, Uint32 ui);
   void append(Array<char>& out, Boolean b);

   void toBin(Array<char>& out, const CIMClass &cls);
   void toBin(Array<char>& out, const CIMInstance &inst);
   void toBin(Array<char>& out, const CIMQualifierDecl &qual);

   void toBin(Array<char>& out, const CIMQualifier &qual);
   void toBin(Array<char>& out, const CIMMethod &prop);
   void toBin(Array<char>& out, const CIMProperty &prop);
   void toBin(Array<char>& out, const CIMParameter& prm);
   void toBin(Array<char>& out, const CIMValue& prm);
   void toBin(Array<char>& out, const CIMFlavor& flav);
   void toBin(Array<char>& out, const CIMScope& scp);


   CIMObjectPath extractObjectPath(const char *ar, Uint32 & pos);
   CIMName extractName(const char *ar, Uint32 & pos);
   Uint16 extractUint16(const char *ar, Uint32 & pos);
   Uint32 extractUint32(const char *ar, Uint32 & pos);
   CIMType extractType(const char* ar, Uint32 & pos);
   Boolean extractBoolean(const char *ar, Uint32 & pos);

   CIMClass extractClass(const Array<char>& in, Uint32 & pos, const String &path);
   CIMInstance extractInstance(const Array<char>& in, Uint32 & pos, const String &path);
   CIMQualifierDecl extractQualifierDecl(const Array<char>& in, Uint32 & pos, const String &path);

   CIMQualifier extractQualifier(const Array<char>& in, Uint32 & pos);
   CIMMethod extractMethod(const Array<char>& in, Uint32 & pos);
   CIMProperty extractProperty(const Array<char>& in, Uint32 & pos);
   CIMParameter extractParameter(const Array<char>& in, Uint32 & pos);
   CIMValue extractValue(const Array<char>& in, Uint32 & pos);
   CIMFlavor extractFlavor(const Array<char>& in, Uint32 & pos);
   CIMScope extractScope(const Array<char>& in, Uint32 & pos);

};

#define PREAMBLE_CTL \
   (((('a' & 0xC0)+sizeof(struct record_preamble))<<8)+sizeof(struct record_preamble))
   
#define BINREP_MARKER (0x11)

// ascii  a: 0x61
// ebcdic a: 0x81

struct record_preamble {
   Uint8 _format;
   Uint8  _pVersion;
   Uint16 _pLenAndCtl;
   Uint8  _type;
   Uint8  _tVersion;
   Uint8 size()           { return (_pLenAndCtl & 0x3F); }
   Uint8 type()           { return _type; }
   Uint8 version()        { return _tVersion; }
   Uint8 format()         { return _format; }
   Boolean littleEndian() { return (_pLenAndCtl & 0xC000)!=0; }
   Boolean asciiChars()   { return ((_pLenAndCtl>>8)^(_pLenAndCtl&0xFF))==(0x61 & 0xC0); }
   Boolean endogenous()   { return _pLenAndCtl==PREAMBLE_CTL; }
};


#define BINREP_RECORD_IN(n,i,p) \
   record_preamble *n=(struct record_preamble*)(i.getData()+p)

#define BINREP_CLASS 1
#define BINREP_CLASS_V1 1
#define BINREP_CLASS_PREAMBLE_V1(n) \
   struct record_preamble n={BINREP_MARKER,1,PREAMBLE_CTL,\
   BINREP_CLASS,BINREP_CLASS_V1};


#define BINREP_INSTANCE 3
#define BINREP_INSTANCE_V1 1
#define BINREP_INSTANCE_PREAMBLE_V1(n) \
   struct record_preamble n={BINREP_MARKER,1,PREAMBLE_CTL,\
   BINREP_INSTANCE,BINREP_INSTANCE_V1};


#define BINREP_QUALIFIERDECL 4
#define BINREP_QUALIFIERDECL_V1 1
#define BINREP_QUALIFIERDECL_PREAMBLE_V1(n) \
   struct record_preamble n={BINREP_MARKER,1,PREAMBLE_CTL,\
   BINREP_QUALIFIERDECL,BINREP_QUALIFIERDECL_V1};



struct subtype_preamble {
   Uint8  _pLength;
   Uint8  _type;
   Uint8  _tVersion;
   Uint8 size() { return _pLength; }
   Uint8 type() { return _type; }
   Uint8 version() { return _tVersion; }
};

#define BINREP_SUBTYPE_IN(n,i,p) \
   subtype_preamble *n=(struct subtype_preamble*)(i.getData()+p)

#define BINREP_QUALIFIER 10
#define BINREP_QUALIFIER_V1 1
#define BINREP_QUALIFIER_PREAMBLE_V1(n) \
   struct subtype_preamble n={sizeof(struct subtype_preamble), \
   BINREP_QUALIFIER,BINREP_QUALIFIER_V1};


#define BINREP_PROPERTY 11
#define BINREP_PROPERTY_V1 1
#define BINREP_PROPERTY_PREAMBLE_V1(n) \
   struct subtype_preamble n={sizeof(struct subtype_preamble), \
   BINREP_PROPERTY,BINREP_PROPERTY_V1};


#define BINREP_METHOD 12
#define BINREP_METHOD_V1 1
#define BINREP_METHOD_PREAMBLE_V1(n) \
   struct subtype_preamble n={sizeof(struct subtype_preamble), \
   BINREP_METHOD,BINREP_METHOD_V1};


#define BINREP_PARAMETER 13
#define BINREP_PARAMETER_V1 1
#define BINREP_PARAMETER_PREAMBLE_V1(n) \
   struct subtype_preamble n={sizeof(struct subtype_preamble), \
   BINREP_PARAMETER,BINREP_PARAMETER_V1};


#define BINREP_VALUE 14
#define BINREP_VALUE_V1 1
#define BINREP_VALUE_PREAMBLE_V1(n) \
   struct subtype_preamble n={sizeof(struct subtype_preamble), \
   BINREP_VALUE,BINREP_VALUE_V1};


class BinException {
  public:
    BinException(int t, String m) {
       type=t;
       message=m;
    }
   int type;
   String message;
};

PEGASUS_NAMESPACE_END

#endif
