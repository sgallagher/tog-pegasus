//%/////////////////////////////////////////////////////////////////////////////
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

#ifndef Pegasus_HelloWorldProvider_h
#define Pegasus_HelloWorldProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>

PEGASUS_NAMESPACE_BEGIN

class HelloWorldProvider : public CIMInstanceProvider {
public:
   HelloWorldProvider(void);
   virtual ~HelloWorldProvider(void);

   // CIMBaseProvider interface
   virtual void initialize(CIMOMHandle & cimom);
   virtual void terminate(void);

   // CIMInstanceProvider interface
   virtual void getInstance(
      const OperationContext & context,
      const CIMReference & ref,
      const Uint32 flags,
      const Array<String> & propertyList,
      ResponseHandler<CIMInstance> & handler);

   virtual void enumerateInstances(
      const OperationContext & context,
      const CIMReference & ref,
      const Uint32 flags,
      const Array<String> & propertyList,
      ResponseHandler<CIMInstance> & handler);

   virtual void enumerateInstanceNames(
      const OperationContext & context,
      const CIMReference & ref,
      ResponseHandler<CIMReference> & handler);

   virtual void modifyInstance(
      const OperationContext & context,
      const CIMReference & ref,
      const CIMInstance & obj,
      ResponseHandler<CIMInstance> & handler);

   virtual void createInstance(
      const OperationContext & context,
      const CIMReference & ref,
      const CIMInstance & obj,
      ResponseHandler<CIMInstance> & handler);

   virtual void deleteInstance(
      const OperationContext & context,
      const CIMReference & ref,
      ResponseHandler<CIMInstance> & handler);

protected:
   CIMOMHandle          _cimom;
   Array<CIMInstance>   _instances;

};

PEGASUS_NAMESPACE_END

#endif
