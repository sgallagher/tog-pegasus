//%//-*-c++-*-//////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationContext_h
#define Pegasus_OperationContext_h
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/internal_dq.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Provider/ProviderException.h>
#include <stdio.h>

PEGASUS_NAMESPACE_BEGIN

#define CONTEXT_EMPTY            0x00000000
#define CONTEXT_IDENTITY         0x00000001
#define CONTEXT_AUTHENICATION    0x00000002
#define CONTEXT_AUTHORIZATION    0x00000004
#define CONTEXT_OTHER_SECURITY   0x00000008
#define CONTEXT_LOCALE           0x00000010
#define CONTEXT_OPTIONS          0x00000020
#define CONTEXT_VENDOR           0x00000040
#define CONTEXT_UID_PRESENT      0x00000080
#define CONTEXT_UINT32_PRESENT   0x00000100
#define CONTEXT_OTHER            0x00000200

#define OPERATION_NONE                      0x00000000
#define OPERATION_LOCAL_ONLY                0x00000001
#define OPERATION_INCLUDE_QUALIFIERS        0x00000002
#define OPERATION_INCLUDE_CLASS_ORIGIN      0x00000004
#define OPERATION_DEEP_INHERITANCE          0x00000008
#define OPERATION_PARTIAL_INSTANCE          0x00000010
#define OPERATION_REMOTE_ONLY               0x00000020
#define CONTEXT_COPY_MEMORY                 0x00000040
#define CONTEXT_DELETE_MEMORY               0x00000080
#define CONTEXT_POINTER                     0x00000100


//void PEGASUS_EXPORT default_serialize(Sint8 *, Uint32 ) throw(BufferTooSmall, NotSupported);
void PEGASUS_EXPORT default_delete(void * data) ;
void PEGASUS_EXPORT stringize_uid(void *uid, Sint8 **dest, size_t *size) throw (NullPointer, BufferTooSmall);
void PEGASUS_EXPORT binaryize_uid(Sint8 *uid, void *dest, size_t size) throw(NullPointer, BufferTooSmall);

class PEGASUS_EXPORT context
{
   public:
      context(Uint32 data_size,
		 void *data, 
		 void (*del)(void *), 
		 Uint32 uint_val ,
		 Uint32 key , 
		 Uint32 flag ,
		 Uint8 *uid);
      
      
      ~context(void);
      
      inline Boolean operator == (const Uint32 uint_val) const
      {
	 if(_uint_val == uint_val)
	    return true;
	 return false;
      }
      
      inline Boolean operator == (const Uint8 *uid) const
      {
	 if(uid == 0)
	    return(false);
	 if( ! memcmp(_uid, const_cast<Uint8 *>(uid), 16) )
	    return true;
	 return false;
      }
      
      inline Boolean operator == (const void *data) const
      {
	 if(data == 0 || _data == 0)
	    return false;
	 if(! memcmp(_data, const_cast<void *>(data), _size))
	    return true;
	 return false;
      }
      
      inline Boolean operator == (const context & c) const
      {
	 if( true == (operator == ((const Uint32)c._uint_val)))
	    if( true == (operator ==((const Uint8 *)c._uid )))
	       if( true == (operator ==((const void *)c._data)))
		  return true;
	 return false;
      }
      
      inline Uint32 get_key(void) 
      {
	 return _key;
      }
      
      inline Uint32 get_flag(void)
      {
	 return _flag;
      }
      
      inline Uint32 get_uint_val(void)
      {
	 return _uint_val;
      }
      
      inline void *get_data(void **buf, Uint32 *size)
      {
	 if(buf != 0)
	    *buf = _data;
	 if(size != 0)
	    *size = _size;
	 return(_data);
      }
      
      inline void get_uid(Uint8 *buf)
      {
	 if(buf != 0)
	    memcpy(buf, _uid, 16);
      }
      
   private:
      size_t _size;
      Uint32 _uint_val;
      Uint32 _key;
      Uint32 _flag;
      Uint8 _uid[16];
      void *_data;
      void (*_delete_func)(void *data);
      friend class OperationContext;
} ;


class PEGASUS_EXPORT OperationContext
{
   public:
      OperationContext(void)
	 : _context(true) 
      {
      }
      
      ~OperationContext(void)
      {

      }
      
      void add_context(Uint32 data_size,
		       void *data, 
		       void (*del)(void *), 
		       Uint32 uint_val,
		       Uint32 key , 
		       Uint32 flag ,
		       Uint8 *uid );

      void add_context(context *);

      // ATTN: This is a hack, but was necessary to compile on HP-UX
      Boolean operator==(OperationContext *rhs) { return this == rhs; }

      context *remove_context(void);
      context *remove_context(Uint32 uint_val);
      context *remove_context(Uint8 *uid); 
      context *remove_context(void *data);
      context *remove_context_key(Uint32 key);
      
   private:
      unlocked_dq<class context> _context;
} ;

inline void OperationContext::add_context(Uint32 data_size,
		       void *data, 
		       void (*del)(void *), 
		       Uint32 uint_val ,
		       Uint32 key , 
		       Uint32 flag ,
		       Uint8 *uid )

{
   context *c = new context(data_size, data, del, uint_val, key, flag, uid );
   _context.insert_first(c);
   return;
}

inline void OperationContext::add_context(context *c)
{
   if(c != 0)
      _context.insert_first(c);
   return;
}

inline context *OperationContext::remove_context(void)
{
   return(_context.remove_first());
}

inline context *OperationContext::remove_context(Uint32 uint_val)
{
   context *c = 0;
   c = _context.next(c);
   while(c != 0)
   {
      if (c->operator==(uint_val) == true)
      {
	 _context.remove(c);
	 break;
      }
      c = _context.next(0);
   }
   return c;
}

inline context *OperationContext::remove_context(Uint8 *uid) 
{
   context *c = 0;
   c = _context.next(c);
   while(c != 0)
   {
      if (c->operator==(uid) == true)
      {
	 _context.remove(c);
	 break;
      }
      c = _context.next(0);
   }
   return c;
}

inline context *OperationContext::remove_context(void *data)
{
   context *c = 0;
   c = _context.next(c);
   while(c != 0)
   {
      if (c->operator==(data) == true)
      {
	 _context.remove(c);
	 break;
      }
      c = _context.next(0);
   }
   return c;
}

inline context *OperationContext::remove_context_key(Uint32 key)
{
   context *c = 0;
   c = _context.next(c);
   while(c != 0)
   {
      if (c->get_key() == key)
      {
	 _context.remove(c);
	 break;
      }
      c = _context.next(0);
   }
   return c;
}



PEGASUS_NAMESPACE_END

#endif
