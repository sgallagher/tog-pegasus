//%/////////-*-c++-*-///////////////////////////////////////////////////////////
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProcessIndicationProvider_h
#define Pegasus_ProcessIndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/CIMIndicationConsumer.h>
#include <Pegasus/ProviderManager/OperationResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

class ProcessIndicationProvider :
    public CIMIndicationProvider
{
public:
    ProcessIndicationProvider (void) throw ();
    virtual ~ProcessIndicationProvider (void) throw ();

    // CIMProvider interface
    virtual void initialize (CIMOMHandle & cimom);
    virtual void terminate (void);

    // CIMIndicationProvider interface
    virtual void enableIndications (IndicationResponseHandler & handler);
    virtual void disableIndications (void);

    virtual void createSubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames,
	const CIMPropertyList & propertyList,
	const Uint16 repeatNotificationPolicy);

    virtual void modifySubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames,
	const CIMPropertyList & propertyList,
	const Uint16 repeatNotificationPolicy);

    virtual void deleteSubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames);

protected:
    CIMOMHandle *_cimom;
     IndicationResponseHandler *_response_handler;
 private:
    Thread _indication_thread;
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _monitor(void *);
      AtomicInt _indications_enabled;
      Mutex _enable_disable;
    
};


class ProcessIndicationConsumer : public CIMIndicationConsumer
{
   public:
      ProcessIndicationConsumer(void) throw() ;
      virtual ~ProcessIndicationConsumer(void) throw() ;

      // CIMBaseProvider interface
      virtual void initialize (CIMOMHandle & cimom);
      virtual void terminate (void);
      
      // CIMIndicationConsumer interface 

    virtual void handleIndication(
	const OperationContext & context,
	const CIMInstance & indication,
	IndicationResponseHandler & handler) ;

   protected:
      CIMOMHandle *_cimom;
   private:

};


PEGASUS_NAMESPACE_END

#endif

