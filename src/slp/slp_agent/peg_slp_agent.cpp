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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "peg_slp_agent.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

class sa_reg_params
{
   public:

      sa_reg_params(const char*, const char*, const char*, const char*, unsigned short);
      ~sa_reg_params(void);
      
      char* url;
      char* attrs;
      char* type;
      char* scopes;
      Uint32 lifetime;
      Uint32 expire;
      

   private:
      sa_reg_params(void);
      sa_reg_params(const sa_reg_params&);
      sa_reg_params& operator=(const sa_reg_params&);
       
};



 sa_reg_params::sa_reg_params(const char*  _url, 
			     const char*  _attrs, 
			     const char*  _type,
			     const char*  _scopes,
			     unsigned short _lifetime)
{
   if(_url)
      url = strdup(_url);
   
   if(_attrs)
      attrs = strdup(_attrs);
   
   if(_type)
      type = strdup(_type);
   
   if(_scopes)
      scopes = strdup(_scopes);
   

   lifetime = _lifetime;
   Uint32 msec, now;
   System::getCurrentTime(now, msec);
   expire = 0;
}

sa_reg_params::~sa_reg_params(void)
{
   if(url)
      free(url);
   if(attrs)
      free(attrs);
   if(type)
      free(type);
   if(scopes)
      free(scopes);
}


slp_service_agent::slp_service_agent(void)
   : _listen_thread(service_listener, this, false),
   _initialized(0)

{
   try 
   {
      _init();
   }
   catch(...)
   {
      
   }
   if(_initialized.value() && _lib_handle && _create_client)
   {
      _rep = _create_client("239.255.255.253",
			    0,
			    427,
			    "DSA",
			    "DEFAULT",
			    TRUE,
			    FALSE);
   }
}

slp_service_agent::slp_service_agent(const char *local_interface, 
				     unsigned short target_port,
				     const char *scopes,
				     Boolean listen,
				     Boolean use_da)
   : _listen_thread(service_listener, this, false),
   _initialized(0)
{
   
   try 
   {
      _init();
   }
   catch(...)
   {
      
   }
   if(_initialized.value() && _lib_handle && _create_client)
   {
      _rep = _create_client("239.255.255.253",
			    local_interface,
			    target_port,
			    "DSA",
			    "scopes",
			    listen,
			    use_da);
   }
}

slp_service_agent::~slp_service_agent(void)
{
   try
   {
      _de_init();
   }
   catch(...)
   {
   }
   
}


void slp_service_agent::_init(void)
{

   _initialized = 0;
#ifdef PEGASUS_OS_TYPE_WINDOWS
   _lib_fileName.append("pegslp_client.dll");
#elif defined(PEGASUS_OS_HPUX)
   _lib_fileName.append("pegslp_client.sl");
#elif defined(PEGASUS_OS_OS400)
   _lib_fileName.append("pegslp_client");
#else
   _lib_fileName.append("libpegslp_client.so");
#endif
   _lib_handle = System::loadDynamicLibrary((const char *)_lib_fileName.getCString());

   if(_lib_handle)
   {
      _create_client = 
	 (slp_client * (*)(const char*, const char*,  uint16, const char*, const char*, BOOL, BOOL))
	 System::loadDynamicSymbol(_lib_handle, "create_slp_client");

      _destroy_client = 
	 (void (*)(struct slp_client *)) 
	 System::loadDynamicSymbol(_lib_handle, "destroy_slp_client");
      
      _find_das = 
	 (int (*)(struct slp_client *, const char *,  const char *))
	 System::loadDynamicSymbol(_lib_handle, "find_das");

      _test_reg = 
	 (uint32 (*)(char*, char*, char*, char*))
	 System::loadDynamicSymbol(_lib_handle, "test_srv_reg");

      _initialized = 1;

      if(_create_client == 0 || _destroy_client == 0 || _find_das == 0 || _test_reg == 0)
      {
    	 _initialized = 0;
         String symbol;
         if (_create_client == 0){symbol = "create_slp_client";}
         if (_destroy_client == 0){symbol = "destroy_slp_client";}
         if (_find_das == 0){symbol = "find_das";}
         if (_test_reg == 0){symbol = "test_srv_reg";}

         Logger::put(Logger::ERROR_LOG, "slp_agent", Logger::SEVERE,
                 "Link Error to library: $0, symbol: $1" ,
             _lib_fileName, symbol);

    	 System::unloadDynamicLibrary(_lib_handle);
      }
   }
}

