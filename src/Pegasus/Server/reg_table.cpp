//%///////////-*-c++-*-/////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Copyright (c) 2003 Hewlett-Packard Company, IBM,The Open Group
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
//%////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Sharable.h>
#include "reg_table.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const DynamicRoutingTable _internal_routing_table;

class reg_table_record
{
   private:
      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       const MessageQueueService * destination_service);

      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       const Array<Uint8> & extended_type, 
		       Uint32 flags, 
		       const Array<Uint8> & extended_flags,
		       const MessageQueueService * destination_service);
      
      reg_table_record(const reg_table_record & );
      reg_table_record & operator =(const reg_table_record & );
      
      CIMName class_name;
      CIMNamespaceName namespace_name;
      Uint32 type;
      Array<Uint8> extended_key;
      Uint32 flags;
      Array<Uint8> extended_flags;
      MessageQueueService *service;
      
      friend class reg_table_rep;
      friend class DynamicRoutingTable;
};


reg_table_record::reg_table_record(
   const CIMName &_name, 
   const CIMNamespaceName &_ns, 
   Uint32 _type, 
   const MessageQueueService * _svce)
   : class_name(_name),
     namespace_name(_ns),
     type(_type),
     extended_key(),
     flags(),
     extended_flags(),
     service(const_cast<MessageQueueService *>(_svce))
{

}


reg_table_record::reg_table_record(
   const CIMName &_name, 
   const CIMNamespaceName &_ns, 
   Uint32 _type, 
   const Array<Uint8> & _type_key, 
   Uint32 _flags, 
   const Array<Uint8> & _ext_flags,
   const MessageQueueService * _svce)
   : class_name(_name),
     namespace_name(_ns),
     type(_type),
     extended_key(_type_key),
     flags(_flags),
     extended_flags(_ext_flags),
     service(const_cast<MessageQueueService *>(_svce))
{

}

reg_table_record::reg_table_record(const reg_table_record & rtr)
{
   class_name = rtr.class_name;
   namespace_name = rtr.namespace_name;
   type = rtr.type;
   extended_key = rtr.extended_key;
   flags = rtr.flags;
   extended_flags = rtr.extended_flags;
   service = rtr.service;
}

reg_table_record & 
reg_table_record::operator =(const reg_table_record & other)
{
      if(this != &other)
      {
	 class_name = other.class_name;
	 namespace_name = other.namespace_name;
	 type = other.type;
	 extended_key = other.extended_key;
	 flags = other.flags;
	 extended_flags = other.extended_flags;
	 service = other.service;
      }
      return *this;
}

// instead of concatenating class, namespace, type into a single key, use a 3-level set-associative cache 
// implemented with hash tables for namespace, class, and type. Because key composition is not 
// necessary it should be faster than a fully associative cache. 

typedef HashTable<String,  struct reg_table_record *, EqualFunc<String>, HashFunc<String> > routing_table;

typedef HashTable<Uint32, routing_table *, EqualFunc<Uint32>, HashFunc<Uint32> > type_table;

typedef HashTable<String, type_table *, EqualFunc<String>, HashFunc<String> > namespace_table;

class reg_table_rep : public Sharable
{

   private:

      // store a record 
      void _insert(const reg_table_record & rec);

      // retrieve a pointer to the stored record
      const reg_table_record * find(const reg_table_record &rec);
      void find(const reg_table_record & rec, Array<reg_table_record *> *results);

      // remove the record and retrieve a pointer to it 
      reg_table_record *release(const reg_table_record &rec);
      void release(const reg_table_record & rec, Array<reg_table_record *> *results);
      
      // remove and destroy a record or records
      void destroy(const reg_table_record & rec);
      void destroy_all(const reg_table_record & rec);


      reg_table_rep(void) ;
      ~reg_table_rep(void) ;

      static const Uint32 FIND;
      static const Uint32 REMOVE;
      static const Uint32 MULTIPLE;
      static const Uint32 DESTROY;
      static const Uint32 EXTENDED;
      
      reg_table_record * _find(const reg_table_record & rec, 
			       Uint32 flags, 
			       Array<reg_table_record *> *arr_ptr = 0);

      // "wildcard" routines that use iterators
      // ns.isNull() == true means enumerate for all name spaces
      // cls.isNull() == true means enumerate for all classes
      // type == 0xffffffff means enumerate for all types
      void _enumerate(const reg_table_record & rec,
		      Uint32 flags, 
		      Array<reg_table_record *> *arr_ptr = 0);
      
      namespace_table _table;
      Mutex _mutex;

      friend class DynamicRoutingTable;
};

const Uint32 reg_table_rep::FIND =         0x00000001;
const Uint32 reg_table_rep::REMOVE =       0x00000002;
const Uint32 reg_table_rep::MULTIPLE =     0x00000004;
const Uint32 reg_table_rep::DESTROY =      0x00000008;
const Uint32 reg_table_rep::EXTENDED =     0x00000010;


// insert optimized for simplicity, not speed 
void reg_table_rep::_insert(const reg_table_record &rec)
{
   // ipc synchronization 
   auto_mutex monitor(&_mutex);
   
   type_table *tt;
   if(false ==  _table.lookup(rec.namespace_name, tt))
   {
      type_table *temp = new type_table();
      _table.insert(rec.namespace_name.getString(), temp);
   }
   
   routing_table *rt;
   
   if(false == tt->lookup(rec.type, rt))
   {
      routing_table *temp = new routing_table();
      tt->insert(rec.type, temp);
      tt->lookup(rec.type, rt);
   }
   
   rt->insert(rec.class_name.getString(), new reg_table_record(rec));
}

