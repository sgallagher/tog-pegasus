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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//
// class to encapsulate a qualifier list construct in the MOF grammer
// Since qualifier lists can appear before a lot of different metaelements, 
// we have to collect them before we know where they go; then apply
// them to the metaelement one at a time.
// NOTE: KS. Sept 2003. Most of this could be replaced by CIMQualifierList.
// The one function that is unique is the applyQualifierList template.

#ifndef _QUALIFIERLIST_H_
#define _QUALIFIERLIST_H_

#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Compiler/Linkage.h>
#include "memobjs.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

typedef Array<CIMQualifier *> qplist;

/**	Class to create and add to a qualifierList
*/
class PEGASUS_COMPILER_LINKAGE qualifierList {
 public:
  qplist *_pv;
  unsigned int _initsize;
 public:
	 /** constructor - creates instance of list with size
	     @param - optional parameter to set size
	 */
	 qualifierList(unsigned int vsize = 10) : _pv(0), _initsize(vsize) 
	{init(vsize);}
	
	~qualifierList();
	/** init function sets list size
	    @param size to set. Default is zero
	*/
	void init(int size = 0);
	/** add a qualifier to the list
		@param - pointer to CIMQualifer object to add to list.
	*/
	void add(CIMQualifier *q);
};

/** applyQualifierList applies the qualifier list in the
    first parameter to the object defined in the second
    parameter. Applies each qualifier in the list to the
    object with an addQualifier function.
    @param - pointer to qualifierList object that containes
    the qualifier list to be applied
    @param - pointer to object to which it is to be applied.
  	This is a template.
    The efficacy of this template depends on each metaelement
    (class, instance, method, etc., supporting an addQualifier()
    method.
*/
template <class T> 
void applyQualifierList(qualifierList* that, T *c)
{
  if (that->_pv) {
    for (Uint32 i = 0;
         i < that->_pv->size();  // && (*that->_pv)[i] ?
         i++) {
      CIMQualifier** qpp = &((*that->_pv)[i]);
      c->addQualifier( **qpp );
      delete *qpp;
      *qpp = 0;
    }
    that->init(that->_initsize);
  }
}

#endif
