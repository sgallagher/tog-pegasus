//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationContext.h"

PEGASUS_NAMESPACE_BEGIN

void default_serialize(Sint8 *dst, Uint32 dst_sz) throw(BufferTooSmall, NotSupported)
{
   throw NotSupported("no serialization routine present") ;
}

void default_delete(void * data) 
{ 
   if( data != NULL)
      ::operator delete(data); 
}

void stringize_uid(void *uid, Sint8 **dest, size_t *size) throw (NullPointer, BufferTooSmall)
{
   Sint8 *ptr;
   
   if(uid == NULL || dest == NULL || *dest == NULL || size == NULL)
      throw NullPointer();
   
   if( *size < 37 )
      throw BufferTooSmall(37);
 
   ptr = (Sint8 *)uid;
   sprintf(*dest,
	   "%.2d%.2d%.2d%.2d-%.2d%.2d-%.2d%.2d-%.2d%.2d-%.2d%.2d%.2d%.2d%.2d%.2d",
	   *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3),*(ptr + 4), *(ptr + 5),
	   *(ptr + 6), *(ptr + 7), *(ptr + 8), *(ptr + 9), *(ptr + 10),
	   *(ptr + 11), *(ptr + 12),*(ptr + 13),*(ptr + 14),*(ptr + 15) );
   *dest += 37;
   *size -= 37;
   return;
}


void binaryize_uid(Sint8 *uid, void *dest, size_t size) throw(NullPointer, BufferTooSmall)
{

   if(uid == NULL || dest == NULL)
      throw NullPointer();
   if(size < 16)
      throw(BufferTooSmall(16));   
   Sint8 *src = uid;
   Sint8 *dst = (Sint8 *)dest;
   
   int i = 0;
   Sint8 temp;
   
   while( i < 16 )
   {
      temp = *(src + 2);
      *(src + 2) = 0x00;
      *(dst + i) = (Sint8)atoi(src);
      *(src + 2) = temp;
      i++;
      src += 2;
      if(*src == '-')
	 src++;
   }
}


context::context(Uint32 data_size,
		 void *data, 
		 void (*del)(void *), 
		 Uint32 uint_val ,
		 Uint32 key , 
		 Uint32 flag ,
		 Uint8 *uid)
   : _size(data_size), _uint_val(uint_val), _key(key), _flag(flag)
     
{
   if(uid != 0)
      memcpy(_uid, uid, 16);
   else
      memset(_uid, 0x00, 16);
   
   if(flag & CONTEXT_POINTER)
      _data = data;
   else if (flag & CONTEXT_COPY_MEMORY)
   {
      if(data != 0)
      {
	 _data = ::operator new(_size);
	 memcpy(_data, data, _size);
      }
      
   }

   if(flag & CONTEXT_DELETE_MEMORY)
   {
      if(del != 0)
	 _delete_func = del;
      else 
	 _delete_func =  default_delete;
   }
}

context::~context(void)
{
   if(_flag & CONTEXT_DELETE_MEMORY)
   {
      if(_delete_func != 0)
	 _delete_func(_data);
      else
	 default_delete(_data);
   }
   
}


PEGASUS_NAMESPACE_END
