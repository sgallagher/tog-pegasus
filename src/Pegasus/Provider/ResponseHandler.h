//%/////-*-c++-*-//////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ResponseHandler_h
#define Pegasus_ResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/OperationContext.h>

PEGASUS_NAMESPACE_BEGIN

template<class object_type>
class PEGASUS_COMMON_LINKAGE ResponseHandler
{
public:
      /** ATTN:
      */
      ResponseHandler(void) {};

      /** ATTN:
      */
      virtual ~ResponseHandler(void) {};

      /** ATTN:
      */
      virtual void deliver(const object_type & object) = 0;

      /** ATTN:
      */
      virtual void deliver(const Array<object_type> & objects) = 0;

      /** ATTN:
      */
      virtual void reserve(const Uint32 size) = 0;

      /** ATTN:
      */
      virtual void processing(void) = 0;

      virtual void processing(OperationContext *context) = 0;

      /** ATTN:
      */
      virtual void complete(void) = 0;

      virtual void complete(OperationContext *context) = 0;
};

PEGASUS_NAMESPACE_END

#endif
