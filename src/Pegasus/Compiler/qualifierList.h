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
// Header for a class to generate CIMValue objects from String values
//
//
// class to encapsulate a qualifier list construct in the MOF grammer
// Since qualifier lists can appear before a lot of different metaelements, 
// we have to collect them before we know where they go; then apply
// them to the metaelement one at a time.
//

#ifndef _QUALIFIERLIST_H_
#define _QUALIFIERLIST_H_

#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/Array.h>
#include "memobjs.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

typedef Array<CIMQualifier *> qplist;

class PEGASUS_COMPILER_LINKAGE qualifierList {
 public:
  qplist *_pv;
  unsigned int _initsize;
 public:
  qualifierList(unsigned int vsize = 10) : _pv(0), _initsize(vsize) 
   {init(vsize);}
  
  ~qualifierList();
  
  void init(int size = 0);

  void add(CIMQualifier *q);
};

// The efficacy of this template depends on each metaelement
// (class, instance, method, etc., supporting an addQualifier()
// method.  So far, they all do.
template <class T> 
void apply(qualifierList* that, T *c)
{
  if (that->_pv) {
    for (qplist::iterator i = that->_pv->begin(); 
		    i != that->_pv->end() && *i; i++) {
      c->addQualifier( **i );
      delete *i;
      *i = 0;
    }
    that->init(that->_initsize);
  }
}

#endif
