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

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>
#include <Pegasus/Provider2/SimpleResponseHandler.h>
#include <Pegasus/Provider2/CIMProviderStub.h>

#include "HelloWorldProvider.h"

PEGASUS_NAMESPACE_BEGIN

static HelloWorldProvider provider;

extern "C" PEGASUS_EXPORT CIMProvider* PegasusCreateProvider_HelloWorldProvider()
{
   return(new CIMProviderStub((CIMInstanceProvider *)&provider));
}

HelloWorldProvider::HelloWorldProvider(void)
{
}

HelloWorldProvider::~HelloWorldProvider(void)
{
}

void HelloWorldProvider::initialize(CIMOMHandle& cimom)
{
   // save cimom handle
   _cimom = cimom;

   // create default instances
   CIMInstance instance1("HelloWorld");

   instance1.addProperty(CIMProperty("Identifier", Uint32(1)));   // key
   instance1.addProperty(CIMProperty("TimeStamp", CIMDateTime()));
   instance1.addProperty(CIMProperty("Message", String("Hello World")));

   _instances.append(instance1);

   CIMInstance instance2("HelloWorld");

   instance2.addProperty(CIMProperty("Identifier", Uint32(2)));   // key
   instance2.addProperty(CIMProperty("TimeStamp", CIMDateTime()));
   instance2.addProperty(CIMProperty("Message", String("Hey Planet")));

   _instances.append(instance2);

   CIMInstance instance3("HelloWorld");

   instance3.addProperty(CIMProperty("Identifier", Uint32(3)));   // key
   instance3.addProperty(CIMProperty("TimeStamp", CIMDateTime()));
   instance3.addProperty(CIMProperty("Message", String("Yo Earth")));

   _instances.append(instance3);
}

void HelloWorldProvider::terminate(void)
{
}

void HelloWorldProvider::getInstance(
   const OperationContext & context,
   const CIMReference & ref,
   const Uint32 flags,
   const Array<String> & propertyList,
   ResponseHandler<CIMInstance> & handler)
{
   handler.setStatus(PROCESSING);

   // get references
   Array<CIMReference> references;

   try {
      SimpleResponseHandler<CIMReference> handler;

      enumerateInstanceNames(context, ref, handler);

      references = handler._objects;
   }
   catch(...) {
   }

   handler.setStatus(PROCESSING);

   // instance index corresponds to reference index
   for(Uint32 i = 0; i < references.size(); i++) {
      if(references[i] == ref) {
         handler.putObject(_instances[i]);

         handler.setStatus(COMPLETE);

         return;
      }
   }

   throw CIMException(CIM_ERR_NOT_FOUND);
}

void HelloWorldProvider::enumerateInstances(
   const OperationContext & context,
   const CIMReference & ref,
   const Uint32 flags,
   const Array<String> & propertyList,
   ResponseHandler<CIMInstance> & handler)
{
   handler.setStatus(PROCESSING);

   for(Uint32 i = 0; i < _instances.size(); i++) {
      handler.putObject(_instances[i]);
   }

   handler.setStatus(COMPLETE);
}

void HelloWorldProvider::enumerateInstanceNames(
   const OperationContext & context,
   const CIMReference & ref,
   ResponseHandler<CIMReference> & handler)
{
   handler.setStatus(PROCESSING);

   // get class definition from repository
   CIMClass cimclass = _cimom.getClass(ref.getNameSpace(), ref.getClassName());

   // convert instances to references;
   for(Uint32 i = 0; i < _instances.size(); i++) {
      CIMReference tempRef = _instances[i].getInstanceName(cimclass);

      // make references fully qualified
      tempRef.setHost(ref.getHost());
      tempRef.setNameSpace(ref.getNameSpace());

      handler.putObject(tempRef);
   }

   handler.setStatus(COMPLETE);
}

void HelloWorldProvider::modifyInstance(
   const OperationContext & context,
   const CIMReference & ref,
   const CIMInstance & obj,
   ResponseHandler<CIMInstance> & handler)
{
   handler.setStatus(PROCESSING);

   // get references
   Array<CIMReference> references;

   try {
      SimpleResponseHandler<CIMReference> handler;

      enumerateInstanceNames(context, ref, handler);

      references = handler._objects;
   }
   catch(...) {
   }

   handler.setStatus(PROCESSING);

   for(Uint32 i = 0; i < references.size(); i++) {
      if(ref == references[i]) {
         _instances.remove(i);
         _instances.insert(i, obj);

         handler.setStatus(COMPLETE);

         return;
      }
   }

   throw CIMException(CIM_ERR_NOT_FOUND);
}

void HelloWorldProvider::createInstance(
   const OperationContext & context,
   const CIMReference & ref,
   const CIMInstance & obj,
   ResponseHandler<CIMInstance> & handler)
{
   handler.setStatus(PROCESSING);

   // get references
   Array<CIMReference> references;

   try {
      SimpleResponseHandler<CIMReference> handler;

      enumerateInstanceNames(context, ref, handler);

      references = handler._objects;
   }
   catch(...) {
   }

   handler.setStatus(PROCESSING);

   for(Uint32 i = 0; i < references.size(); i++) {
      if(ref == references[i]) {
         throw CIMException(CIM_ERR_ALREADY_EXISTS);
      }
   }

   _instances.append(obj);

   handler.setStatus(COMPLETE);

   return;
}

void HelloWorldProvider::deleteInstance(
   const OperationContext & context,
   const CIMReference & ref,
   ResponseHandler<CIMInstance> & handler)
{
   handler.setStatus(PROCESSING);

   // get references
   Array<CIMReference> references;

   try {
      SimpleResponseHandler<CIMReference> handler;

      enumerateInstanceNames(context, ref, handler);

      references = handler._objects;
   }
   catch(...) {
   }

   handler.setStatus(PROCESSING);

   for(Uint32 i = 0; i < references.size(); i++) {
      if(ref == references[i]) {
         _instances.remove(i);

         handler.setStatus(COMPLETE);

         return;
      }
   }

   throw CIMException(CIM_ERR_NOT_FOUND);
}

PEGASUS_NAMESPACE_END
