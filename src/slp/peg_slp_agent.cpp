//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "peg_slp_agent.h"

PEGASUS_NAMESPACE_BEGIN


slp_service_agent::slp_service_agent(void)
{
   try 
   {
      _init();
   }
   catch(...)
   {
      
   }
   if(_lib_handle && _create_client)
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
{
   
   try 
   {
      _init();
   }
   catch(...)
   {
      
   }
   if(_lib_handle && _create_client)
   {
      _rep = _create_client("239.255.255.253",
			    local_interface,
			    target_port,
			    "DSA",
			    "scpoes",
			    listen,
			    use_da);
   }
}

void slp_service_agent::_init(void)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
   _lib_fileName.append("slp_client.dll");
#elif defined(PEGASUS_OS_HPUX)
   _lib_fileName.append("slp_client.sl");
#elif defined(PEGASUS_OS_OS400)
   _lib_fileName.append("slp_client");
#else
   _lib_fileName.append("slp_client.so");
#endif
   _lib_handle = System::loadDynamicLibrary((const char *)_lib_fileName.getCstring());

   if(_lib_handle)
   {
      _create_client = 
	 (slp_client * (*)(const int8*, const int8*,  uint16, const int8*, const int8*, BOOL, BOOL))
	 System::loadDynamicSymbol(_lib_handle, "create_slp_client");

      _test_reg = 
	 (uint32 (*)(int8*, int8*, int8*, int8*))
	 System::locadDynamicSymbol(_lib_handle, "test_srv_reg");
   }
   else 
   {
      _create_client = 0;
      _test_reg = 0;
   }
}


void slp_service_agent::_de_init(void)
{
   if(_lib_handle)
   {
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


PEGASUS_NAMESPACE_END
