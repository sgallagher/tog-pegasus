//%//////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2002 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of chargem, to any person obtaining a copy
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


#include <Pegasus/suballoc/suballoc.h>
#include <new.h>
PEGASUS_NAMESPACE_BEGIN
peg_suballocator internal_allocator;

void * PEGASUS_SUBALLOC_LINKAGE pegasus_alloc(size_t size)
{
   return internal_allocator.vs_malloc(size, 
 				       &(internal_allocator.get_handle())) ;
}

void PEGASUS_SUBALLOC_LINKAGE pegasus_free(void * dead) 
{
   internal_allocator.vs_free(dead); 
}

void * operator new(size_t size)
{
   return internal_allocator.vs_malloc(size, 
 				       &(internal_allocator.get_handle())) ;
}

void operator delete(void *dead)
{
   internal_allocator.vs_free(dead); 
} 


// void * operator new(size_t size)
// {
//    cout << "operator new " << endl;
   
// //    if( internal_allocator.get_mode() == false)
// //    {
// //       cout << " recursing on operator new " << endl;
      
// //       return(::operator new(size));
// //    }
   
   
//    if( size == 0 )
//       size = 1;
//    void *p;
   
//    while(1)
//    {
//       p = internal_allocator.vs_malloc(size, 
// 				       &(internal_allocator.get_handle())) ;
//       if( p )
// 	 return p;
//       new_handler global = set_new_handler(0);
//       set_new_handler(global);
//       if( global) 
// 	 (*global)();
//       else
// 	 throw std::bad_alloc();
//    }
// }

// void operator delete(void *dead)
// {
//    if( dead == 0 )
//       return;
//    if( internal_allocator.get_mode() == false)
//       return(::operator delete (dead));
//    internal_allocator.vs_free(dead);
//    return;
// }

// void * operator new [] (size_t size)
// {

//    if( internal_allocator.get_mode() == false)
//       return(::operator new(size));
   
//    if( size == 0 )
//       size = 1;
//    void *p;
   
//    while(1)
//    {
//       p = internal_allocator.vs_malloc(size, 
// 				       &(internal_allocator.get_handle()), 
// 				       ARRAY) ;
//       if( p )
// 	 return p;
//       new_handler global = set_new_handler(0);
//       set_new_handler(global);
//       if( global) 
// 	 (*global)();
//       else
// 	 throw std::bad_alloc();
//    }
// }

// void operator delete [] (void *dead)
// {
//    if( dead == 0 )
//       return;
//    if( internal_allocator.get_mode() == false)
//       return(::operator delete (dead));
//    internal_allocator.vs_free(dead, ARRAY);
//    return;
// }


// <<< Sun May  5 22:25:14 2002 mdd >>> 
// to do: 
// flag to note array new and array delete versus normal new and delete
// pattern for deleted objects to cause segfault when dereferencing freed memory
// Boolean to indication wether to use debug features or to pass through to 
// global new and global delete
// void * operator new(size_t size);
// void * operator new [] (size_t size);
// void operator delete(void *);
// void operator delete [] (void *);

const Sint8 peg_suballocator::dumpFileName[] = ".peg_suballoc_dump.txt";
const Uint8 peg_suballocator::guard[] = {0x01, 0x02, 0x03, 0x04, 0x05, 
					 0x06, 0x07, 0x08, 0x09, 0x09, 
					 0x08, 0x07, 0x06, 0x05, 0x04, 
					 0x03};
const Uint8 peg_suballocator::alloc_pattern = 0xaa;
const Uint8 peg_suballocator::delete_pattern = 0xee;

const Sint32 peg_suballocator::nodeSizes[3][16] = 
{
   {
      0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 
      0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xff
   },
   {
      0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 
      0x800, 0x900, 0xa00, 0xb00, 0xc00, 0xd00, 0xe00, 
      0xf00, 0xfff
   },
   {
      0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 
      0x7000, 0x8000, 0x9000, 0xa000, 0xb000, 0xc000, 
      0xd000, 0xe000, 0xf000, 0xffff
   }
};	

