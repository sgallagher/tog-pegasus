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
// Modified By: 
//	   Rammnath Ravindran (Ramnath.Ravindran@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "suballoc.h"
#include <Pegasus/Common/Tracer.h>
#include <new.h>
PEGASUS_NAMESPACE_BEGIN
//#define PEGASUS_DEBUG_MEMORY
peg_suballocator *peg_suballocator::_suballoc_instance = 0;

PEGASUS_SUBALLOC_LINKAGE peg_suballocator *peg_suballocator::get_instance(void)
{
   if(peg_suballocator::_suballoc_instance == 0)
   {
#if defined(PEGASUS_DEBUG_MEMORY)
      PEGASUS_STD(cout) << "Initializing Debug Heap" << PEGASUS_STD(endl);
       
#endif
      void *buffer = calloc(sizeof(peg_suballocator), sizeof(char));
      peg_suballocator::_suballoc_instance = new(buffer) peg_suballocator(true);
   }
   return peg_suballocator::_suballoc_instance;
}

#if defined(PEGASUS_DEBUG_MEMORY)
PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t size, 
					      void * handle,
					      int type,
					      const Sint8 *classname, 
					      Sint8 *file,
					      Uint32 line)
{
   return peg_suballocator::get_instance()->vs_malloc(size, 
 				       ((handle == NULL) ? &(peg_suballocator::get_instance()->get_handle()) : handle),
				       type, 
				       classname, 
				       file,  
				       line) ;
}

PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void * dead,
					   void * handle,
					   int type, 
					   Sint8 *classname, 
					   Sint8 * file, 
					   Uint32 line) 
{
   if ( dead == 0 )
      return;
   
   peg_suballocator::get_instance()->vs_free(dead,  				       
			      ((handle == NULL) ? &(peg_suballocator::get_instance()->get_handle()) : handle),
			      type, 
			      classname, 
			      file, 
			      line); 
}

#else

PEGASUS_SUBALLOC_LINKAGE void * pegasus_alloc(size_t size)
{
   return peg_suballocator::get_instance()->vs_malloc(size);
}

PEGASUS_SUBALLOC_LINKAGE void pegasus_free(void *dead)
{
   peg_suballocator::get_instance()->vs_free(dead);
}

#endif

PEGASUS_NAMESPACE_END
PEGASUS_USING_STD;

#if !defined(PEGASUS_PLATFORM_HPUX_ACC) && !defined(PEGASUS_PLATFORM_LINUX_IA64_GNU) && !defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX)
PEGASUS_USING_PEGASUS;
void * operator new(size_t size) throw (PEGASUS_STD(bad_alloc))
{

   if( size == 0 )
      size = 1;
   void *p;
   while(1)
   {
#if defined(PEGASUS_DEBUG_MEMORY)
      p = peg_suballocator::get_instance()->vs_malloc(size, 
				       &(peg_suballocator::get_instance()->get_handle()),
				       NORMAL, 
				       "BUILTIN NEW", 
				       __FILE__, __LINE__) ;

      
#else 
      p = peg_suballocator::get_instance()->vs_malloc(size); 
#endif
      if( p )
	 return p;
      new_handler global = set_new_handler(0);
      set_new_handler(global);
      if( global) 
	 (*global)();
      else
	 throw PEGASUS_STD(bad_alloc());
   }
}

#ifdef PEGASUS_PLATFORM_HPUX_ACC
void operator delete(void *dead) throw()
#else
void operator delete(void *dead) 
#endif
{
   if( dead == 0 )
      return;
#if defined(PEGASUS_DEBUG_MEMORY)
   peg_suballocator::get_instance()->vs_free(dead,  
			      &(peg_suballocator::get_instance()->get_handle()), 
			      NORMAL, 
			      "internal", 
			      __FILE__, 
			      __LINE__);

#else
   peg_suballocator::get_instance()->vs_free(dead);
#endif 
   return;
}


