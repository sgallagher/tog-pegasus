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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/IPC.h>
#include "DQueue.h"

PEGASUS_NAMESPACE_BEGIN

DQueue<class L> * DQueue<L>::_headOfFreeList;
const int DQueue<class L>::BLOCK_SIZE = 20;
Mutex DQueue<class L>::_alloc_mut;


void * DQueue<class L>::operator new(size_t size)
{
   if (size != sizeof(DQueue<L>))
      return ::operator new(size);
   
   _alloc_mut.lock(pegasus_thread_self());
   
   DQueue<L> *p = _headOfFreeList;
   if(p)
      _headOfFreeList = p->_dq_next;
   else
   {
      DQueue<L> * newBlock = 
	 static_cast<DQueue<L> *>(::operator new(BLOCK_SIZE * sizeof(DQueue<L>)));
      int i;
      for( i = 1; i < BLOCK_SIZE - 1; ++i)
	 newBlock[i]._dq_next = &newBlock[i];
      newBlock[BLOCK_SIZE - 1]._dq_next = 0;
      
      p = newBlock;
      _headOfFreeList = &newBlock[1];
   }
   _alloc_mut.unlock();
   
   return p;
}

void DQueue<class L>::operator delete(void *deadObject, size_t size)
{
   if(deadObject == 0)
      return;
   if(size != sizeof(DQueue<L>))
   {
      ::operator delete(deadObject);
      return;
   }
   DQueue<L> *p = static_cast<DQueue<L> *>(deadObject);
   _alloc_mut.lock(pegasus_thread_self());
   p->_dq_next = _headOfFreeList;
   _headOfFreeList = p;
   _alloc_mut.unlock();
}



AsyncDQueue<class L> * AsyncDQueue<L>::_headOfFreeList;
const int AsyncDQueue<class L>::BLOCK_SIZE = 20;
Mutex AsyncDQueue<class L>::_alloc_mut;

void * AsyncDQueue<class L>::operator new(size_t size)
{
   if (size != sizeof(AsyncDQueue<L>))
      return ::operator new(size);
   
   _alloc_mut.lock(pegasus_thread_self());
   
   AsyncDQueue<L> *p = _headOfFreeList;
   if(p)
      _headOfFreeList = p->_dq_next;
   else
   {
      AsyncDQueue<L> * newBlock = 
	 static_cast<AsyncDQueue<L> *>(::operator new(BLOCK_SIZE * sizeof(AsyncDQueue<L>)));
      int i;
      for( i = 1; i < BLOCK_SIZE - 1; ++i)
	 newBlock[i]._dq_next = &newBlock[i];
      newBlock[BLOCK_SIZE - 1]._dq_next = 0;
      
      p = newBlock;
      _headOfFreeList = &newBlock[1];
   }
   _alloc_mut.unlock();
   
   return p;
}

void AsyncDQueue<class L>::operator delete(void *deadObject, size_t size)
{
   if(deadObject == 0)
      return;
   if(size != sizeof(AsyncDQueue<L>))
   {
      ::operator delete(deadObject);
      return;
   }
   AsyncDQueue<L> *p = static_cast<AsyncDQueue<L> *>(deadObject);
   _alloc_mut.lock(pegasus_thread_self());
   p->_dq_next = _headOfFreeList;
   _headOfFreeList = p;
   _alloc_mut.unlock();
}
       

PEGASUS_NAMESPACE_END
