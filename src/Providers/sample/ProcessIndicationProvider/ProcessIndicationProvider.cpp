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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include "ProcessIndicationProvider.h"

PEGASUS_NAMESPACE_BEGIN

ProcessIndicationProvider::ProcessIndicationProvider (void) throw ()
   : _indication_thread(_monitor, this, false), 
     _indications_enabled(0)
{
}

ProcessIndicationProvider::~ProcessIndicationProvider (void) throw ()
{
}

void ProcessIndicationProvider::initialize (CIMOMHandle & cimom)
{
    // save cimom handle
    _cimom = &cimom;
}

void ProcessIndicationProvider::terminate (void)
{
   disableIndications();
   
   delete this;
   
}

void ProcessIndicationProvider::enableIndications (
    IndicationResponseHandler & handler)
{



   _enable_disable.lock(pegasus_thread_self());
   if(_indications_enabled.value())
   {
      _enable_disable.unlock();
      return;
   }
   _response_handler = &handler;
   
   _response_handler->processing ();   
   _indications_enabled = 1;
   _indication_thread.run();
   _enable_disable.unlock();

}

void ProcessIndicationProvider::disableIndications (void)
{
   _enable_disable.lock(pegasus_thread_self());
   
   if(_indications_enabled.value() == 0 )
   {
      _enable_disable.unlock();
      return;
   }

   _indications_enabled = 0;
   _response_handler = 0;
   _indication_thread.join();
   _enable_disable.unlock();
}

void ProcessIndicationProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
//cout << "create ProcessIndicationProvider" << endl;
}

void ProcessIndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void ProcessIndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
}



PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProcessIndicationProvider::_monitor(void *parm)
{
   PEGASUS_ASSERT(parm != 0);
   
   Thread *th = static_cast<Thread *>(parm);
   ProcessIndicationProvider *myself = static_cast<ProcessIndicationProvider *>(th->get_parm());

   if(true)
   {
      pegasus_sleep(1000);
      try
      {
	 myself->_enable_disable.try_lock(pegasus_thread_self());
      }
      catch(...)
      {
	 if(myself->_indications_enabled.value() == 0  || myself->_response_handler == 0 )
	    exit_thread((PEGASUS_THREAD_RETURN)0);
      }
      try
      {
	 
	 CIMInstance indicationInstance ("root/PG_Interop:CIM_ProcessIndication");
	 
	 indicationInstance.addProperty
	    (CIMProperty ("IndicationTime", CIMValue (CIMDateTime ())));
	 
	 indicationInstance.addProperty
	    (CIMProperty ("IndicationIdentifier", "ProcessIndication01"));
	 
	 Array <String> correlatedIndications;
	 indicationInstance.addProperty (CIMProperty ("CorrelatedIndications", 
						      CIMValue (correlatedIndications)));
	 
	 CIMIndication cimIndication (indicationInstance);
	 
	 myself->_response_handler->deliver (cimIndication);
	 
      }
      catch(...)
      {
	
      }
      myself->_enable_disable.unlock();
   }
   exit_thread((PEGASUS_THREAD_RETURN)0);
   return(PEGASUS_THREAD_RETURN)0;
}


ProcessIndicationConsumer::ProcessIndicationConsumer() throw()
{
   
}

ProcessIndicationConsumer::~ProcessIndicationConsumer() throw()
{

}

void ProcessIndicationConsumer::initialize (CIMOMHandle & cimom)
{
    // save cimom handle
    _cimom = &cimom;
}

void ProcessIndicationConsumer::terminate ()
{
   delete this;
}


void ProcessIndicationConsumer::handleIndication(const OperationContext & context, 
						const CIMInstance & indication,
						IndicationResponseHandler & handler)
{
   
   CIMInstance indication_copy = indication.clone();
   PEGASUS_STD(cout) << "Indication Consumer: recieved indication of class: " << indication_copy.getClassName() << PEGASUS_STD(endl);
    
}


PEGASUS_NAMESPACE_END