const reg_table_record *reg_table_rep::find(const reg_table_record & rec)
{
   return _find(rec, FIND);
}

void 
reg_table_rep::find(const reg_table_record & rec, 
		    Array<reg_table_record *> *results)
{
   _find(rec, (FIND | MULTIPLE), results);
}


reg_table_record *
reg_table_rep::release(const reg_table_record &rec)
{
   return _find(rec, (FIND | REMOVE) );
}


void 
reg_table_rep::release(const reg_table_record & rec, 
		       Array<reg_table_record *> *results)
{
   _find(rec, 
	 (FIND | MULTIPLE | REMOVE ), results);
}

void 
reg_table_rep::destroy(const reg_table_record & rec)
{
   _find(rec, (FIND | REMOVE | DESTROY));
}

void 
reg_table_rep::destroy_all(const reg_table_record & rec)
{
   _find(rec, (FIND | REMOVE | DESTROY | MULTIPLE ));
}



reg_table_record * 
reg_table_rep::_find(const reg_table_record &rec, 
		     Uint32 flags, 
		     Array<reg_table_record *> *arr_ptr)
{
   // ipc synchronization 
   auto_mutex monitor(&_mutex);

   if(flags & MULTIPLE)
   {
      
      _enumerate(rec,
		 flags, 
		 arr_ptr);
      return 0;
   }
   else 
   {
      type_table *tt;
      // find the type entry
      if(true == _table.lookup(rec.namespace_name.getString(), tt))
      {
	 routing_table *rt;
	 if(true == tt->lookup(rec.type, rt))
	 {
	    reg_table_record *record ;
	    while(true == rt->lookup(rec.class_name.getString(), record))
	    {
	       if(flags & EXTENDED)
	       {
		  if(rec.extended_key.size() != record->extended_key.size())
		     continue;
		  for(Uint32 i = 0; i < rec.extended_key.size(); i++)
		  {
		     if(rec.extended_key[i] == record->extended_key[i])
			;
		     else
			continue;
		  }
		  
	       }
	       
	       if(flags & REMOVE || flags & DESTROY)
	       {
		  rt->remove(rec.class_name.getString());
		  if( flags & DESTROY)
		  {
		     delete record;
		     record = 0;
		  }
	       }
	       return record;
	    }
	 }
      }
   }
   return 0;
}



// do not call directly - does not lock the reg table mutext !!
void 
reg_table_rep::_enumerate(const reg_table_record & rec,
			  Uint32 flags, 
			  Array<reg_table_record *> *results)
{
   if(flags & MULTIPLE)
   {
      if(results == 0 && ! (flags & DESTROY))
	 return;
      
      for(namespace_table::Iterator i = _table.start(); i; i++)
      {
	 // null namespace_name is a wildcard
	 if(false == rec.namespace_name.isNull())
	 {
	    if(rec.namespace_name.getString() != i.key())
	       continue;
	 }
      
	 for(type_table::Iterator y = i.value()->start(); y; y++)
	 {
	    // type of -1 is a wildcard
	    if(rec.type != 0xffffffffff)
	    {
	       if(rec.type != y.key())
		  continue;
	    }
	 
	    for(routing_table::Iterator x = y.value()->start(); x; x++)
	    {
	       // null class_name is a wildcard
	       if(false == rec.class_name.isNull())
	       {
		  if(rec.class_name != x.value()->class_name)
		     continue;
	       }
	       reg_table_record *tmp = x.value();
	       if(flags & REMOVE || flags & DESTROY)
	       {
		  y.value()->remove(x.key());
		  if(flags & DESTROY)
		  {
		     delete tmp;
		  }
	       }
	       else
	       {
		  results->append(tmp);
	       }
	    }
	 }
      }
   }
}

DynamicRoutingTable::DynamicRoutingTable(void)
{
   _rep = new reg_table_rep();
}

DynamicRoutingTable::~DynamicRoutingTable(void)
{
   Dec(_rep);
}

DynamicRoutingTable::DynamicRoutingTable(const DynamicRoutingTable & table)
{
   if(this != &table)
   {
      Inc(_rep = table._rep);
   }
}

DynamicRoutingTable & 
DynamicRoutingTable::operator =(const DynamicRoutingTable & table)
{
   if(this != &table)
   {
      Inc(_rep = table._rep);
   }
   return *this;
}

const DynamicRoutingTable 
DynamicRoutingTable::get_ro_routing_table(void)
{
   return DynamicRoutingTable(_internal_routing_table);
}

DynamicRoutingTable
DynamicRoutingTable::get_rw_routing_table(void)
{
   return DynamicRoutingTable(_internal_routing_table);
}


MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName & classname, 
				 const CIMNamespaceName & ns, 
				 Uint32 type) const
{
   const reg_table_record rec(classname, ns, type, 0);
   
   const reg_table_record *ret = _rep->find(rec);
   return ret->service;
}

// get a single service that can route this extended spec. 
MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName & classname,
				 const CIMNamespaceName & ns,
				 Uint32 type,
				 const Array<Uint8> & extended_type,
				 Uint32 flags,
				 const Array<Uint8> & extended_flags) const 
{
   const reg_table_record rec(classname, ns, type, 
			      extended_type, 
			      flags, 
			      extended_flags, 
			      0);
   Array<reg_table_record *> set;
   
}


PEGASUS_NAMESPACE_END
