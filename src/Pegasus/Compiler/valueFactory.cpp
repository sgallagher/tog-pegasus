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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//
// implementation of valueFactory 
//


#include "valueFactory.h"
#include "objname.h"
#include "cimmofParser.h"  /* unfortunately.  Now that valueFactory needs
                              to know about cimmofParser, it might as well
			      be rolled into it. */
#include <cstring>
#include <Pegasus/Common/String.h>

#define min(a,b) ( a < b ? a : b )
#define max(a,b) ( a > b ? a : b )

unsigned long
valueFactory::Stoi(const String &val) {
  unsigned int end = val.getLength();
  String s;
  for (unsigned int i = 0; i < end; i++) {
    switch(val[i]) {
    case '1': s += "1"; break;
    case '2': s += "2"; break;
    case '3': s += "3"; break;
    case '4': s += "4"; break;
    case '5': s += "5"; break;
    case '6': s += "6"; break;
    case '7': s += "7"; break;
    case '8': s += "8"; break;
    case '9': s += "9"; break;
    case '0': s += "0"; break;
    }
  }
  return atol(_CString(s));
}

static double
Stof(const String &val) {
  unsigned int end = val.getLength();
  String s;
  for (unsigned int i = 0; i < end; i++) {
    switch(val[i]) {
    case '1': s += "1"; break;
    case '2': s += "2"; break;
    case '3': s += "3"; break;
    case '4': s += "4"; break;
    case '5': s += "5"; break;
    case '6': s += "6"; break;
    case '7': s += "7"; break;
    case '8': s += "8"; break;
    case '9': s += "9"; break;
    case '0': s += "0"; break;
    case '.': s += "."; break;
    case 'E':
    case 'e': s += "E"; break;
    }
  }
  return atof(_CString(s));
}

static CIMDateTime &
StoDT(const String &val, CIMDateTime &dt) {
  unsigned int end = val.getLength();
  String s;
  for (unsigned int i = 0; i < end; i++) {
    switch(val[i]) {
    case '1': s += "1"; break;
    case '2': s += "2"; break;
    case '3': s += "3"; break;
    case '4': s += "4"; break;
    case '5': s += "5"; break;
    case '6': s += "6"; break;
    case '7': s += "7"; break;
    case '8': s += "8"; break;
    case '9': s += "9"; break;
    case '0': s += "0"; break;
    case '.': s += "."; break;
    case '+': s += "+"; break;
    case '-': s += "-"; break;
    }
  }
  if (s != "") {
    dt.set(_CString(s));
  }
  return dt;
}

//-------------------------------------------------------------------------
// This is a parser for a comma-separated value String.  It returns one
// value per call.  It handles quoted String and depends on the caller to
// tell it where the end of the String is.
//-------------------------------------------------------------------------
static Uint32
nextcsv(const String &csv, int sep, const Uint32 start,
        const Uint32 end, String &value)
{
  enum parsestate {INDQUOTE, INSQUOTE, NOTINQUOTE};
  value = "";
  Uint32 maxend = min(csv.getLength(), end);
  Uint32 idx = start;
  parsestate state = NOTINQUOTE;
  while (idx < maxend) {
    char idxchar = csv[idx];
    switch (state) {
    case NOTINQUOTE:
      switch (idxchar) {
      case '\\':
	state = INSQUOTE;
	break;
      case '"':
	state = INDQUOTE;
	break;
      default:
        if (idxchar == sep)
           return idx + 1;
        else
	  value += idxchar;
	break;
      }
      break;
    case INSQUOTE:
      value += idxchar;
      state = NOTINQUOTE;
      break;
    case INDQUOTE:
      switch (idxchar) {
      case '"':
	state = NOTINQUOTE;
	break;
      default:
	value += idxchar;
	break;
      }
    }
    idx++;
  }   // end while
  return idx;
}

//-------------------------------------------------------------------
// This builds a reference value from a String via the objname class
//-------------------------------------------------------------------
static
CIMValue *
build_reference_value(const String &rep)
{
  objectName oname(rep);
  CIMReference *ref = cimmofParser::Instance()->newReference(oname);
  return new CIMValue(*ref);
}

