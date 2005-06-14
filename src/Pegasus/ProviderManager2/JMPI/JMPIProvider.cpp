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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "JMPIProvider.h"

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProviderModule.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

#ifdef PEGASUS_DEBUG
#define DDD(x) if (JMPIjvm::trace) x;
#else
#define DDD(x)
#endif

#include "Convert.h"

// set current operations to 1 to prevent an unload
// until the provider has had a chance to initialize
JMPIProvider::JMPIProvider(const String & name,
		   JMPIProviderModule *module,
		   ProviderVector *mv)
   : _module(module), _cimom_handle(0), _name(name),
     _no_unload(0), _rm(0)
{
   _current_operations = 1;
   _currentSubscriptions = 0;
   miVector=*mv;
   jProvider=mv->jProvider;
   jProviderClass=mv->jProviderClass;
   noUnload=false;
   cachedClass=NULL;
}

JMPIProvider::JMPIProvider(JMPIProvider *pr)
  : _module(pr->_module), _cimom_handle(0), _name(pr->_name),
    _no_unload(0), _rm(0)
{
   _current_operations = 1;
   _currentSubscriptions = 0;
   miVector=pr->miVector;
   _cimom_handle=new CIMOMHandle();
   noUnload=pr->noUnload;
   cachedClass=NULL;
}

JMPIProvider::~JMPIProvider(void)
{
   delete cachedClass;
}

JMPIProvider::Status JMPIProvider::getStatus(void) const
{
    AutoMutex lock(_statusMutex);
    return(_status);
}

JMPIProviderModule *JMPIProvider::getModule(void) const
{
    return(_module);
}

String JMPIProvider::getName(void) const
{
    return(_name);
}

void JMPIProvider::initialize(CIMOMHandle& cimom)
{
    _status       = INITIALIZING;
    _cimom_handle = &cimom;

    DDD(PEGASUS_STD(cout)<<"--- JMPIProvider::Initialize()"<<PEGASUS_STD(endl));

    JvmVector *jv  = 0;
    JNIEnv    *env = JMPIjvm::attachThread(&jv);

    // public abstract void initialize (org.pegasus.jmpi.CIMOMHandle ch)
    //        throws org.pegasus.jmpi.CIMException
    jmethodID id = env->GetMethodID((jclass)jProviderClass,
                                    "initialize",
                                    "(Lorg/pegasus/jmpi/CIMOMHandle;)V");

    DDD(PEGASUS_STD(cout)
        <<"--- JMPIProvider::Initialize:id = "
        <<PEGASUS_STD(hex)
        <<(int)id
        <<PEGASUS_STD(dec)
        <<PEGASUS_STD(endl));

    JMPIjvm::checkException(env);

    if (id != NULL)
    {
       jstring jName = env->NewStringUTF(_name.getCString());

       JMPIjvm::checkException(env);

       jint    jCimomRef = DEBUG_ConvertCToJava (CIMOMHandle*, jint, &cimom);
       jobject jch       = env->NewObject(jv->CIMOMHandleClassRef,
                                          JMPIjvm::jv.CIMOMHandleNewISt,
                                          jCimomRef,
                                          jName);

       JMPIjvm::checkException(env);

       env->CallVoidMethod((jobject)jProvider,id,jch);

       JMPIjvm::checkException(env);
    }
    env->ExceptionClear();

    JMPIjvm::detachThread();

    _status             = INITIALIZED;
    _current_operations = 0;
}

Boolean JMPIProvider::tryTerminate(void)
{
    return false;
}

void JMPIProvider::_terminate(void)
{
}


void JMPIProvider::terminate(void)
{
}

Boolean JMPIProvider::operator == (const void *key) const
{
   if( (void *)this == key)
      return true;
   return false;
}

Boolean JMPIProvider::operator == (const JMPIProvider &prov) const
{
   if(String::equalNoCase(_name, prov._name))
      return true;
   return false;
}
/*
void JMPIProvider::get_idle_timer(struct timeval *t)
{
   if(t && _cimom_handle)
      _cimom_handle->get_idle_timer(t);
}

void JMPIProvider::update_idle_timer(void)
{
   if(_cimom_handle)
      _cimom_handle->update_idle_timer();
}

Boolean JMPIProvider::pending_operation(void)
{
   if(_cimom_handle)
      return _cimom_handle->pending_operation();
   return false;
}


Boolean JMPIProvider::unload_ok(void)
{
   return false;
}
*/
//   force provider manager to keep in memory
void JMPIProvider::protect(void)
{
  // _no_unload++;
}

// allow provider manager to unload when idle
void JMPIProvider::unprotect(void)
{
  // _no_unload--;
}

Boolean JMPIProvider::testIfZeroAndIncrementSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    Boolean isZero = (_currentSubscriptions == 0);
    _currentSubscriptions++;

    return isZero;
}

Boolean JMPIProvider::decrementSubscriptionsAndTestIfZero ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    _currentSubscriptions--;
    Boolean isZero = (_currentSubscriptions == 0);

    return isZero;
}

Boolean JMPIProvider::testSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    Boolean currentSubscriptions = (_currentSubscriptions > 0);

    return currentSubscriptions;
}

void JMPIProvider::resetSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    _currentSubscriptions = 0;
}

void JMPIProvider::setProviderInstance (const CIMInstance & instance)
{
    _providerInstance = instance;
}

CIMInstance JMPIProvider::getProviderInstance ()
{
    return _providerInstance;
}

PEGASUS_NAMESPACE_END
