//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:  Karl Schopmeyer (k.schopmeyer@opengroup.org)
//                  Correct Null processing and correct calls to nextcsv - Mar 4 2002
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


//
// implementation of valueFactory 
//

#include <Pegasus/Common/Config.h>
#include "valueFactory.h"
#include "objname.h"
#include "cimmofParser.h"  /* unfortunately.  Now that valueFactory needs
                              to know about cimmofParser, it might as well
			      be rolled into it. */
#include <cstring>
#include <cstdlib>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Destroyer.h>

#define local_min(a,b) ( a < b ? a : b )
#define local_max(a,b) ( a > b ? a : b )

long
valueFactory::Stoi(const String &val) {
#if 0
  unsigned int end = val.size();
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
#else
  long longValue;
  ArrayDestroyer<char> valCharStr(val.allocateCString());
  if (!sscanf(valCharStr.getPointer(), "%ld", &longValue))
  {
//   ATTN-DME-P3-20020602: How should this error condition be handled?
     return 0;
  }
  else return longValue;
#endif
}

static double
Stof(const String &val) {
  unsigned int end = val.size();
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
  unsigned int end = val.size();
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
// Returns value in value and return pointing to character after separator 
// string
//-------------------------------------------------------------------------
static Uint32
nextcsv(const String &csv, int sep, const Uint32 start,
        const Uint32 end, String &value)
{
  enum parsestate {INDQUOTE, INSQUOTE, NOTINQUOTE};
  value = "";
  Uint32 maxend = local_min(csv.size(), end);
  Uint32 idx = start;
  parsestate state = NOTINQUOTE;
  // Change KS 4 March 2002. Change from < to <=. Was dropping last char in string.
  // ATTN-RK-P3-071702: Added hack to check for null character because Strings
  // were sometimes getting created that included an extra null character.
  while (idx <= maxend && csv[idx]) {
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
  CIMObjectPath *ref = cimmofParser::Instance()->newReference(oname);
  CIMValue* v = new CIMValue(*ref);
  delete ref;
  return v;
}

// ------------------------------------------------------------------
// When the value to be build is of Array type, this routine
// parses out the comma-separated values and builds the array
// ----------------------------------------------------------------- 
static
CIMValue *
build_array_value(CIMType type, unsigned int arrayDimension,
		  const String &rep)
{
  String sval;
  Uint32 start = 0;
  Uint32 strsize = rep.size();
  Uint32 end = strsize;

  /* KS Changed all of the following from whil {...} to do {...} while (start < end);
  The combination of the testing and nexcsv meant the last entry was not processed.
  */
  switch (type) {
  case CIMTYPE_BOOLEAN: {
    Array<Boolean> *a = new Array<Boolean>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            if (sval[0] == 'T')
                a->append(1);
            else
                a->append(0);
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_UINT8: {
    Array<Uint8> *a = new Array<Uint8>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Uint8)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_SINT8: {
    Array<Sint8> *a = new Array<Sint8>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Sint8)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_UINT16: {
    Array<Uint16> *a = new Array<Uint16>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Uint16)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_SINT16: {
    Array<Sint16> *a = new Array<Sint16>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Sint16)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_UINT32: {
    Array<Uint32> *a = new Array<Uint32>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Uint32)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_SINT32: {
    Array<Sint32> *a = new Array<Sint32>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Sint32)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_UINT64: {
    Array<Uint64> *a = new Array<Uint64>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Uint64)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_SINT64: {
    Array<Sint64> *a = new Array<Sint64>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Sint64)valueFactory::Stoi(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_REAL32: {
    Array<Real32> *a = new Array<Real32>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
        a->append((Real32)Stof(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_REAL64: {
    Array<Real64> *a = new Array<Real64>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append((Real64)Stof(sval));
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_CHAR16: {
    Array<Char16> *a = new Array<Char16>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append(sval[0]);
        } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_STRING: {
    Array<String> *a = new Array<String>;
    if (strsize != 0){
        do{
            start = nextcsv(rep, ',', start, end, sval);
            a->append(sval);
         } while (start < end);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_DATETIME: {
    Array<CIMDateTime> *a = new Array<CIMDateTime>;
    while (strsize && (start = nextcsv(rep, ',', start, end, sval)) < end ) {
      CIMDateTime dt;
      StoDT(sval, dt);
      a->append(dt);
    }
    return new CIMValue(*a);
  }
  case CIMTYPE_REFERENCE:
  case CIMTYPE_NONE:
    break;
  }  // end switch
  return 0;
}

/* ATTN: KS 20 Feb 02 - Think we need to account for NULL value here differently
   They come in as an empty string from devaultValue and if they are an empty 
   string we need to create the correct type but without a value in it.
   Easiest may be to test in each converter since otherwise would have to 
   create a second switch. Either that or if strlength = zero
   create an empty CIMValue and then put in the type
                CIMValue x;
            x.set(Uint16(9)
*/
CIMValue *
valueFactory::createValue(CIMType type, int arrayDimension,
                          Boolean isNULL,
			  const String *repp)
{
  const String &rep = *repp;
  CIMDateTime dt;
  if (arrayDimension == -1) { // this is not an array type
      
    if (isNULL)
    {
       return new CIMValue(type, false);
    }
    switch(type) {
    case CIMTYPE_UINT8:    return new CIMValue((Uint8)  valueFactory::Stoi(rep));
    case CIMTYPE_SINT8:    return new CIMValue((Sint8)  valueFactory::Stoi(rep));
    case CIMTYPE_UINT16:   return new CIMValue((Uint16) valueFactory::Stoi(rep));
    case CIMTYPE_SINT16:   return new CIMValue((Sint16) valueFactory::Stoi(rep));
    case CIMTYPE_UINT32:   return new CIMValue((Uint32) valueFactory::Stoi(rep));
    case CIMTYPE_SINT32:   return new CIMValue((Sint32) valueFactory::Stoi(rep));
    case CIMTYPE_UINT64:   return new CIMValue((Uint64) valueFactory::Stoi(rep));
    case CIMTYPE_SINT64:   return new CIMValue((Sint64) valueFactory::Stoi(rep));
    case CIMTYPE_REAL32:   return new CIMValue((Real32) Stof(rep));
    case CIMTYPE_REAL64:   return new CIMValue((Real64) Stof(rep));
    case CIMTYPE_CHAR16:   return new CIMValue((Char16) rep[0]);
    case CIMTYPE_BOOLEAN:  return new CIMValue((Boolean) (rep[0] == 'T'?1:0));
    case CIMTYPE_STRING:   return new CIMValue(rep);
    case CIMTYPE_DATETIME: return new CIMValue(StoDT(rep, dt));
    case CIMTYPE_REFERENCE: return build_reference_value(rep);
    case CIMTYPE_NONE:     return(new CIMValue((Uint32) 0));
    }
    return(new CIMValue((Uint32) 0));    // default
  } else { // an array type, either fixed or variable

  const String &rep = *repp;
      // KS If empty string set CIMValue type but Null attribute.
      if (isNULL)
          return new CIMValue(type, true, arrayDimension);

    return build_array_value(type, (unsigned int)arrayDimension, rep);
  }
}