// prototypes that begin with an underscore do not attempt to 
// gain ownership of semaphores and can be safely called
// by a process that owns a semaphore


const Uint32 peg_suballocator::preAlloc[3][16] = 
{
   {	2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },	
   {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};	

const Uint32 peg_suballocator::step[3][16] = 
{
   {2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

peg_suballocator::peg_suballocator(void)
   : debug_mode(true), internal_handle("internal_suballoc_log")
{ 
   sprintf(global_log_filename, "%s", dumpFileName);
   InitializeSubAllocator(global_log_filename);
   return;
}

peg_suballocator::peg_suballocator(Sint8 *log_file_name, Boolean mode)
   : debug_mode(mode), internal_handle("internal_suballoc_log")
{
   if(log_file_name)
      snprintf(global_log_filename, MAX_PATH_LEN, "%s", log_file_name);
   else 
      sprintf(global_log_filename, "%s", dumpFileName);
   InitializeSubAllocator(global_log_filename);
   return;
}

peg_suballocator::~peg_suballocator(void)
{
   DeInitSubAllocator(&internal_handle);
}

peg_suballocator::SUBALLOC_HANDLE *peg_suballocator::InitializeProcessHeap(Sint8 *f)
{
   WAIT_MUTEX(&init_mutex);
   if( initialized == 0 )
   {
      RELEASE_MUTEX(&init_mutex);
      if(false == InitializeSubAllocator())
	 return NULL;
      WAIT_MUTEX(&init_mutex);
   }
   
   SUBALLOC_HANDLE *h = NULL;
   
   if(initialized )
   {
      h = (SUBALLOC_HANDLE *)calloc(1, sizeof(SUBALLOC_HANDLE));
      /* FREE the handle on de-init !!!! */
      if (NULL != h)
      {
	 memcpy(&(h->logpath[0]), f, 255);
	 init_count++;
      }
   }
   RELEASE_MUTEX(&init_mutex);
   return(h);
}	

/****************************************************************
 *  InitializeSubAllocator
 *
 *
 *  PARAMETERS: none
 *
 *  DESCRIPTION: If suballocator is already initialized, returns
 *				 true immediately. Otherwise, allocates node list 
 *				 heads and performs pre-allocations. 
 *
 *  RETURNS: true if successful, false otherwise
 *
 ***************************************************************/
Boolean peg_suballocator::InitializeSubAllocator(Sint8 *file)
{
   SUBALLOC_NODE *temp;
   Sint32 i, o;
   Boolean ccode;
   Sint32 waitCode;
   
   WAIT_MUTEX(&init_mutex);
   if(initialized)
   {
      RELEASE_MUTEX(&init_mutex);
      return true;
   }

   if(CREATE_MUTEX(&globalSemHandle))
      return(false);
   
   WAIT_MUTEX(&globalSemHandle, 1000, &waitCode);

   // check to see if we are already initialized
   // gain ownership of global semaphore here
   

   for (o = 0; o < 3; o++)
   {
      for (i = 0; i < 16; i++)
      {
	 // allocate our list heads
	 if (0 == ((Sint32)(CREATE_MUTEX(&(semHandles[o][i])))))
	 {

	    nodeListHeads[o][i] = (SUBALLOC_NODE *)calloc(1, sizeof (SUBALLOC_NODE));
	    if ( nodeListHeads[o][i] == NULL )
	    {
	       while (o >= 0)
	       {
		  while (i >= 0)
		  {
		     if (nodeListHeads[o][i] != NULL)
		     {
			free(nodeListHeads[o][i]);
			nodeListHeads[o][i] = NULL;
		     }
		     CLOSE_MUTEX(&(semHandles[o][i]));
		     i--;
		  }
		  o--;
	       }
	       RELEASE_MUTEX(&globalSemHandle);
	       RELEASE_MUTEX(&init_mutex);
	       
	       return(false);
	    }

	    temp = nodeListHeads[o][i];
	    temp->next = temp->prev = temp;
	    temp->isHead = true;
	    temp->avail = NORMAL;
	    memcpy(temp->guardPre, guard, GUARD_SIZE);
	    memcpy(temp->guardPost, guard, GUARD_SIZE);
	    if (preAlloc[o][i])
	       // we don't need to own the listhead semaphore because we
	       // are initializing
	       ccode = _Allocate(o, i, PRE_ALLOCATE);
	    if (ccode == false)
	    {
	       while (o >= 0)
	       {
		  while (i >= 0)
		  {
		     if (nodeListHeads[o][i] != NULL)
		     {
			free(nodeListHeads[o][i]);
			nodeListHeads[o][i] = NULL;
		     }

		     CLOSE_MUTEX(&(semHandles[o][i]));

		     i--;
		  }
		  o--;
	       }
	       RELEASE_MUTEX(&globalSemHandle);
	       RELEASE_MUTEX(&init_mutex);
	       return(false);
	    }
	 }
      }	
   }
   initialized = 1;
   // release global concurrency semaphore here
   RELEASE_MUTEX(&globalSemHandle);
   RELEASE_MUTEX(&init_mutex);
   return(true);
}	


/****************************************************************
 *  _Allocate
 *
 *
 *  PARAMETERS: IN index is a selector for the correct
 *					node head.
 *				IN code is a control code that determines if we
 *					are doing a pre-allocation or a step-allocation.
 *
 *  DESCRIPTION: Creates new memory nodes, initializes them, 
 *				 and links them to the appropriate list head. 
 *				 A pre-allocation creates nodes at init time, while
 *				 a step-allocation creates nodes at run time when all
 *  			 existing nodes are unavailable.
 *
 *				 INTERNAL CALL ONLY - caller must own the semaphore
 *				 for the list indicated by index; or, alternatively,
 *				 caller must own the global semaphore. 
 *
 *  RETURNS: true if successful, false otherwise
 *
 ***************************************************************/
Boolean peg_suballocator::_Allocate(Sint32 vector, Sint32 index, Sint32 code)
{
   // no semaphores - this internal routine must only be
   // called by threads owning the list head indicated by index
   SUBALLOC_NODE *temp, *temp2;
   Sint32 i;
   Sint8 *g;
   temp = nodeListHeads[vector][index];
   if (code == PRE_ALLOCATE) // this is a preallocation
      i = preAlloc[vector][index];
   else
      i = step[vector][index]; // this is a step allocation
   for ( ; i > 0; i--)
   {
      temp2 = (SUBALLOC_NODE *)calloc(1, sizeof(SUBALLOC_NODE));
      if (temp2 == NULL)
	 return(false);
      temp2->avail = AVAILABLE;
      memcpy(temp2->guardPre, guard, GUARD_SIZE);
      memcpy(temp2->guardPost, guard, GUARD_SIZE);
//      temp2->allocPtr = malloc( sizeof(void *) + nodeSizes[vector][index] +
//			       (2 * GUARD_SIZE) );
      size_t chunk_size = sizeof(SUBALLOC_NODE **) + ( 2 * GUARD_SIZE ) + nodeSizes[vector][index];
      
      void * ptr = malloc( chunk_size );
      temp2->allocPtr = ptr;
      
      if (temp2->allocPtr == NULL)
      {
	 free(temp2);
	 return(false);
      }
      g = (Sint8 *)temp2->allocPtr;
      *(SUBALLOC_NODE **)g = temp2;
      g += sizeof(SUBALLOC_NODE **);
      g = (Sint8 *)memcpy(g, guard, GUARD_SIZE);
      memcpy(g + nodeSizes[vector][index] + GUARD_SIZE, guard, GUARD_SIZE); 
      // insert new node at the beginning of the list
      INSERT(temp2, temp);
   }
   return(true);
}	

/****************************************************************
 *  DeInitSubAllocator
 *
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION: Locks suballocator and destroys all of its data
 *				 structures. Checks for unfreed nodes, which
 *				 represent memory leaks. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void peg_suballocator::DeInitSubAllocator(void *handle)
{
   Sint32 i, o, waitCode;
   assert(handle != 0);
   _UnfreedNodes(handle); 
   if( handle != (void *) &internal_handle)
      free((void *)handle);

   WAIT_MUTEX(&init_mutex);
   init_count--;
   if (! init_count)
   {
      WAIT_MUTEX(&globalSemHandle, 1000, &waitCode);
      for (o = 0; o < 3; o++)
      {
	 for (i = 0; i < 16; i++)
	 {
	    WAIT_MUTEX(&(semHandles[o][i]), 1000, &waitCode);
	    CLOSE_MUTEX(&(semHandles[o][i]));
	    _DeAllocate(o, i);
	    CLOSE_MUTEX(&(semHandles[o][i]));
	    free(nodeListHeads[o][i]);
	    nodeListHeads[o][i] = NULL;
	 }
      }
      CLOSE_MUTEX(&(globalSemHandle));
      initialized = 0;
   }
   RELEASE_MUTEX(&init_mutex);
   
   return;
}	

/****************************************************************
 *  _DeAllocate
 *
 *
 *  PARAMETERS: IN index is a selector for the correct
 *				list to destroy
 *
 *  DESCRIPTION: INTERNAL CALL ONLY - caller must own the semaphore
 *					for the list head, or, alternatively, the 
 *					global semaphore. 
 *				After calling this function, be certain to close
 *				the semaphore handle corresponding to index.
 *
 *  RETURNS:
 *
 ***************************************************************/
void peg_suballocator::_DeAllocate(Sint32 vector, Sint32 index)
{
   // caller owns semHandles[vector][index]
   SUBALLOC_NODE *temp, *temp2;
   temp = nodeListHeads[vector][index];
   while ( ! IS_EMPTY(temp) )
   {
      temp2 = temp->next;
      _DELETE(temp2);
      free(temp2->allocPtr);
      free(temp2); 
   }
   return;
}

/****************************************************************
 *  GetNode
 *
 *
 *  PARAMETERS:	IN index is a selector for the listHead from which
 *				   we want to get a node.
 *
 *  DESCRIPTION: Allocates a node from the selected list. If the 
 *				list is fully utilized, allocates some new nodes
 *				and returns one of those. 
 *
 *  RETURNS: pointer to an allocated node. the node remains linked
 *		     its list but is marked as not being available. 
 *
 ***************************************************************/
peg_suballocator::SUBALLOC_NODE *peg_suballocator::GetNode(Sint32 vector, Sint32 index)
{
   SUBALLOC_NODE *temp;
   Sint32 waitCode;
   WAIT_MUTEX(&(semHandles[vector][index]), 1000, &waitCode);
   temp = (nodeListHeads[vector][index])->next;
   // if list is empty we will fall through
   while (! IS_HEAD(temp) )
   {
      if (temp->avail == AVAILABLE)
      {
	 temp->avail = NORMAL;
	 // release semHandles[index] 
	 RELEASE_MUTEX(&(semHandles[vector][index]));
	 return(temp);
      }
      temp = temp->next;
   }
   // the list is either empty or fully allocated
   if (! _Allocate(vector, index, STEP_ALLOCATE))
   {
      // release semHandles[index];
      RELEASE_MUTEX(&(semHandles[vector][index]));
      return(NULL);
   }
   // Allocate always links new nodes at the front of the list
   // we can just grab the first node and go
   temp = (nodeListHeads[vector][index])->next;
   temp->avail = NORMAL;
   // release semHandles[vector][index];
   RELEASE_MUTEX(&(semHandles[vector][index]));
   return(temp);
}	

/****************************************************************
 *  GetHugeNode
 *
 *
 *  PARAMETERS:	IN size is the amount of memory requested by the 
 *				caller. 
 *
 *  DESCRIPTION: Allocates a node from the "huge" list. This list is 
 *				 maintained specifically for memory allocations that
 *				 are greater than 32K bytes. If the list is fully
 *				 utilized, will allocate some new nodes and return
 *				 one of those. 
 *
 *  RETURNS: pointer to an allocated node. the node remains linked
 *		     its list but is marked as not being available. 
 *
 ***************************************************************/
peg_suballocator::SUBALLOC_NODE * peg_suballocator::GetHugeNode(Sint32 size)
{
   SUBALLOC_NODE *temp;
   Sint8 *g;
   Sint32 waitCode;
   WAIT_MUTEX(&(semHandles[0x03][0x0f]), 1000, &waitCode);
   // use the last listhead to hold all the huge nodes
   temp = (nodeListHeads[0x03][0x0f])->next;
   // if list is empty we will fall through
   while (! IS_HEAD(temp) )
   {
     hugeNodeLoop:
      if (temp->avail == AVAILABLE)
      {
	 temp->avail = NORMAL;
	 if (temp->allocPtr != NULL)
	    free(temp->allocPtr);
	 temp->allocPtr = calloc(sizeof(SUBALLOC_NODE **) + size + 
				 (2 * GUARD_SIZE), sizeof(Sint8));
	 if (temp->allocPtr != NULL)
	 {
	    g = (Sint8 *)temp->allocPtr;
	    *(SUBALLOC_NODE **)g = temp;
	    g += sizeof(SUBALLOC_NODE **);
	    g = (Sint8 *)memcpy(g, guard, GUARD_SIZE);
	    memcpy(g + size + GUARD_SIZE, guard, GUARD_SIZE); 
	 }
	 // release semHandles[index]
	 RELEASE_MUTEX(&(semHandles[0x03][0x0f]));
	 if (temp->allocPtr != NULL)
	    return(temp);
	 else
	    return(NULL);
      }
      temp = temp->next;
   }
   // the list is either empty or fully allocated
   if (! _Allocate(0x03, 0x0f, STEP_ALLOCATE))
   {
      RELEASE_MUTEX(&(semHandles[0x03][0x0f]));
      return(NULL);
   }
   // Allocate always links new nodes at the front of the list
   temp = (nodeListHeads[0x03][0x0f])->next;
   goto hugeNodeLoop;
   // release semHandles[index];
   assert(0);
   RELEASE_MUTEX(&(semHandles[0x03][0x0f]));
   return(NULL);	  // should never get here
}	

/****************************************************************
 *  PutNode
 *
 *
 *  PARAMETERS:	IN index is a selector for the correct listhead.
 *				IN node is the node that we need to "put" back 
 *				on to the list.
 *
 *  DESCRIPTION: "put" is a misnomer because the node never 
 *				 leaves its list. However, we do unlink the node
 *				 and insert it at the front of the list before marking
 *				 it as being available. We do this to speed up allocation
 *				 of the node next time someone needs it. We want
 *				 to keep all available nodes at the front of the list. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void peg_suballocator::PutNode(Sint32 vector, Sint32 index, SUBALLOC_NODE *node)
{
   Sint32 waitCode;
   WAIT_MUTEX(&(semHandles[vector][index]), 1000, &waitCode);

   // mark the node as available, unlink it, and relink it
   // to the front of the list
   assert(node != NULL);
   // gain ownership of semHandles[index];
   assert((! IS_EMPTY(nodeListHeads[vector][index])));
   _DELETE(node);
   // insert the node at the front of the list - 
   // this will make it faster to get the node
   // next time we need it. 
   INSERT(node, nodeListHeads[vector][index]);
   node->avail = AVAILABLE;
   RELEASE_MUTEX(&(semHandles[vector][index]));
   return;
}	

/****************************************************************
 *  PutHugeNode
 *
 *
 *  PARAMETERS:	IN node is the node that we need to "put" back 
 *				on to the "huge" list.
 *
 *  DESCRIPTION: "put" is a misnomer because the node never 
 *				 leaves its list. However, we do unlink the node
 *				 and insert it at the front of the list before marking
 *				 it as being available. We do this to speed up allocation
 *				 of the node next time someone needs it. We want
 *				 to keep all available nodes at the front of the list. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void peg_suballocator::PutHugeNode(SUBALLOC_NODE *node)
{
   // mark the node as available, unlink it, and relink it
   // to the front of the list
   Sint32 waitCode;
   assert(node != NULL);
   WAIT_MUTEX(&(semHandles[0x03][0x0f]), 1000, &waitCode);
   // gain ownership of semHandles[0x0f];
   assert((! IS_EMPTY(nodeListHeads[0x03][0x0f])));
   _DELETE(node);
   free(node->allocPtr);
   node->allocPtr = NULL;
   // insert the node at the front of the list - 
   // this will make it faster to get the node
   // next time we need it. 
   INSERT(node, nodeListHeads[0x03][0x0f]);
   node->avail = AVAILABLE;
   RELEASE_MUTEX(&(semHandles[0x03][0x0f]));
   return;
}	


/****************************************************************
 *  vs_malloc
 *
 *
 *  PARAMETERS: IN size is the amount of memory requested
 *
 *  DESCRIPTION: initializes the suballocator if neccesary. 
 *				 gets an appropriately sized memory node and
 *				 returns a pointer to the caller. 
 *
 *  RETURNS: pointer to memory available for use by the caller,
 *   		 or NULL. 
 *
 ***************************************************************/

void *peg_suballocator::vs_malloc(size_t size, void *handle, int type, Sint8 *f, Sint32 l)
{
   // we don't need to grab any semaphores, 
   // called routines will do that for us
   SUBALLOC_NODE *temp;
   Sint8 *g;
   assert(size != 0);
   WAIT_MUTEX(&init_mutex);
   if (! initialized)
   {
      RELEASE_MUTEX(&init_mutex);
      if (! InitializeSubAllocator())
	 return(NULL);
   }
   else
   {
      RELEASE_MUTEX(&init_mutex);
   }
   
   if (! (size >> 8))
   {
      temp = GetNode(0, (size >> 4));
   }
   else if (! (size >> 12))
   {
      temp = GetNode(1, (size >> 8));
   }
   else if (! (size >> 16))
   {
      temp = GetNode(2, (size >> 12));
   }
   else
      GetHugeNode(size);
   temp->allocSize = size;
   temp->concurrencyHandle = (void *)handle;
   g = (Sint8 *)temp->allocPtr;
   assert(*(SUBALLOC_NODE **)g == temp);
   g += sizeof(SUBALLOC_NODE **);
   memset(temp->file, 0x00, MAX_PATH_LEN + 1);
   if( f ) 
      strncpy(temp->file, f, MAX_PATH_LEN);
   memset(temp->line, 0x00, MAX_LINE_LEN + 1);
   if( l ) 
      sprintf(temp->line, "%d", l);
   memcpy(g, guard, GUARD_SIZE);
   g += GUARD_SIZE;
   memcpy(g + size, guard, GUARD_SIZE);
   return((void *)g);
}	

/****************************************************************
 *  vs_calloc
 *
 *
 *  PARAMETERS: IN num is the number of memory units requested.
 *				IN size is the size of each memory unit. result
 *				is a contigious area of memory that is 
 *				num * size bytes.
 *
 *  DESCRIPTION: initializes the suballocator if neccesary. 
 *				 gets an appropriately sized memory node, zeros
 *				 caller's memory block and returns a 
 *				 pointer to the caller. 
 *
 *  RETURNS: pointer to zero'ed memory available for use by the 
 *	caller, or NULL. 
 ***************************************************************/
void *peg_suballocator::vs_calloc(size_t num, size_t s, void *handle, int type, Sint8 *f, Sint32 l)
{
   // we don't need to grab any semaphores - 
   // called routines will do that for us
   SUBALLOC_NODE *temp;
   Sint32 size;
   Sint8 *g;
   assert(num != 0);
   assert(s != 0);
   size = num * s;
   if (! initialized)
   {
      if (! InitializeSubAllocator())
	 return(NULL);
   }

   if (! (size >> 8))
   {
      temp = GetNode(0, (size >> 4));
   }
   else if (! (size >> 12))
   {
      temp = GetNode(1, (size >> 8));
   }
   else if (! (size >> 16))
   {
      temp = GetNode(2, (size >> 12));
   }
   else
      temp = GetHugeNode(size);
   assert(temp != NULL);
   temp->allocSize = size;
   temp->concurrencyHandle = (void *)handle;
   g = (Sint8 *)temp->allocPtr;
   assert((*(SUBALLOC_NODE **)g) == temp);
   g += sizeof(SUBALLOC_NODE **);
   memset(temp->file, 0x00, MAX_PATH_LEN + 1);
   strncpy(temp->file, f, MAX_PATH_LEN);
   memset(temp->line, 0x00, MAX_LINE_LEN + 1);
   sprintf(temp->line, "%d", l);
   g += GUARD_SIZE;
   memset(g, 0x00, size);
   return((void *)g);
}
	
/****************************************************************
 *  vs_free
 *
 *
 *  PARAMETERS: IN pointer to memory that the caller wants freed.
 *
 *  DESCRIPTION: marks the node as available. in debug mode, 
 *				 checks for memory under and over -writes. 
 *
 *  RETURNS:
 *
 ***************************************************************/
void peg_suballocator::vs_free(void *m, int type )
{
   // we don't need to grab any semaphores - 
   // called routines will do that for us
   SUBALLOC_NODE *temp;
   Sint8 *g;
   assert(m != NULL);
   g = (Sint8 *)m;
   g -= GUARD_SIZE;	
   temp = *(SUBALLOC_NODE **)(g - sizeof(SUBALLOC_NODE **));
   // check all the guard pages
   // we don't need to own semaphores because we will not be
   // walking the list 
   assert(_CheckGuard(temp));

   if (! (temp->allocSize >> 8))
   {
      PutNode(0, (temp->allocSize >> 4), temp);
   }
   else if (! (temp->allocSize >> 12))
   {
      PutNode(1, (temp->allocSize >> 8), temp);
   }
   else if (! (temp->allocSize >> 16))
   {
      PutNode(2, (temp->allocSize >> 12), temp);
   }
   else
      PutHugeNode(temp);
   return;
}

void *peg_suballocator::vs_realloc(void *pblock, size_t newsize, void *handle, int type, Sint8 *f, Sint32 l)
{
   if (pblock == NULL) {
      return(vs_malloc(newsize, handle, type, f, l));	
   }
   if (newsize == 0) {
      vs_free(pblock);
      return(NULL);
   }

   {
      void *newblock;
      newblock = vs_calloc(newsize, sizeof(char), handle, type, f, l);
      if (newblock != NULL) {
	 SUBALLOC_NODE *temp;
	 Sint8 *g;
	 int copysize;
	 g = (Sint8 *)pblock;
	 g -= GUARD_SIZE;	
	 temp = *(SUBALLOC_NODE **)(g - sizeof(SUBALLOC_NODE **));
	 if(newsize > temp->allocSize)
	    copysize = temp->allocSize;
	 else
	    copysize = newsize;

	 /* get the actual data size of the old block - only copy that much */
	 /* if newsize is smaller than oldsize, only copy newsize */
	 memcpy(newblock, pblock, copysize);
	 vs_free(pblock);
	 return(newblock);
      }
      else
	 return(pblock);
   }
}	
	
Sint8 * peg_suballocator::vs_strdup(const Sint8 *string, void *handle, int type, Sint8 *f, Sint32 l)
{

   Sint8 *memory;

   if (!string)
      return(NULL);
   if ((memory = (Sint8 *)vs_malloc(strlen(string) + 1, handle, type, f, l)))
      return(strcpy(memory,string));
   
   return(NULL);
}

/****************************************************************
 *  _UnfreedNodes
 *
 *
 *  PARAMETERS: IN index is a selector for the correct listhead.
 *
 *  DESCRIPTION:  Checks the list for unfreed nodes. These are nodes
 *				  that are marked as available. Call this routine
 *				  to check for memory leaks.
 *				  INTERNAL call only - caller must own the semaphore
 *				  for the list indicated by index. 
 *
 *  RETURNS: true if there are unfreed nodes on the list, 
 *	false otherwise
 *
 ***************************************************************/
Boolean peg_suballocator::_UnfreedNodes(void * handle)
{
   Sint8 i, y;
   Sint32 waitCode;
   SUBALLOC_NODE *temp;
   Boolean ccode = false;
   SUBALLOC_HANDLE *h = (SUBALLOC_HANDLE *)handle;
   dumpFile = fopen(h->logpath, "wt");

   WAIT_MUTEX(&(globalSemHandle), 1000, &waitCode);

   for (y = 0; y < 3; y++ )
   {
      for (i = 0; i < 16; i++)
      {
	 WAIT_MUTEX(&(semHandles[y][i]), 1000, &waitCode);
	 temp = (nodeListHeads[y][i])->next;
	 // if list is empty we will fall through
	 while (! IS_HEAD(temp) )
	 {
	    if (temp->avail != AVAILABLE  && temp->concurrencyHandle == (void *)handle)
	    {
	       if (dumpFile != NULL)
	       {
		  fprintf(dumpFile, "\nfreeing memory: vector %d index %d %s, %s", 
			  y, i, temp->file, temp->line);
	       }
	       ccode = true;
	       temp->avail = AVAILABLE;
	       if ((temp->allocSize >> 16))
	       {
		  free(temp->allocPtr);
		  temp->allocPtr = NULL;
	       }
	    }
	    temp = temp->next;	
	 }
	 RELEASE_MUTEX(&(semHandles[y][i]));
      }
   }
   if (dumpFile != NULL)
   {
      fclose(dumpFile);
      dumpFile = NULL;
   }
   RELEASE_MUTEX(&(globalSemHandle));
   return(ccode);
}
	
/****************************************************************
 *  _CheckGuard
 *
 *
 *  PARAMETERS: IN node is a node that we want to check for 
 *				under and over -writes. 
 *
 *  DESCRIPTION: checks node for under and over -writes
 *	
 *	NOTE: only available in debug mode. 
 *
 *  RETURNS: true if node is intact, false if node has been 
 *			 stomped upon.
 *
 ***************************************************************/
Boolean peg_suballocator::_CheckGuard(SUBALLOC_NODE *node)
{
   Sint32 ccode;
   Sint8 *g;
   assert(node->avail != AVAILABLE);
   ccode = memcmp(node->guardPre, guard, GUARD_SIZE);
   if (ccode == 0)
   {
      g = (Sint8 *)node->allocPtr + sizeof(SUBALLOC_NODE **);
      ccode = memcmp(g, guard, GUARD_SIZE);
      if (ccode == 0)
      {
	 g += (node->allocSize + GUARD_SIZE);
	 ccode = memcmp(g, guard, GUARD_SIZE);
	 if (ccode == 0)
	 {
	    ccode = memcmp(node->guardPost, guard, GUARD_SIZE);
	    if (ccode == 0)
	       return(true);
	 }
      }
   }
   return(false);
}

void peg_suballocator::_CheckNode(void *m)
{
   // we don't need to grab any semaphores - 
   // called routines will do that for us
   SUBALLOC_NODE *temp;
   Sint8 *g;
   assert(m != NULL);
   g = (Sint8 *)m;
   g -= GUARD_SIZE;	
   temp = *(SUBALLOC_NODE **)(g - sizeof(SUBALLOC_NODE **));
   // check all the guard pages
   // we don't need to own semaphores because we will not be
   // walking the list 
   assert(_CheckGuard(temp));
   return;
}






PEGASUS_NAMESPACE_END
