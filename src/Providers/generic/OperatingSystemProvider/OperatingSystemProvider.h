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

#ifndef Pegasus_OperatingSystemProvider_h
#define Pegasus_OperatingSystemProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider2/CIMInstanceProvider.h>

PEGASUS_NAMESPACE_BEGIN

class OperatingSystemProvider: public CIMInstanceProvider {
public:
   OperatingSystemProvider(void);
   virtual ~OperatingSystemProvider(void);

   // CIMBaseProvider interface
   virtual void initialize(CIMOMHandle& cimomHandle);
   virtual void terminate(void);

   // CIMInstanceProvider interface
   virtual CIMInstance getInstance(const OperationContext & context, const CIMReference & ref, const Uint32 flags = 0, const Array<String> & propertyList = EmptyStringArray());
   virtual Array<CIMInstance> enumerateInstances(const OperationContext & context, const CIMReference & ref, const Uint32 flags = 0, const Array<String> & propertyList = EmptyStringArray());
   virtual Array<CIMReference> enumerateInstanceNames(const OperationContext & context, const CIMReference & ref);

protected:
   CIMRepository *      _pRepository;

};

PEGASUS_NAMESPACE_END

#endif