void * operator new[] (size_t size) throw (PEGASUS_STD(bad_alloc))
{

   
   if( size == 0 )
      size = 1;
    void *p;  
   
   while(1)
   {
#if defined(PEGASUS_DEBUG_MEMORY)
      p = peg_suballocator::get_instance()->vs_malloc(size, 
				       &(peg_suballocator::get_instance()->get_handle()), 
				       ARRAY, 
				       "BUILTIN ARRAY NEW", 
				       __FILE__, __LINE__) ;

#else
      p = peg_suballocator::get_instance()->vs_malloc(size);
#endif
      if( p )
	 return p;
      new_handler global = set_new_handler(0);
      set_new_handler(global);
      if( global)  
	 (*global)();
      else 
	 throw PEGASUS_STD(bad_alloc());
   }
}

#ifdef PEGASUS_PLATFORM_HPUX_ACC
void operator delete[] (void *dead) throw()
#else
void operator delete[] (void *dead) 
#endif
{
   if( dead == 0 )
      return;
#if defined(PEGASUS_DEBUG_MEMORY)
   peg_suballocator::get_instance()->vs_free(dead, 
			      &(peg_suballocator::get_instance()->get_handle()), 
			      ARRAY, 
			      "internal",
			      __FILE__, 
			      __LINE__);

#else
   peg_suballocator::get_instance()->vs_free(dead);
#endif
   return;
}
#endif // PEGASUS_PLATFORM_HPUX_ACC

PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_DEBUG_MEMORY)
const Uint8 peg_suballocator::guard[] = {0x01, 0x02, 0x03, 0x04, 0x05, 
					 0x06, 0x07, 0x08, 0x09, 0x09, 
					 0x08, 0x07, 0x06, 0x05, 0x04, 
					 0x03};
const Uint8 peg_suballocator::alloc_pattern = 0xaa;
const Uint8 peg_suballocator::delete_pattern = 0xee;
#endif

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
      0xd000, 0xe000, 0xf000, 
      2 * sizeof(void *)
   }
};	


// prototypes that begin with an underscore do not attempt to 
// gain ownership of semaphores and can be safely called
// by a process that owns a semaphore


