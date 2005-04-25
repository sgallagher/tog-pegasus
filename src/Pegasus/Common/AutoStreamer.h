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
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG#3348
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef AutoStreamer_h
#define AutoStreamer_h

#include "ObjectStreamer.h"

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE AutoStreamer : public ObjectStreamer {

   ObjectStreamer *_primary,*_defaultReader;
   Uint16 _readerCount;
   struct _reader {
      _reader() { reader=NULL, marker=0; }
      ObjectStreamer *reader;
      Uint8 marker;
   } _readers[8];

public:

   AutoStreamer(ObjectStreamer *primary, Uint8 marker);

   ~AutoStreamer();

   void addReader(ObjectStreamer *primary, Uint8 marker);

   void encode(Array<char>& out, const CIMClass& cls);
   void encode(Array<char>& out, const CIMInstance& inst);
   void encode(Array<char>& out, const CIMQualifierDecl& qual);

   void decode(const Array<char>& in, unsigned int pos, CIMClass& cls);
   void decode(const Array<char>& in, unsigned int pos, CIMInstance& inst);
   void decode(const Array<char>& in, unsigned int pos, CIMQualifierDecl& qual);

   void write(PEGASUS_STD(ostream)& os, Array<char>& in);
};


PEGASUS_NAMESPACE_END

#endif
