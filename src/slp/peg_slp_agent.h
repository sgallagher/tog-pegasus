//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/Exception.h>
#include "Linkage.h"
#include <Pegasus/../Unsupported/slp_client/src/cmd-utils/slp_client/slp_client.h>

PEGASUS_NAMESPACE_BEGIN


class sa_reg_params;

struct slpTableEqual
{
    static Boolean equal(const String & x, const String & y)
    {
        if (0 == String::compareNoCase(x, y))
            return true;
        return false;
    }
};

struct slpTableHash
{
      static Uint32 hash(const String & str)
      {
	 String cpy(str);
	 cpy.toLower();
	 Uint32 h = 0;
	 for(Uint32 i = 0, n = cpy.size(); i < n; i++)
            h = 5 * h + cpy[i];
	 return h;
      }
};

typedef HashTable<String, sa_reg_params*, slpTableEqual, slpTableHash > slp_reg_table;

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
      
      Boolean srv_register(const char *url, 
		       const char *attributes, 
		       const char *type,
		       const char *scopes, 
		       unsigned short lifetime);
      
      Uint32 test_registration(const char *url, 
			       const char *attrs, 
			       const char *type, 
			       const char *scopes);

      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL service_listener(void *);
      
      void start_listener(void);
      void unregister(void);
			
   private:



      slp_service_agent(const slp_service_agent & );
      slp_service_agent & operator= (const slp_service_agent &);

      struct slp_client* _rep;

      slp_client * (*_create_client )(const int8 *,
				      const int8 *, 
				      uint16,
				      const int8 *, 
				      const int8 *, 
				      BOOL, 
				      BOOL);
      void (*_destroy_client)(struct slp_client *);
      
      
      int (*_find_das)(struct slp_client *client, 
		       const int8 *predicate, 
		       const int8 *scopes);

      uint32 (*_test_reg)(int8 *, 
			  int8 *, 
			  int8 *, 
			  int8 *);
      
      DynamicLibraryHandle _lib_handle;
      String _lib_fileName;

      void _init(void);
      void _de_init(void);
      Thread _listen_thread;
      AtomicInt _should_listen;
      AtomicInt _initialized;
      AtomicInt _using_das;
      slp_reg_table _internal_regs;
};


// class PEGASUS_SLP_LINKAGE slp_user_agent
// {

// };



PEGASUS_NAMESPACE_END

#endif // Pegasus_slp_agent_h