const Uint32 peg_suballocator::preAlloc[3][16] = 
{
   {20, 20, 20, 20, 20, 20, 20, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
   {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   {	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};	

const Uint32 peg_suballocator::step[3][16] = 
{
   {20, 20, 20, 20, 20, 20, 20, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
   {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 },
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

peg_suballocator::peg_suballocator(void)
   : initialized(0),
     debug_mode(true), 
     abort_on_error(true), 
     check_for_leaks(true),
     internal_handle("internal_suballoc_log")
     
{
   memcpy(internal_handle.classname, "internal\0", 9);
   InitializeSubAllocator();
   return;
}

peg_suballocator::peg_suballocator(Boolean mode)
   : initialized(0),
     debug_mode(mode), 
     abort_on_error(true), 
     check_for_leaks(true),
     internal_handle("internal_suballoc_log")
     
{ 
   memcpy(internal_handle.classname, "internal\0", 9);
   InitializeSubAllocator();
   return;
}

peg_suballocator::~peg_suballocator(void)
{ 
   DeInitSubAllocator(&internal_handle);
}

peg_suballocator::SUBALLOC_HANDLE *peg_suballocator::InitializeProcessHeap(Sint8 *f)
{
   return new SUBALLOC_HANDLE(f);
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
Boolean peg_suballocator::InitializeSubAllocator(void)
{
   SUBALLOC_NODE *temp;
   Sint32 i, o;
   Boolean ccode = true;
   Sint32 waitCode;
   
   if(initialized)
      return true;
   
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
	    temp->flags |= (IS_HEAD_NODE );
#if defined(PEGASUS_DEBUG_MEMORY)
	    memcpy(temp->guardPre, guard, GUARD_SIZE);
#endif
	    if (preAlloc[o][i])
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
#if defined(PEGASUS_DEBUG_MEMORY)
   size_t chunk_size = (i * (sizeof(SUBALLOC_NODE) + GUARD_SIZE + nodeSizes[vector][index]));
#else
   size_t chunk_size = (i * (sizeof(SUBALLOC_NODE) + nodeSizes[vector][index]));
#endif 
   
   temp2 = (SUBALLOC_NODE *)calloc(chunk_size, sizeof(char));
   if(temp2 == NULL)
      return false;
   for ( ; i > 0; i--)
   {
      temp2->flags |= AVAIL;
      temp2->concurrencyHandle = &internal_handle;
      
#if defined(PEGASUS_DEBUG_MEMORY)
      memcpy(temp2->guardPre, guard, GUARD_SIZE);
#endif
      g = (Sint8 *)temp2;
      g+= sizeof(SUBALLOC_NODE);
      g+= nodeSizes[vector][index];
#if defined(PEGASUS_DEBUG_MEMORY)
      memcpy(g, guard, GUARD_SIZE);
      g += GUARD_SIZE;
#endif
      INSERT(temp2, temp);
      temp2 = (SUBALLOC_NODE *)g;
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
//   _UnfreedNodes(handle); 
   if( handle != (void *) &internal_handle)
      free((void *)handle);
   
   init_count--;
   if (! init_count)
   {
      initialized = 0;
      WAIT_MUTEX(&globalSemHandle, 1000, &waitCode);
       for (o = 0; o < 3; o++)
      {
	 for (i = 0; i < 16; i++)
	 {
	    WAIT_MUTEX(&(semHandles[o][i]), 1000, &waitCode);
	    CLOSE_MUTEX(&(semHandles[o][i]));
	    _DeAllocate(o, i);
	    free(nodeListHeads[o][i]);
	    nodeListHeads[o][i] = NULL;
	 }
      }
      CLOSE_MUTEX(&(globalSemHandle));
      initialized = 0;
   }
   
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
   if(initialized == 0 )
   {
      InitializeSubAllocator();
   }
   WAIT_MUTEX(&(semHandles[vector][index]), 1000, &waitCode);
   temp = (nodeListHeads[vector][index])->next;
   // if list is empty we will fall through
   
   while (! IS_HEAD(temp) )
   {
      if ((temp->flags & AVAIL) && (false == IS_HEAD(temp)))
      {
	 temp->flags &= ~(AVAIL) ;
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
   temp->flags &= ~(AVAIL);
   if( check_for_leaks == true )
      temp->flags |= CHECK_LEAK;
   
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
   Sint32 waitCode;
   if(initialized == 0 )
   {
      InitializeSubAllocator();
   }
   

   WAIT_MUTEX(&(semHandles[2][15]), 1000, &waitCode);
   temp = (nodeListHeads[2][15])->next;
   while (! IS_HEAD(temp) )
   {
      if ((temp->flags & AVAIL) && (false == IS_HEAD(temp)))
      {
	 if(temp->nodeSize >= (Uint32)size)
	 {
	    temp->flags &= ~(AVAIL) ;
	    temp->allocSize = size;
	    break;
	 }
      }
      temp = temp->next;
   }

   if( IS_HEAD(temp))
   {
#if defined(PEGASUS_DEBUG_MEMORY)
      size_t chunk_size = (sizeof(SUBALLOC_NODE) + GUARD_SIZE + size);
#else
      size_t chunk_size = (sizeof(SUBALLOC_NODE) + size);
#endif 
      temp = (SUBALLOC_NODE *)calloc(chunk_size, sizeof(char));
      if(temp == NULL)
      {
	 RELEASE_MUTEX(&(semHandles[2][15]));
	 return temp;
      }
      
      temp->allocSize = temp->nodeSize = size;
      INSERT(temp, nodeListHeads[2][15]);
   }
   temp->concurrencyHandle = &internal_handle;
#if defined(PEGASUS_DEBUG_MEMORY)
   memcpy(temp->guardPre, guard, GUARD_SIZE);
#endif

   RELEASE_MUTEX(&(semHandles[2][15]));
   return(temp);
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
   // delete insert the node at the front of the list - 
   // this will make it faster to get the node
   _DELETE(node);
   node->concurrencyHandle = &internal_handle;
   node->flags |= AVAIL;
   INSERT(node, nodeListHeads[vector][index]);
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
   PutNode(2, 15, node);
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

#if defined(PEGASUS_DEBUG_MEMORY)
void *peg_suballocator::vs_malloc(size_t size, void *handle, int type, const Sint8 *classname, const Sint8 *f, Uint32 l)
{
   // we don't need to grab any semaphores, 
   // called routines will do that for us
   SUBALLOC_NODE *temp;
   Sint8 *g;
   
   assert(size != 0);
     
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
   {
      temp = GetHugeNode(size);
   }
   
   temp->allocSize = size;
   temp->concurrencyHandle = (void *)handle;
   if(type == ARRAY)
      temp->flags |= ARRAY_NODE;
   if(classname)
      strncpy(temp->classname, classname, MAX_CLASS_LEN);
   else
      temp->classname[0] = 0x00;
   
   if(f)
      strncpy(temp->file, f, MAX_PATH_LEN);
   else 
   temp->file[0] = 0x00;
   
   temp->line = l;

   g = (Sint8 *)temp;
   g += sizeof(SUBALLOC_NODE);
   memcpy(g + size, guard, GUARD_SIZE);
   return((void *)g);
}	
#else
void *peg_suballocator::vs_malloc(size_t size)
{
   SUBALLOC_NODE *temp;
   Sint8 *g;
   assert(size != 0);
     
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
   temp->allocSize = size;
   temp->concurrencyHandle = (void *)&internal_handle;
   g = (Sint8 *)temp;
   g += sizeof(SUBALLOC_NODE);
   return((void *)g);
}
#endif


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

#if defined(PEGASUS_DEBUG_MEMORY)
void *peg_suballocator::vs_calloc(size_t num, size_t s, void *handle, int type, Sint8 *f, Uint32 l)
{
   void *g = vs_malloc((unsigned)num * s, handle, type, "internal", f, l);
   memset(g, 0x00, num * s);
   return(g);
}
	
#else

void *peg_suballocator::vs_calloc(size_t num, size_t s)
{
   void *g = vs_malloc((unsigned)num * s);
   memset(g, 0x00, num * s);
   return(g);
}


#endif

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

#if defined(PEGASUS_DEBUG_MEMORY)
void peg_suballocator::vs_free(void *m, 
			       void *handle,
			       int type , 
			       Sint8 *classname, 
			       Sint8* file, 
			       Uint32 line)
{
   // we don't need to grab any semaphores - 
   // called routines will do that for us
   assert( m != NULL);
   
   SUBALLOC_NODE *temp = _CheckNode(m, type, file, line);
   memset(temp->d_classname, 0x00, MAX_CLASS_LEN + 1);
   strncpy(temp->d_classname, classname, MAX_CLASS_LEN);
   memset(temp->d_file, 0x00, MAX_PATH_LEN + 1);
   strncpy(temp->d_file, file, MAX_PATH_LEN);
   temp->d_line = line;
   
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

#else
void peg_suballocator::vs_free(void *m)
{
   // we don't need to grab any semaphores - 
   // called routines will do that for us
   assert( m != 0 );
   SUBALLOC_NODE *temp = (SUBALLOC_NODE *)m;
   temp--;
   
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

#endif


#if defined(PEGASUS_DEBUG_MEMORY)

void *peg_suballocator::vs_realloc(void *pblock, size_t newsize, void *handle, int type, Sint8 *f, Uint32 l)
{
   if (pblock == NULL) {
      return(vs_malloc(newsize, handle, type, 0, f, l));	
   }
   if (newsize == 0) {
      vs_free(pblock, handle, type, "INTERNAL", f, l);
      return(NULL);
   }

   {
      void *newblock;
      newblock = vs_calloc(newsize, sizeof(char), handle, type, f, l);
      if (newblock != NULL) {
	 SUBALLOC_NODE *temp;
	 int copysize;
	 temp = (SUBALLOC_NODE *)pblock;
	 temp--;
	 if(newsize > temp->allocSize)
	    copysize = temp->allocSize;
	 else
	    copysize = newsize;

	 /* get the actual data size of the old block - only copy that much */
	 /* if newsize is smaller than oldsize, only copy newsize */
	 memcpy(newblock, pblock, copysize);
	 vs_free(pblock, handle, type, "INTERNAL", f, l);
	 return(newblock);
      }
      else
	 return(pblock);
   }
}	

#else

void *peg_suballocator::vs_realloc(void *pblock, size_t newsize)
{
   if (pblock == NULL) {
      return(vs_malloc(newsize));	
   }
   if (newsize == 0) {
      vs_free(pblock);
      return(NULL);
   }

   {
      void *newblock;
      newblock = vs_calloc(newsize, sizeof(char));
      if (newblock != NULL) {
	 SUBALLOC_NODE *temp;
	 int copysize;
	 temp = (SUBALLOC_NODE *)pblock;
	 temp--;
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

#endif
	

#if defined(PEGASUS_DEBUG_MEMORY)
Sint8 * peg_suballocator::vs_strdup(const Sint8 *string, void *handle, int type, Sint8 *f, Uint32 l)
{

   Sint8 *memory;

   if (!string)
      return(NULL);
   if ((memory = (Sint8 *)vs_malloc(strlen(string) + 1, handle, type, 0, f, l)))
      return(strcpy(memory,string));
   
   return(NULL);
}

#else

Sint8 * peg_suballocator::vs_strdup(const Sint8 *string)
{

   Sint8 *memory;

   if (!string)
      return(NULL);
   if ((memory = (Sint8 *)vs_malloc(strlen(string) + 1)))
      return(strcpy(memory,string));
   return(NULL);
}
#endif


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
#if defined(PEGASUS_DEBUG_MEMORY)
Boolean peg_suballocator::_UnfreedNodes(void * handle)
{
   Sint8 i, y;   
   Sint32 waitCode;
   SUBALLOC_NODE *temp;
   Boolean ccode = false;
   SUBALLOC_HANDLE *h = (SUBALLOC_HANDLE *)handle;
   for (y = 0; y < 3; y++ )
   {
      for (i = 0; i < 16; i++)
      {
	 WAIT_MUTEX(&(semHandles[y][i]), 1000, &waitCode);
	 temp = (nodeListHeads[y][i])->next;
	 // if list is empty we will fall through
	 while (! IS_HEAD(temp) )
	 {
	    if (!(temp->flags & AVAIL) && temp->concurrencyHandle == handle)
	    {
	       if( temp->flags & CHECK_LEAK)
	       {
		  h = (SUBALLOC_HANDLE *)temp->concurrencyHandle;
		  RELEASE_MUTEX(&(semHandles[y][i]));
		  
		  Tracer::trace(__FILE__, __LINE__, TRC_MEMORY, Tracer::LEVEL2, 
				"Memory Leak: %d bytes class %s allocated memory in source file %s at " \
				"line %d", temp->allocSize, temp->classname, temp->file, temp->line);
		  WAIT_MUTEX(&(semHandles[y][i]), 1000, &waitCode);
		  ccode = true;
	       }
	    }
	    temp = temp->next;	
	 }
	 RELEASE_MUTEX(&(semHandles[y][i]));
      }
   }
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

   ccode = memcmp(node->guardPre, guard, GUARD_SIZE);
   if (ccode == 0)
   {
      Sint8 *g = (Sint8 *)node;
      g += sizeof(SUBALLOC_NODE);
      g += node->allocSize;
      ccode = memcmp(g, guard, GUARD_SIZE);
      if (ccode == 0)
      { 
	 return(true); 
      }
   }
   return(false);
}

Uint32 peg_suballocator::CheckMemory(void *m)
{
   return _CheckNode(m);
}

Uint32 peg_suballocator::_CheckNode(void *m)
{
   assert(m != NULL);
   SUBALLOC_NODE *temp = (SUBALLOC_NODE *)m;
   temp--;
   
   if(temp->flags & AVAIL)
      return ALREADY_DELETED;
   if(false == _CheckGuard(temp))
      return OVERWRITE;
   return 0;
}

peg_suballocator::SUBALLOC_NODE *peg_suballocator::_CheckNode(void *m, 
							      int type,  
							      Sint8 *file, 
							      Uint32 line)
{
   assert(m != NULL);
   SUBALLOC_NODE *temp = (SUBALLOC_NODE *)m;
   temp--;
   if( (temp->flags & AVAIL ) )
   {
      Tracer::trace(file, line, TRC_MEMORY, Tracer::LEVEL2, 
		    "Doubly freed memory, already deleted by class %s at " \
		    "source file %s line number %d; this deletion is from " \
		    "source file %s line number %d",
		    temp->d_classname, temp->d_file, temp->d_line, file, line);
      temp->flags |= CHECK_FAILED;
      if( abort_on_error)
	 abort(); 
   }
/******
   if( type == ARRAY )
   {
      if( false == IS_ARRAY(temp))
      {
	 Tracer::trace(file, line, TRC_MEMORY, Tracer::LEVEL2,
		       "Array delete called with non-array object at " \
		       "source file %s line number %d. Object was originally allocated " \
		       " by class %s in source file %s at line number %d",
		       file, line, temp->classname, temp->file, temp->line);
	 temp->flags |= CHECK_FAILED;
	 if( abort_on_error)
	    abort();
      }
   }
   else if( true == IS_ARRAY(temp))
   {
      Tracer::trace(file, line, TRC_MEMORY, Tracer::LEVEL2,
		    "Normal delete called with array object at " \
		    "source file %s line number %d. Object was originally allocated " \
		    " by class %s in source file %s at line number %d",
		    file, line, temp->classname, temp->file, temp->line);
      temp->flags |= CHECK_FAILED;
      if( abort_on_error)
	 abort();
   }

*****/
   if(false == _CheckGuard(temp))
   {
      Tracer::trace(file, line, TRC_MEMORY, Tracer::LEVEL2,
		    "Memory overwritten. Allocated by %s in source file %s at line number %d",
		    temp->classname, temp->file, temp->line);
      temp->flags |= CHECK_FAILED;
      if( abort_on_error)
	 abort();
   }
   return temp;
}



void peg_suballocator::PrintNodeInfo(void *m, Sint8 *output_file, Sint8 *file, Uint32 line)
{
   assert(m != NULL);
   SUBALLOC_NODE *temp = (SUBALLOC_NODE *)m;
   temp--;
   FILE *output = fopen(output_file, "a+");
   if(output != NULL)
   {
      fprintf(output, "********* MEMORY NODE RECORD *********\n");
      fprintf(output, "node address: %p\n", temp);
      
      fprintf(output, "allocated at %s line %d\n", temp->file, temp->line);
      fprintf(output, "classname (may not be available): %s\n", temp->classname);
      fprintf(output, "alloc size: %d; node size: %d\n", temp->allocSize, temp->nodeSize);
      
      if(temp->flags & AVAIL)
      {
	 fprintf(output, 
		 "Doubly freed memory, already deleted by class %s at " \
		 "source file %s line number %d; this deletion is from " \
		 "source file %s line number %d\n",
		 temp->d_classname, temp->d_file, temp->d_line, file, line);
      }
      if( true == IS_ARRAY(temp))
      {
	 fprintf(output, "Array node\n");
      }
      if( false == _CheckGuard(temp))
      {
	 fprintf(output, "Memory overwritten!\n");
      }
      
      fclose(output);
   }
}


#endif // defined PEGASUS_DEBUG_MEMORY 
PEGASUS_NAMESPACE_END
