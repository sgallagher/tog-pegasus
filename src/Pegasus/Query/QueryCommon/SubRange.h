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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SubRange_h
#define Pegasus_SubRange_h

#include <Pegasus/Query/QueryCommon/Linkage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Common/Array.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES                                                                                                                                       
PEGASUS_NAMESPACE_BEGIN

                                                                                                                                       
#define SUBRANGE_END_OF_ARRAY -1;  // signifies the array range ends with the last element in the array
#define SUBRANGE_NO_INDEX -2; // signifies the array range is empty                                                                                                                                      
class PEGASUS_QUERYCOMMON_LINKAGE SubRange{
/*
Exceptions:
        CQLInvalidArrayRangeException - for example: "invalid characters ![0-9*-..]"
                                                                                                                                       
*/
        public:
	  SubRange();
          SubRange(String range);
                                                                                                                                       
          Boolean operator==(const SubRange &rhs)const;
          Boolean operator!=(const SubRange &rhs)const;

	  String toString()const;
                                                                                                                                       
          Sint32 start;
          Sint32 end;
                                                                                                                                       
        private:
          void parse(String range);
          Boolean isNum(CString cstr);
};


/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T SubRange
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
*/
                                                                                                                                       
PEGASUS_NAMESPACE_END
//#endif 
#endif
#endif 

