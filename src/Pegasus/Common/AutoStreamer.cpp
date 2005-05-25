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


#include "ObjectStreamer.h"

#include "AutoStreamer.h"

PEGASUS_NAMESPACE_BEGIN

AutoStreamer::AutoStreamer(ObjectStreamer *primary, Uint8 marker) {
   _readerCount=0;
   if (marker) {
      _readers[_readerCount].reader=primary;
      _readers[_readerCount++].marker=marker;
   }
   else _defaultReader=primary;
   _primary=primary; 
}

AutoStreamer::~AutoStreamer() {
    for (Uint32 i=0; i<=_readerCount; ++i)
    {
	if (_defaultReader != _readers[i].reader)
        	delete _readers[i].reader;
    }
    if (_defaultReader)
    	delete _defaultReader;
}

void AutoStreamer::addReader(ObjectStreamer *reader, Uint8 marker) {
   if (marker) {
      _readers[_readerCount].reader=reader;
      _readers[_readerCount++].marker=marker;
   }
   else _defaultReader=reader;
}


void AutoStreamer::encode(Array<char>& out, const CIMClass& cls)
{
   _primary->encode(out,cls);
}

void AutoStreamer::encode(Array<char>& out, const CIMInstance& inst)
{
   _primary->encode(out,inst);
}

void AutoStreamer::encode(Array<char>& out, const CIMQualifierDecl& qual)
{
   _primary->encode(out,qual);
}

void AutoStreamer::write(PEGASUS_STD(ostream)& os, Array<char>& in)
{
   _primary->write(os,in);
}


void AutoStreamer::decode(const Array<char>& in, unsigned int pos, CIMClass& cls)
{
   for (Uint16 i=0,m=_readerCount; i<m; i++) {
      if (_readers[i].marker==in[pos]) {
         _readers[i].reader->decode(in,pos,cls);
         return;
      }
   }
   _defaultReader->decode(in,pos,cls);
}

void AutoStreamer::decode(const Array<char>& in, unsigned int pos, CIMInstance& inst)
{
   for (Uint16 i=0,m=_readerCount; i<m; i++) {
      if (_readers[i].marker==in[pos]) {
         _readers[i].reader->decode(in,pos,inst);
         return;
      }
   }
   _defaultReader->decode(in,pos,inst);
}

void AutoStreamer::decode(const Array<char>& in, unsigned int pos, CIMQualifierDecl& qual)
{
   for (Uint16 i=0,m=_readerCount; i<m; i++) {
      if (_readers[i].marker==in[pos]) {
         _readers[i].reader->decode(in,pos,qual);
         return;
      }
   }
   _defaultReader->decode(in,pos,qual);
}

PEGASUS_NAMESPACE_END

