//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef CIMOM_include
#define CIMOM_include

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/_cimom/CimomMessage.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_EXPORT module_capabilities 
{
   public:
      static Uint32 async;
      static Uint32 remote;
      static Uint32 trusted;
} ;

class PEGASUS_EXPORT message_module
{
   public:
      message_module(const String & name,
		     Uint32 capabilities,
		     Uint32 messages,
		     Uint32 queue)
      
	 : _name(name), _capabilities(capabilities),
	   _messages(messages), _q_id(queue)  { }
      
      Boolean operator == (const String & name );
      Boolean operator == (const message_module & mm );
      
   private:
      String _name;
      Uint32 _capabilities;
      Uint32 _messages;
      Uint32 _q_id;
};


class PEGASUS_EXPORT cimom : public MessageQueue
{
  
   public : cimom(void)
      : MessageQueue("cimom"), _modules(true, 20) {  }
            
   protected:
      Uint32 get_module_q(const String & name);
      void _enqueueResponse(CimomRequest *req, CimomReply *rep);
      
   private:
      AsyncDQueue<message_module> _modules;
};



inline Boolean message_module::operator == (const String & name )
{
   if(name == this->_name)
      return true;
   return false;
   
}

inline Boolean message_module::operator == (const message_module & mm)
{
   if(this == &mm)
      return true;
   if( _name == mm._name )
      if ( _capabilities == mm._capabilities)
	 if(_messages == mm._messages)
	    if(_q_id == mm._q_id)
	       return true;
   
   return false;
   
}


PEGASUS_NAMESPACE_END

#endif // CIMOM_include