void slp_service_agent::_de_init(void)
{
   if(_initialized.value() && _lib_handle)
   {
      if(_rep)
      {
	 _destroy_client(_rep);
	 _rep = 0;
      }

      try 
      {
	 System::unloadDynamicLibrary(_lib_handle);
	 _lib_handle = 0;
      }
      catch(...)
      {
	 _lib_handle = 0;
      }
   }
}

Boolean slp_service_agent::srv_register(const char* url, 
					const char* attributes, 
					const char* type,
					const char* scopes, 
					unsigned short lifetime)
{
   if(_initialized.value() == 0 )
      throw UninitializedObjectException();

   if(url == 0 || attributes == 0 || type == 0)
      return false;

   sa_reg_params* rp = 0;
   _internal_regs.lookup(url, rp);
   if(rp )
   {
      _internal_regs.remove(url);
      delete rp;
   }
   
   rp = new sa_reg_params(url, attributes, type, scopes, lifetime);
   
   _internal_regs.insert(url, rp);
   
   
   return true;
    
}


void slp_service_agent::unregister(void)
{
   if(_initialized.value() == 0 )
      throw UninitializedObjectException();
   _should_listen = 0;
   _listen_thread.join();
   
   while(slp_reg_table::Iterator i = _internal_regs.start())
   {
      sa_reg_params *rp = i.value();
      _internal_regs.remove(rp->url);
      delete rp;
   }
}



Uint32 slp_service_agent::test_registration(const char *url, 
					    const char *attrs, 
					    const char *type, 
 					    const char *scopes)
{

   if(_initialized.value() == 0 )
      throw UninitializedObjectException();

   //cout << "test_registration. type= " << type << endl;
   if(type ==  0)
      return 1;
   
   if(url == 0)
      return 2;
   
   if(attrs == 0)
      return 3;
   
   if(scopes == 0)
      return 4;
   
   char* _type = strdup(type);
   char* _url = strdup(url);
   char* _attrs = strdup(attrs);
   char* _scopes = strdup(scopes);

   Uint32 ccode = _test_reg(_type, _url, _attrs, _scopes);

   //cout << "rtn from _tst_reg: " << ccode << endl;
   
   free(_type);
   free(_url);
   free(_attrs);
   free(_scopes);
   return ccode;
}


void slp_service_agent::start_listener(void)
{
   // see if we need to use an slp directory agent 
   if(_initialized.value() == 0 )
      throw UninitializedObjectException();
   
   _using_das = _find_das(_rep, NULL, "DEFAULT");
   
   _should_listen = 1;
   _listen_thread.run();
   
}

PEGASUS_THREAD_RETURN 
PEGASUS_THREAD_CDECL slp_service_agent::service_listener(void *parm)
{
   Thread *myself = (Thread *)parm;
   if(myself == 0)
      throw NullPointer();
   
   slp_service_agent *agent = 
      (slp_service_agent *)myself->get_parm();


   
   lslpMsg msg_list;
   
   while(agent->_should_listen.value())
   {
      Uint32 now, msec;
      System::getCurrentTime(now, msec);
	 // now register everything
      
      for(slp_reg_table::Iterator i = agent->_internal_regs.start(); i ; i++)
      {
	 sa_reg_params *rp = i.value();

	 if(rp->expire == 0 || rp->expire < now - 1)
	 {
	    rp->expire = now + rp->lifetime;
	    
	    if(agent->_using_das.value())
	    { 
	       agent->_rep->srv_reg_all(
		  agent->_rep, 
		  rp->url, 
		  rp->attrs, 
		  rp->type, 
		  rp->scopes, 
		  rp->lifetime);
	    }
	    else
	    {
	       agent->_rep->srv_reg_local(
		  agent->_rep, 
		  rp->url, 
		  rp->attrs, 
		  rp->type, 
		  rp->scopes, 
		  rp->lifetime);
	    }
	 }
      }
      agent->_rep->service_listener(agent->_rep, 0, &msg_list);
      _LSLP_SLEEP(1);
   }
   myself->exit_self((PEGASUS_THREAD_RETURN) 0) ;
   return(0);
}

PEGASUS_NAMESPACE_END
