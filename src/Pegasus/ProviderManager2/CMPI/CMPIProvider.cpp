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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPIProvider.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Ftabs.h"

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderModule.h>

PEGASUS_NAMESPACE_BEGIN


// set current operations to 1 to prevent an unload
// until the provider has had a chance to initialize
CMPIProvider::CMPIProvider(const String & name,
		   CMPIProviderModule *module,
		   ProviderVector *mv)
   : _module(module), _cimom_handle(0), _name(name),
     _no_unload(0), _rm(0)
{
   _current_operations = 1;
   miVector=*mv;
   noUnload=false;
}

CMPIProvider::CMPIProvider(CMPIProvider *pr)
  : _module(pr->_module), _cimom_handle(0), _name(pr->_name),
    _no_unload(0), _rm(0)
{
   _current_operations = 1;
   miVector=pr->miVector;
   _cimom_handle=new CIMOMHandle();
   noUnload=pr->noUnload;
}

CMPIProvider::~CMPIProvider(void)
{

}

CMPIProvider::Status CMPIProvider::getStatus(void) const
{
    return(_status);
}

CMPIProviderModule *CMPIProvider::getModule(void) const
{
    return(_module);
}

String CMPIProvider::getName(void) const
{
    return(_name);
}

void CMPIProvider::initialize(CIMOMHandle & cimom,
                              ProviderVector & miVector,
			      String & name,
                              CMPI_Broker & broker)
{
        broker.hdl=new CIMOMHandle(cimom);
        broker.bft=CMPI_Broker_Ftab;
        broker.eft=CMPI_BrokerEnc_Ftab;
        broker.clsCache=NULL;
        broker.name=name;

        const OperationContext opc;
        CMPI_ContextOnStack eCtx(opc);

        if (miVector.genericMode) {
           CString mName=name.getCString();
           if (miVector.miTypes & CMPI_MIType_Instance)
              miVector.instMI=miVector.createGenInstMI(&broker,&eCtx,mName);
           if (miVector.miTypes & CMPI_MIType_Association)
              miVector.assocMI=miVector.createGenAssocMI(&broker,&eCtx,mName);
           if (miVector.miTypes & CMPI_MIType_Method) 
              miVector.methMI=miVector.createGenMethMI(&broker,&eCtx,mName);
           if (miVector.miTypes & CMPI_MIType_Property)
              miVector.propMI=miVector.createGenPropMI(&broker,&eCtx,mName);
           if (miVector.miTypes & CMPI_MIType_Indication)
              miVector.indMI=miVector.createGenIndMI(&broker,&eCtx,mName);
        }
        else {
           if (miVector.miTypes & CMPI_MIType_Instance)
              miVector.instMI=miVector.createInstMI(&broker,&eCtx);
           if (miVector.miTypes & CMPI_MIType_Association)
              miVector.assocMI=miVector.createAssocMI(&broker,&eCtx);
           if (miVector.miTypes & CMPI_MIType_Method) 
              miVector.methMI=miVector.createMethMI(&broker,&eCtx);
           if (miVector.miTypes & CMPI_MIType_Property)
              miVector.propMI=miVector.createPropMI(&broker,&eCtx);
           if (miVector.miTypes & CMPI_MIType_Indication)
              miVector.indMI=miVector.createIndMI(&broker,&eCtx);
        }
}

void CMPIProvider::initialize(CIMOMHandle & cimom)
{

    _status = INITIALIZING;

    try
    {
	// yield beasmfore a potentially lengthy operation.
        pegasus_yield();
	CMPIProvider::initialize(cimom,miVector,_name,broker);
	if (miVector.miTypes & CMPI_MIType_Method) {
	   if (miVector.methMI->ft->miName==NULL) noUnload=true;
	}
    }
    catch(...) 
    {
	_status = UNKNOWN;
	_module->unloadModule();
	throw;
    }

    _status = INITIALIZED;
    _current_operations = 0;
}

