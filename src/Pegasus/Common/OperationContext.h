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
#include <Pegasus/Common/Exception.h>

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

#define OPERATION_NONE                      0x00000000
#define OPERATION_LOCAL_ONLY                0x00000001
#define OPERATION_INCLUDE_QUALIFIERS        0x00000002
#define OPERATION_INCLUDE_CLASS_ORIGIN      0x00000004
#define OPERATION_DEEP_INHERITANCE          0x00000008
#define OPERATION_PARTIAL_INSTANCE          0x00000010
#define OPERATION_REMOTE_ONLY               0x00000020

void PEGASUS_EXPORT default_serialize(Sint8 *, Uint32 ) throw(BufferTooSmall, NotImplemented);
void PEGASUS_EXPORT default_delete(void * data) ;
void PEGASUS_EXPORT stringize_uid(void *uid, Sint8 **dest, size_t *size) throw (NullPointer, BufferTooSmall);
void PEGASUS_EXPORT binaryize_uid(Sint8 *uid, void *dest, size_t size) throw(NullPointer, BufferTooSmall);

class PEGASUS_EXPORT OperationContext
{

   public:

      OperationContext(Uint32 key = CONTEXT_EMPTY, Uint32 flag = OPERATION_LOCAL_ONLY)
	 : _flag(flag), _key(key), _size(0), _data(NULL)
      {

	 _serialize = default_serialize;
	 _deserialize = (void (*)(void *,unsigned int))default_serialize;
	 _delete_func = default_delete;
	 memset(_uid, 0x00, 16);
      }

      OperationContext(Uint32 key, void * uid, size_t size, Uint32 flag = OPERATION_LOCAL_ONLY)
	 : _flag(flag), _key(key), _size(size)
      {
	 if (uid == NULL )
	    throw NullPointer();
	 _serialize = default_serialize;
	 _deserialize = (void (*)(void *,unsigned int))default_serialize;
	 _delete_func = default_delete;

	 memcpy(_uid, uid, 16);
	 _data = ::operator new(_size);
      }

      ~OperationContext(void)
      {
	 if(_data != NULL)
	    if(_delete_func != NULL)
	       _delete_func(_data);
      }

      Uint32 get_flag(void) { return _flag; }


      void set_flag(Uint32 flag) { _flag = flag; }

      Uint32 get_key(void) { return _key; }

      void put_data(void (*del)(void *), size_t size, void *data ) throw(NullPointer)
      {
	 if(_data != NULL)
	    if(_delete_func != NULL)
	       _delete_func(_data);

	 _delete_func = del;
	 _data = data;
	 _size = size;
	 return ;
      }
      size_t get_size(void) { return _size; }

      void get_data(void **data, size_t *size)
      {
	 if(data == NULL || size == NULL)
	    throw NullPointer();
	
	 *data = _data;
	 *size = _size;
	 return;
	
      }

      void copy_data(void **buf, size_t *size) throw(BufferTooSmall, NullPointer)
      {
	 if((buf == NULL) || (size == NULL))
	    throw NullPointer() ;
	 *buf = ::operator new(_size);
	 *size = _size;
	 memcpy(*buf, _data, _size);
	 return;
      }


      void get_uid(Uint8 **uid)
      {
	 if(uid == NULL)
	    throw NullPointer();
	 if(_flag & CONTEXT_UID_PRESENT)
	    *uid = _uid;
	 else
	    *uid = NULL;
	 return;
      }


      void copy_uid(Uint8 **uid)
      {
	 if(uid == NULL)
	    throw NullPointer();
	 if(_flag & CONTEXT_UID_PRESENT)
	 {
	    *uid = new Uint8[16];
	    memcpy(*uid, _uid, 16);
	 }
	 else
	    *uid = NULL;
	 return;
      }

      inline Boolean operator==(const void *key) const
      {
	 if(key == NULL)
	    return false;
	
	 if(_flag & CONTEXT_UID_PRESENT)
	 {
	    if ( 0 == memcmp(key, _uid, 16) )
	       return true;
	 }
	 else if (*(Uint32 *)key == _key)
	    return true;
	 return(false);
      }

      inline Boolean operator==(const OperationContext& b) const
      {
	 if(_flag & CONTEXT_UID_PRESENT)
	    return(operator==((const void *)b._uid));
	 return(operator==((const void *)b._key));
      }

   private:
      Uint32 _flag;
      Uint32 _key;
      size_t _size;
      Uint8 _uid[16];

      void *_data;

      void (*_serialize)(Sint8 *dest, Uint32 dest_size) ;
      void (*_deserialize)(void *dest, Uint32 dest_size) ;
      void (*_delete_func)(void *data);
};


PEGASUS_NAMESPACE_END

#endif
