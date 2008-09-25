/*
//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////
*/
#define PEGASUS_INSIDE_ALLOC_CPP
#include "Alloc.h"
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

//==============================================================================
//
// pegasus_malloc_notifier()
//
//     Invoked everytime malloc is invoked.
//
//==============================================================================

void (*pegasus_malloc_notifier)(size_t total);

//==============================================================================
//
// struct Block
//
//==============================================================================

#define BLOCK_MAGIC 0xF00DFACE

struct Block
{
    unsigned int magic;
    unsigned int size;
    Block* next;
};

/* total bytes allocated */
static size_t _total = 0;

//==============================================================================
//
// struct Set
//
//==============================================================================

#define SET_NUM_CHAINS (64 * 1024)

typedef struct _Set
{
    Block* chains[SET_NUM_CHAINS];
}
Set;

static Set _set;
static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

int Set_find(Set* self, Block* block)
{
    Block* p;
    size_t h = (unsigned long)block % SET_NUM_CHAINS;

    assert(h < SET_NUM_CHAINS);

    for (p = self->chains[h]; p; p = p->next)
    {
        if (p == block)
            return 0;
    }

    /* Not found */
    return -1;
}

int Set_insert(Set* self, Block* block)
{
    Block* p;
    size_t h = (unsigned long)block % SET_NUM_CHAINS;

    assert(h < SET_NUM_CHAINS);

    for (p = self->chains[h]; p; p = p->next)
    {
        if (p == block)
        {
            /* duplicate */
            return -1;
        }
    }

    block->next = self->chains[h];
    self->chains[h] = block;
    return 0;
}

int Set_remove(Set* self, Block* block)
{
    Block* p;
    Block* prev = 0;
    size_t h = (unsigned long)block % SET_NUM_CHAINS;

    assert(h < SET_NUM_CHAINS);

    for (p = self->chains[h]; p; p = p->next)
    {
        if (p == block)
        {
            if (prev)
                prev->next = p->next;
            else
                self->chains[h] = p->next;

            return 0;
        }

        prev = p;
    }

    /* Not found */
    return -1;
}

//==============================================================================
//
// pegasus_malloc()
//
//==============================================================================

extern "C" void* pegasus_malloc(size_t size)
{
    Block* block = (Block*)malloc(sizeof(Block) + size);

    if (!block)
        return NULL;

    block->size = size;
    block->magic = BLOCK_MAGIC;

    pthread_mutex_lock(&_mutex);
    {
        Set_insert(&_set, block);

        _total += size;
    }
    pthread_mutex_unlock(&_mutex);

    if (pegasus_malloc_notifier)
        (*pegasus_malloc_notifier)(_total);

    return block + 1;
}

//==============================================================================
//
// pegasus_realloc()
//
//==============================================================================

extern "C" void* pegasus_realloc(void* ptr, size_t size)
{
    if (!ptr)
        return pegasus_malloc(size);

    Block* block = (Block*)ptr - 1;

    pthread_mutex_lock(&_mutex);
    {
        if (Set_remove(&_set, block) != 0)
        {
            // Not allocated by pegasus_realloc() or pegasus_malloc()
            pthread_mutex_unlock(&_mutex);
            return realloc(ptr, size);
        }

        _total -= block->size;
        _total += size;
    }
    pthread_mutex_unlock(&_mutex);

    if (pegasus_malloc_notifier)
        (*pegasus_malloc_notifier)(_total);

    assert(block->magic == BLOCK_MAGIC);

    if (!(block = (Block*)realloc(block, sizeof(Block) + size)))
    {
        return NULL;
    }

    block->size = size;

    pthread_mutex_lock(&_mutex);
    {
        if (Set_insert(&_set, block) != 0)
        {
            assert(0);
        }
    }
    pthread_mutex_unlock(&_mutex);

    return block + 1;
}

//==============================================================================
//
// pegasus_free()
//
//==============================================================================

extern "C" void pegasus_free(void* ptr)
{
    if (!ptr)
        return;

    Block* block = (Block*)ptr - 1;

    pthread_mutex_lock(&_mutex);
    {
        if (Set_remove(&_set, block) != 0)
        {
            // Not allocated by pegasus_realloc() or pegasus_malloc()
            pthread_mutex_unlock(&_mutex);
            free(ptr);
            return;
        }

        _total -= block->size;
    }
    pthread_mutex_unlock(&_mutex);

    if (pegasus_malloc_notifier)
        (*pegasus_malloc_notifier)(_total);


    assert(block->magic == BLOCK_MAGIC);
    free(block);
}