Boolean CMPIProvider::tryTerminate(void)
{
   if(false == unload_ok())
   {
      return false;
   }

   Status savedStatus=_status;
   _status = TERMINATING;
   Boolean terminated = false;

   try
   {
      // yield before a potentially lengthy operation.
      pegasus_yield();
      try
      {
	if (noUnload==false) {
	   terminate();
	   if (noUnload==true) {
	      _status=savedStatus;
	      return false;
	   }
	   terminated=true;
	 }  
      }
      catch(...)
      {
	 PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			  "Exception caught in CMPIProviderFacade::tryTerminate() for " +
			  _name);
	 terminated = false;
	 
      }
      // yield before a potentially lengthy operation.
      pegasus_yield();
      if(terminated == true)
	 _module->unloadModule();
   }
   catch(...)
   {
      _status = UNKNOWN;
      
   }
   
   _status = TERMINATED;
   return terminated;
}

void CMPIProvider::_terminate(void)
{
    if (broker.clsCache) {
        std::cerr<<"--- CMPIProvider::_terminate() deleting ClassCache "<<std::endl;
	ClassCache::Iterator i=broker.clsCache->start();
	for (; i; i++) {
	   std::cerr<<"--- CMPIProvider::_terminate() deleting class "<<i.value()->getClassName()<<std::endl;
	   delete i.value(); }
	delete broker.clsCache;
    }

    const OperationContext opc;
    CMPIStatus rc={CMPI_RC_OK,NULL};
    CMPI_ContextOnStack eCtx(opc);

    if (miVector.miTypes & CMPI_MIType_Instance) {
       rc=miVector.instMI->ft->cleanup(miVector.instMI,&eCtx);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
    }   
    if (miVector.miTypes & CMPI_MIType_Association) {
       rc=miVector.assocMI->ft->cleanup(miVector.assocMI,&eCtx);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
    }   
    if (miVector.miTypes & CMPI_MIType_Method) {
       rc=miVector.methMI->ft->cleanup(miVector.methMI,&eCtx);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
    }   
    if (miVector.miTypes & CMPI_MIType_Property) {
       rc=miVector.propMI->ft->cleanup(miVector.propMI,&eCtx);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
    }   
    if (miVector.miTypes & CMPI_MIType_Indication) {
       rc=miVector.indMI->ft->cleanup(miVector.indMI,&eCtx);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
    }   

}


void CMPIProvider::terminate(void)
{
    Status savedStatus=_status;     
    _status = TERMINATING;
    try
    {
	// yield before a potentially lengthy operation.
	pegasus_yield();
	try 
       {
         _terminate();
	 if (noUnload==true) {
            _status=savedStatus;
	    return;
	 }
       }
       catch(...)
       {
	  PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			       "Exception caught in CMPIProviderFacade::Terminate for " + 
			       _name);
       }
	// yield before a potentially lengthy operation.
	pegasus_yield();

	_module->unloadModule();
    }
    catch(...)
    {
	_status = UNKNOWN;
   
	throw;
    }

    _status = TERMINATED;

}

Boolean CMPIProvider::operator == (const void *key) const 
{
   if( (void *)this == key)
      return true;
   return false;
}

Boolean CMPIProvider::operator == (const CMPIProvider &prov) const
{
   if(String::equalNoCase(_name, prov._name))
      return true;
   return false;
}

void CMPIProvider::get_idle_timer(struct timeval *t)
{
   if(t && _cimom_handle)
      _cimom_handle->get_idle_timer(t);
}

void CMPIProvider::update_idle_timer(void)
{
   if(_cimom_handle)
      _cimom_handle->update_idle_timer();
}

Boolean CMPIProvider::pending_operation(void)
{
   if(_cimom_handle)
      return _cimom_handle->pending_operation();
   return false; 
}


Boolean CMPIProvider::unload_ok(void)
{
   if (noUnload==true) return false;
   if(_no_unload.value() )
      return false;
   
   if(_cimom_handle)
      return _cimom_handle->unload_ok();
   return true;
}

//   force provider manager to keep in memory
void CMPIProvider::protect(void)
{ 
   _no_unload++;
}

// allow provider manager to unload when idle 
void CMPIProvider::unprotect(void)
{ 
   _no_unload--;
}

PEGASUS_NAMESPACE_END
