//BEGIN_LICENSE
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
//END_LICENSE

#include <Pegasus/Common/Config.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>
#include <Pegasus/Provider2/CIMProviderStub.h>

#include "HelloWorldProvider.h"

PEGASUS_NAMESPACE_BEGIN

// provider module entry point
extern "C" PEGASUS_EXPORT CIMProvider* PegasusCreateProvider_HelloWorldProvider()
{
   return(new CIMProviderStub((CIMInstanceProvider *)new HelloWorldProvider()));
}

HelloWorldProvider::HelloWorldProvider(void) : _pRepository(0)
{
}

HelloWorldProvider::~HelloWorldProvider(void)
{
}

void HelloWorldProvider::initialize(CIMRepository & repository)
{
   // derefence repository pointer and save for later
   _pRepository = &repository;

   // create an instances
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

CIMInstance HelloWorldProvider::getInstance(const OperationContext & context, const CIMReference & ref, const Uint32 flags, const Array<String> & propertyList)
{
   // instance index corresponds to reference index
   Array<CIMReference> references = enumerateInstanceNames(context, ref);

   for(Uint32 i = 0; i < references.size(); ++i) {
      if(references[i] == ref) {
         return(_instances[i]);
      }
   }

   throw((CIMException)(CIMException::NOT_FOUND));
}

Array<CIMInstance> HelloWorldProvider::enumerateInstances(const OperationContext & context, const CIMReference & ref, const Uint32 flags, const Array<String> & propertyList)
{
   return(_instances);
}

Array<CIMReference> HelloWorldProvider::enumerateInstanceNames(const OperationContext & context, const CIMReference & ref)
{
   // check dependencies
   if(_pRepository == 0) {
      throw(UnitializedHandle());
   }

   // get class definition from repository
   CIMClass cimclass = _pRepository->getClass(ref.getNameSpace(), ref.getClassName());

   Array<CIMReference> references;

   // convert instances to references;
   for(Uint32 i = 0; i < _instances.size(); ++i) {
      CIMReference tempRef = _instances[i].getInstanceName(cimclass);

      // make references fully qualified
      tempRef.setHost(ref.getHost());
      tempRef.setNameSpace(ref.getNameSpace());

      // add instance reference to array
      references.append(tempRef);
   }

   return(references);
}

void HelloWorldProvider::modifyInstance(const OperationContext & context, const CIMReference & ref, const CIMInstance & obj)
{
   // instance index corresponds to reference index
   Array<CIMReference> references = enumerateInstanceNames(context, ref);

   for(Uint32 i = 0; i < references.size(); ++i) {
      if(ref == references[i]) {
         _instances.remove(i);
         _instances.insert(i, obj);

         return;
      }
   }

   throw((CIMException)(CIMException::NOT_FOUND));
}

void HelloWorldProvider::createInstance(const OperationContext & context, const CIMReference & ref, const CIMInstance & obj)
{
   // instance index corresponds to reference index
   Array<CIMReference> references = enumerateInstanceNames(context, ref);

   for(Uint32 i = 0; i < references.size(); ++i) {
      if(ref == references[i]) {
         throw((CIMException)(CIMException::ALREADY_EXISTS));
      }
   }

   _instances.append(obj);
}

void HelloWorldProvider::deleteInstance(const OperationContext & context, const CIMReference & ref)
{
   // instance index corresponds to reference index
   Array<CIMReference> references = enumerateInstanceNames(context, ref);

   for(Uint32 i = 0; i < references.size(); ++i) {
      if(ref == references[i]) {
         _instances.remove(i);

         return;
      }
   }

   throw((CIMException)(CIMException::NOT_FOUND));
}

PEGASUS_NAMESPACE_END
