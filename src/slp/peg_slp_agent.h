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


#ifndef Pegasus_slp_agent_h
#define Pegasus_slp_agent_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/../Unsupported/slp_client/src/cmd-utils/slp_client/slp_client.h>

PEGASUS_NAMESPACE_BEGIN



class PEGASUS_SLP_LINKAGE slp_service_agent 
{
   public:
      slp_service_agent(void);
      slp_service_agent(const char *local_interface, 
			unsigned short target_port,
			const char *scopes,
			Boolean listen,
			Boolean use_da);
      ~slp_service_agent(void);
      
      Boolean register_local(const char *url, 
			     const char *attributes, 
			     const char *type,
			     const char *scopes, 
			     unsigned short lifetime);
      
      Boolean register_multicast(const char *interface, 
				 const char *url, 
				 const char *attributes, 
				 const char *type,
				 const char *scopes, 
				 unsigned short lifetime);
      
      Boolean register_broadcast(const char *url, 
				 const char *attributes, 
				 const char *type,
				 const char *scopes, 
				 unsigned short lifetime);
      
      Boolean register_da(const char *url, 
			  const char *attributes, 
			  const char *type,
			  const char *scopes, 
			  unsigned short lifetime);
      
      Uint32 test_registration(const char *type, 
			       const char *url, 
			       const char *attrs, 
			       const char *scopes);

      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL service_listener(void *);
			
   private:
      slp_service_agent(const slp_service_agent & );
      slp_service_agent & operator= (const slp_service_agent &);

      slp_client* _rep;

      slp_client * (*_create_client )(const int8 *,
				      const int8 *, 
				      uint16,
				      const int8 *, 
				      const int8 *, 
				      BOOL, 
				      BOOL);
      
      uint32 (*_test_reg)(int8 *, 
			  int8 *, 
			  int8 *, 
			  int8 *);
      
      DynamicLibraryHandle _lib_handle;
      String _lib_fileName;

      void _init(void);
      void _de_init(void);
      
};


// class PEGASUS_SLP_LINKAGE slp_user_agent
// {

// };



PEGASUS_NAMESPACE_END

#endif // Pegasus_slp_agent_h