// ------------------------------------------------------------------
// When the value to be build is of Array type, this routine
// parses out the comma-separated values and builds the array
// ----------------------------------------------------------------- 
static
CIMValue *
build_array_value(CIMType::Tag type, unsigned int arrayDimension,
		  const String &rep)
{
  String sval;
  Uint32 start = 0;
  Uint32 strsize = rep.getLength();
  Uint32 end = strsize - 1;
 
  switch (type) {
  case CIMType::BOOLEAN: {
    Array<Boolean> *a = new Array<Boolean>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      if (sval[0] == 'T')
        a->append(1);
      else
        a->append(0);
    }
    return new CIMValue(*a);
  }
  case CIMType::UINT8: {
    Array<Uint8> *a = new Array<Uint8>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Uint8)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::SINT8: {
    Array<Sint8> *a = new Array<Sint8>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Sint8)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::UINT16: {
    Array<Uint16> *a = new Array<Uint16>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Uint16)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::SINT16: {
    Array<Sint16> *a = new Array<Sint16>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Sint16)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::UINT32: {
    Array<Uint32> *a = new Array<Uint32>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Uint32)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::SINT32: {
    Array<Sint32> *a = new Array<Sint32>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Sint32)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::UINT64: {
    Array<Uint64> *a = new Array<Uint64>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Uint64)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::SINT64: {
    Array<Sint64> *a = new Array<Sint64>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Sint64)valueFactory::Stoi(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::REAL32: {
    Array<Real32> *a = new Array<Real32>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Real32)Stof(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::REAL64: {
    Array<Real64> *a = new Array<Real64>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append((Real64)Stof(sval));
    }
    return new CIMValue(*a);
  }
  case CIMType::CHAR16: {
    Array<Char16> *a = new Array<Char16>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append(sval[0]);
    }
    return new CIMValue(*a);
  }
  case CIMType::STRING: {
    Array<String> *a = new Array<String>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      a->append(sval);
    }
    return new CIMValue(*a);
  }
  case CIMType::DATETIME: {
    Array<CIMDateTime> *a = new Array<CIMDateTime>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      CIMDateTime dt;
      StoDT(sval, dt);
      a->append(dt);
    }
    return new CIMValue(*a);
  }
  case CIMType::REFERENCE:
  case CIMType::NONE:
    break;
  }  // end switch
  return 0;
}


CIMValue *
valueFactory::createValue(CIMType::Tag type, int arrayDimension,
			  const String *repp)
{
  const String &rep = *repp;
  CIMDateTime dt;
  if (arrayDimension == -1) { // this is not an array type
    switch(type) {
    case CIMType::UINT8:    return new CIMValue((Uint8)  valueFactory::Stoi(rep));
    case CIMType::SINT8:    return new CIMValue((Sint8)  valueFactory::Stoi(rep));
    case CIMType::UINT16:   return new CIMValue((Uint16) valueFactory::Stoi(rep));
    case CIMType::SINT16:   return new CIMValue((Sint16) valueFactory::Stoi(rep));
    case CIMType::UINT32:   return new CIMValue((Uint32) valueFactory::Stoi(rep));
    case CIMType::SINT32:   return new CIMValue((Sint32) valueFactory::Stoi(rep));
    case CIMType::UINT64:   return new CIMValue((Uint64) valueFactory::Stoi(rep));
    case CIMType::SINT64:   return new CIMValue((Sint64) valueFactory::Stoi(rep));
    case CIMType::REAL32:   return new CIMValue((Real32) Stof(rep));
    case CIMType::REAL64:   return new CIMValue((Real64) Stof(rep));
    case CIMType::CHAR16:   return new CIMValue((Char16) rep[0]);
    case CIMType::BOOLEAN:  return new CIMValue((Boolean) (rep[0] == 'T'?1:0));
    case CIMType::STRING:   return new CIMValue(rep);
    case CIMType::DATETIME: return new CIMValue(StoDT(rep, dt));
    case CIMType::REFERENCE: return build_reference_value(rep);
    case CIMType::NONE: return(new CIMValue((Uint32) 0));
    }
    return(new CIMValue((Uint32) 0));    // default
  } else { // an array type, either fixed or variable
    return build_array_value(type, (unsigned int)arrayDimension, rep);
  }
}
