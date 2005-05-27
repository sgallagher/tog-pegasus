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
// Modified By: Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101, Bug#3531
//
//%////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/AutoPtr.h>
#include "reg_table.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const DynamicRoutingTable _internal_routing_table;

class reg_table_record
{
   private:
      reg_table_record(void);
      virtual ~reg_table_record(void);
      

      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       Uint32 flags, 
		       const MessageQueueService * destination_service);
      

      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       const Array<Uint8> & extended_type, 
		       Uint32 flags, 
		       const Array<Uint8> & extended_flags,
		       const MessageQueueService * destination_service);
      

      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       Uint32 flags, 
		       const MessageQueueService * destination_service,
		       const String & provider_name, 
		       const String & module_name);

      reg_table_record(const CIMName & class_name, 
		       const CIMNamespaceName & namespace_name, 
		       Uint32 type, 
		       const Array<Uint8> & extended_type, 
		       Uint32 flags, 
		       const Array<Uint8> & extended_flags,
		       const MessageQueueService * destination_service,
		       const String & provider_name,
		       const String & module_name);
      
      reg_table_record(const reg_table_record & );
      reg_table_record & operator =(const reg_table_record & );

      void dump(void);
      
      CIMName class_name;
      CIMNamespaceName namespace_name;
      Uint32 type;
      Array<Uint8> extended_key;
      Uint32 flags;
      Array<Uint8> extended_flags;
      MessageQueueService *service;
      String provider_name;
      String module_name;
      
      friend class reg_table_rep;
      friend class DynamicRoutingTable;
};

reg_table_record::reg_table_record(void)
   : class_name(String::EMPTY),
     namespace_name(String::EMPTY),
     type(0xffffffff),
     extended_key(),
     flags(0xffffffff),
     extended_flags(),
     service(0)
{
}

reg_table_record::reg_table_record(
   const CIMName &_name, 
   const CIMNamespaceName &_ns, 
   Uint32 _type, 
   Uint32 _flags,
   const MessageQueueService * _svce)
   : class_name(_name),
     namespace_name(_ns),
     type(_type),
     extended_key(),
     flags(_flags),
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



reg_table_record::reg_table_record(
   const CIMName &_name, 
   const CIMNamespaceName &_ns, 
   Uint32 _type, 
   Uint32 _flags,
   const MessageQueueService * _svce,
   const String & _provider_name, 
   const String & _module_name)
   : class_name(_name),
     namespace_name(_ns),
     type(_type),
     extended_key(),
     flags(_flags),
     extended_flags(),
     service(const_cast<MessageQueueService *>(_svce)),
     provider_name(_provider_name),
     module_name(_module_name)
{

}


reg_table_record::reg_table_record(
   const CIMName &_name, 
   const CIMNamespaceName &_ns, 
   Uint32 _type, 
   const Array<Uint8> & _type_key, 
   Uint32 _flags, 
   const Array<Uint8> & _ext_flags,
   const MessageQueueService * _svce,
   const String & _provider_name,
   const String & _module_name)
   : class_name(_name),
     namespace_name(_ns),
     type(_type),
     extended_key(_type_key),
     flags(_flags),
     extended_flags(_ext_flags),
     service(const_cast<MessageQueueService *>(_svce)),
     provider_name(_provider_name),
     module_name(_module_name)
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
   provider_name = rtr.provider_name;
   module_name = rtr.module_name;
}

reg_table_record:: ~reg_table_record(void)
{
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
      provider_name = other.provider_name;
      module_name = other.module_name;
   }
   return *this;
}


void 
reg_table_record::dump(void)
{
   PEGASUS_STD(cout) << "----Dumping Reg Table Record:----" << PEGASUS_STD(endl);
   PEGASUS_STD(cout) << class_name.getString() << PEGASUS_STD(endl);
   PEGASUS_STD(cout) << namespace_name.getString() << PEGASUS_STD(endl);
   PEGASUS_STD(cout) << type << PEGASUS_STD(endl);
   PEGASUS_STD(cout) << "---------------------------------" << PEGASUS_STD(endl);
}


// instead of concatenating class, namespace, type into a single key, use a 3-level set-associative cache 
// implemented with hash tables for namespace, class, and type. Because key composition is not 
// necessary it should be faster than a fully associative cache. 

struct RegTableEqual
{
    static Boolean equal(const String & x, const String & y)
    {
        if (0 == String::compareNoCase(x, y))
            return true;
        return false;
    }
};

struct RegTableHash
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

typedef HashTable<String,  reg_table_record *, RegTableEqual, RegTableHash > routing_table;

typedef HashTable<Uint32, routing_table *, EqualFunc<Uint32>, HashFunc<Uint32> > type_table;

typedef HashTable<String, type_table *, RegTableEqual, RegTableHash  > namespace_table;

class reg_table_rep : public Sharable
{
   public:
      reg_table_rep(void) 
      {
      }
      ~reg_table_rep(void); 
   private:

      // store a record 
      Boolean _insert(const reg_table_record & rec);

      // retrieve a pointer to the stored record
      const reg_table_record * find(const reg_table_record& rec);
      const reg_table_record * find_extended(const reg_table_record& rec);
      void find(const reg_table_record& rec, Array<reg_table_record *> *results);
      void find_extended(const reg_table_record& rec, Array<reg_table_record *> *results);
      
      // remove the record and retrieve a pointer to it 
      reg_table_record * release(const reg_table_record& rec);
      reg_table_record *release_extended(const reg_table_record& rec);

      void release(const reg_table_record& rec, Array<reg_table_record *> *results);
      void release_extended(const reg_table_record& rec, Array<reg_table_record *> *results);
      
      // remove and destroy a record or records
      void destroy(const reg_table_record& rec);
      void destroy_extended(const reg_table_record& rec);
      
      void destroy_all(void);

      Uint32 remove_by_router(const reg_table_record& rec);
      


      static const Uint32 FIND;
      static const Uint32 REMOVE;
      static const Uint32 MULTIPLE;
      static const Uint32 DESTROY;
      static const Uint32 EXTENDED;
      static const Uint32 SERVICE;
      static const Uint32 STRINGS;
      
      
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
      

      Boolean _extended_match(const reg_table_record & rec_a, 
			      const reg_table_record & rec_b) const;

      void _dump_table();
      namespace_table _table;
      Mutex _mutex;

      friend class DynamicRoutingTable;
};

const Uint32 reg_table_rep::FIND =         0x00000001;
const Uint32 reg_table_rep::REMOVE =       0x00000002;
const Uint32 reg_table_rep::MULTIPLE =     0x00000004;
const Uint32 reg_table_rep::DESTROY =      0x00000008;
const Uint32 reg_table_rep::EXTENDED =     0x00000010;
const Uint32 reg_table_rep::SERVICE  =     0x00000020;
const Uint32 reg_table_rep::STRINGS  =     0x00000020;

reg_table_rep::~reg_table_rep(void)
{
   type_table *tt;
   routing_table *rt;
   reg_table_record *record  = 0;

   try { 
    for(namespace_table::Iterator _table_i = _table.start(); _table_i; _table_i++)
    {
      tt = _table_i.value();
      for(type_table::Iterator _tt_i = tt->start(); _tt_i  ;_tt_i++)
       {
         rt = _tt_i.value();
         for (routing_table::Iterator _rt_i = rt->start(); _rt_i; _rt_i++)
          {
             delete _rt_i.value();
          }
         delete rt;
       }
     delete tt;
    }
   } catch (... ) 
   {
     // Just ignore them. The worst that can happend is that we have a memory leak.
   }
}
// insert optimized for simplicity, not speed 
Boolean
reg_table_rep::_insert(const reg_table_record &rec)
{
   // ipc synchronization 
   AutoMutex monitor(_mutex);
   
   type_table *tt = 0;
   
   if(false ==  _table.lookup(rec.namespace_name.getString(), tt))
   {
      AutoPtr<type_table> temp(new type_table()); 
      _table.insert(rec.namespace_name.getString(), temp.get());
      temp.release();
      if(false ==  _table.lookup(rec.namespace_name.getString(), tt))
	 return false;
      
   }
   
   routing_table *rt = 0;
   
   if(false == tt->lookup(rec.type, rt))
   {
      AutoPtr<routing_table> temp(new routing_table()); 
      tt->insert(rec.type, temp.get());
      temp.release();
      if(false == tt->lookup(rec.type, rt))
	 return false;
   }
   
   Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	       "reg_table_rep::_insert - Inserting provider $0 into the provider reqistration table,",
	       rec.class_name.getString());	

   return rt->insert(rec.class_name.getString(), new reg_table_record(rec));
}

const reg_table_record *
reg_table_rep::find(const reg_table_record & rec)
{
   return _find(rec, FIND);
}

const reg_table_record *
reg_table_rep::find_extended(const reg_table_record & rec)
{
   return _find(rec, FIND | EXTENDED);
}



void 
reg_table_rep::find(const reg_table_record & rec, 
		    Array<reg_table_record *> *results)
{
   _find(rec, (FIND | MULTIPLE), results);
}

void 
reg_table_rep::find_extended(const reg_table_record & rec, 
			     Array<reg_table_record *> *results)
{
   _find(rec, (FIND | MULTIPLE | EXTENDED), results);
}


reg_table_record *
reg_table_rep::release(const reg_table_record& rec)
{
   return _find(rec, (FIND | REMOVE));
   
}

reg_table_record *
reg_table_rep::release_extended(const reg_table_record &rec)
{
   return _find(rec, (FIND | REMOVE | EXTENDED) );
}


void 
reg_table_rep::release(const reg_table_record& rec, 
		       Array<reg_table_record *> *results)
{
    _find(rec, (FIND | REMOVE), results);
}

void 
reg_table_rep::release_extended(const reg_table_record &rec, 
				Array<reg_table_record *> *results)
{
   _find(rec, (FIND | REMOVE | EXTENDED), results);
}

void 
reg_table_rep::destroy(const reg_table_record & rec)
{
   _find(rec, (FIND | REMOVE | DESTROY));
}

void 
reg_table_rep::destroy_extended(const reg_table_record& rec)
{
   _find(rec, (FIND | REMOVE | DESTROY | EXTENDED));
}

void 
reg_table_rep::destroy_all(void)
{
   reg_table_record rec;
   _find(rec, (FIND | REMOVE | DESTROY | MULTIPLE ));
}


Uint32
reg_table_rep::remove_by_router(const reg_table_record& rec)
{
   Uint32 ret;
   
   Array<reg_table_record *> results;
   
   _find(rec, (FIND | REMOVE | DESTROY | MULTIPLE | SERVICE), &results);

   ret = results.size();
   for(Uint32 i = 0; i < results.size() ; ++i)
   {
      delete results[i];
      results[i] = 0;
   }
   return ret;
}


reg_table_record * 
reg_table_rep::_find(const reg_table_record &rec, 
		     Uint32 flags, 
		     Array<reg_table_record *> *arr_ptr)
{
   // ipc synchronization 
   AutoMutex monitor(_mutex);
   type_table *tt = 0;
   Boolean try_again = true;
   
   if(flags & MULTIPLE )
   {
      
      _enumerate(rec,
		 flags, 
		 arr_ptr);
      return 0;
   }
   else 
   {

      // find the type entry
      if(true == _table.lookup(rec.namespace_name.getString(), tt))
      {
	try_again_wild_namespace:
	 routing_table *rt;
	 if(true == tt->lookup(rec.type, rt))
	 {
	    reg_table_record *record ;

	    while(true == rt->lookup(rec.class_name.getString(), record))
	    {
	       // flags value of all foxes always matches 
	       if(rec.flags != 0xffffffff)
		  if(record->flags != rec.flags)
		     continue;
	       
	       if(flags & EXTENDED)
	       {
		  if(false == _extended_match(rec, *record))
		     continue;
	       }
	       
	       if(flags & REMOVE || flags & DESTROY)
	       {
		  rt->remove(rec.class_name.getString());
		  if(flags & DESTROY)
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

// now try again using the special wildcard namespace table
   CIMNamespaceName _wild;
   _wild.clear();
   if(true == try_again && true == _table.lookup(_wild.getString(), tt))
   {
      try_again = false;
      goto try_again_wild_namespace;
   }
   
   return 0;
}

// do not call directly - does not lock the reg table mutex !!
void 
reg_table_rep::_enumerate(const reg_table_record & rec,
			  Uint32 flags, 
			  Array<reg_table_record *> *results)
{

  _enuerate_start:

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
      
	 for(type_table::Iterator y = i.value()->start(); y != 0 ; y++)
	 {
	    // type of -1 is a wildcard
	    if(rec.type != 0xffffffff)
	    {
	       if(rec.type != y.key())
		  continue;
	    }
	 
	    for(routing_table::Iterator x = y.value()->start(); x != 0; x++)
	    {
	       // null class_name is a wildcard
	       if(false == rec.class_name.isNull())
	       {
		  if(!rec.class_name.equal(x.value()->class_name))
		     continue;
	       }
	       reg_table_record *tmp = x.value();
	       // flags value of all foxes always matches 
	       if(rec.flags != 0xffffffff)
		  if(tmp->flags != rec.flags)
		     continue;

	       if(flags & EXTENDED)
	       {
		  if(false == _extended_match(rec, *tmp))
		     continue;
	       }
	       
	       if(flags & SERVICE)
	       {
		  if(rec.service != tmp->service)
		     continue;
	       }
	       
	       if(flags & REMOVE || flags & DESTROY)
	       {
		  y.value()->remove(x.key());
		  if(flags & DESTROY)
		  {
		     delete tmp;
		  }
		  goto _enuerate_start;
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


Boolean 
reg_table_rep::_extended_match(const reg_table_record & rec_a, 
			       const reg_table_record & rec_b) const
{
   Uint32 rec_size;
   if((rec_size = rec_a.extended_key.size()) != rec_b.extended_key.size())
      return false;

   Uint32 i = 0;
   do 
   {
      if(! (rec_a.extended_key[i] == rec_b.extended_key[i]) )
	 return false;
      ++i;
   }while (i < rec_size );
   return true;
}

void 
reg_table_rep::_dump_table()
{
   PEGASUS_STD(cout) <<"******** Dumping Reg Table ********"  << PEGASUS_STD(endl);
   AutoMutex monitor(_mutex);
   for(namespace_table::Iterator i = _table.start(); i; i++)
   {
      PEGASUS_STD(cout) << "Namespace: "  << i.key() << PEGASUS_STD(endl);
      
      for(type_table::Iterator y = i.value()->start(); y != 0 ; y++)
      {

	 PEGASUS_STD(cout) << "Type: " << y.key()  << PEGASUS_STD(endl);
	 
	 for(routing_table::Iterator x = y.value()->start(); x != 0; x++)
	 {
	       reg_table_record *tmp = x.value();
	       tmp->dump();
	 }
      }
   }
}


const Uint32 DynamicRoutingTable:: INTERNAL       = 0x00000001;
const Uint32 DynamicRoutingTable:: INSTANCE       = 0x00000002;
const Uint32 DynamicRoutingTable:: CLASS          = 0x00000003;
const Uint32 DynamicRoutingTable:: METHOD         = 0x00000004;
const Uint32 DynamicRoutingTable:: ASSOCIATION    = 0x00000005;
const Uint32 DynamicRoutingTable:: QUERY          = 0x00000006;
const Uint32 DynamicRoutingTable:: INDICATION     = 0x00000007;
const Uint32 DynamicRoutingTable:: CONSUMER       = 0x00000008;
const Uint32 DynamicRoutingTable:: SERVICE        = 0x00000009;
const Uint32 DynamicRoutingTable:: AUTHORIZATION  = 0x0000000a;
const Uint32 DynamicRoutingTable:: AUTHENTICATION = 0x0000000b;
const Uint32 DynamicRoutingTable:: ENCODE_HOOK    = 0x0000000c;
const Uint32 DynamicRoutingTable:: DECODE_HOOK    = 0x0000000d;
const Uint32 DynamicRoutingTable:: EXTENDED       = 0x0000000e;


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
     // De-allocate the _rep we have internally
      Dec(_rep);
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

// get a single service that can route this spec. 

MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName & classname, 
				 const CIMNamespaceName & ns, 
				 Uint32 type,
				 Uint32 flags) const
{
   reg_table_record rec(classname, ns, type, flags, 0);
   const reg_table_record *ret = _rep->find(rec);
   if(ret)
      return ret->service;
   return 0;
   
}

MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName & classname, 
				 const CIMNamespaceName & ns, 
				 Uint32 type,
				 Uint32 flags, 
				 String & provider,
				 String & module) const
{
   reg_table_record rec(classname, ns, type, flags, 0, provider, module);
   const reg_table_record *ret = _rep->find(rec);
   if(ret)
   {
      provider = ret->provider_name;
      module = ret->module_name;
      return ret->service;
   }
   
   return 0;
   
}

// get a single service that can route this extended spec. 
MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName& classname,
				 const CIMNamespaceName& ns,
				 Uint32 type,
				 const Array<Uint8>& extended_type,
				 Uint32 flags,
				 const Array<Uint8>& extended_flags) const 
{
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			0);
   const reg_table_record *ret = _rep->find_extended(rec);
   if(ret)
      return ret->service;
   return 0;
}


// get a single service that can route this extended spec. 
MessageQueueService *
DynamicRoutingTable::get_routing(const CIMName& classname,
				 const CIMNamespaceName& ns,
				 Uint32 type,
				 const Array<Uint8>& extended_type,
				 Uint32 flags,
				 const Array<Uint8>& extended_flags,
				 String & provider,
				 String & module) const 
{
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			0,
			provider, 
			module);
   const reg_table_record *ret = _rep->find_extended(rec);
   if(ret)
   {
      provider = ret->provider_name;
      module = ret->module_name;
      return ret->service;
   }
   
   return 0;
}


// get an array of services that can route this spec 
void
DynamicRoutingTable::get_routing(const CIMName& classname, 
				 const CIMNamespaceName& ns, 
				 Uint32 type,
				 Uint32 flags,
				 Array<MessageQueueService*>& results) const
{
   reg_table_record rec(classname, ns, type, flags, 0);
   Array<reg_table_record*> records;
   _rep->find(rec, &records);
   for(Uint32 i = 0; i < records.size(); ++i )
   {
      // do not include null service pointers in results
      if(records[i]->service)
	 results.append((records[i])->service);
   }
}


// get an array of services that can route this extended spec. 
void 
DynamicRoutingTable::get_routing(const CIMName& classname,
				 const CIMNamespaceName& ns,
				 Uint32 type,
				 const Array<Uint8>& extended_type,
				 Uint32 flags,
				 const Array<Uint8>& extended_flags, 
				 Array<MessageQueueService*>& results) const 
{
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			0);
   Array<reg_table_record*> records;
   _rep->find_extended(rec, &records);
   for(Uint32 i = 0; i < records.size(); ++i )
   {
      // do not include null service pointers in results
      if(records[i]->service)
	 results.append((records[i])->service);
   }
}


Boolean 
DynamicRoutingTable::insert_record(const CIMName& classname, 
				   const CIMNamespaceName& ns, 
				   Uint32 type,
				   Uint32 flags,
				   MessageQueueService* svce) 
{
   reg_table_record rec(classname, ns, type, flags, svce);
   return (_rep->_insert(rec));
}

Boolean 
DynamicRoutingTable::insert_record(const CIMName& classname, 
				   const CIMNamespaceName& ns, 
				   Uint32 type,
				   Uint32 flags,
				   MessageQueueService* svce, 
				   const String& provider, 
				   const String& module) 
{
   reg_table_record rec(classname, ns, type, flags, svce, provider, module);
   return (_rep->_insert(rec));
}


Boolean 
DynamicRoutingTable::insert_record(const CIMName& classname, 
				   const CIMNamespaceName& ns,
				   Uint32 type,
				   const Array<Uint8>& extended_type,
				   Uint32 flags,
				   const Array<Uint8>& extended_flags, 
				   MessageQueueService* svce) 
{
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			svce);
   return (_rep->_insert(rec));
}


Boolean 
DynamicRoutingTable::insert_record(const CIMName& classname, 
				   const CIMNamespaceName& ns,
				   Uint32 type,
				   const Array<Uint8>& extended_type,
				   Uint32 flags,
				   const Array<Uint8>& extended_flags, 
				   MessageQueueService* svce, 
				   const String& provider,
				   const String& module)
{
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			svce,
			provider, 
			module);
   return (_rep->_insert(rec));
}


MessageQueueService *
DynamicRoutingTable::remove_record(const CIMName& classname, 
				   const CIMNamespaceName& ns, 
				   Uint32 type,
				   Uint32 flags)
{
   MessageQueueService *svce = 0;
   
   reg_table_record rec(classname, ns, type, flags, 0);
   
   const reg_table_record *ret = _rep->release(rec);
   if(ret)
      svce = ret->service;
   delete ret;
   
   return svce;
}


MessageQueueService *
DynamicRoutingTable::remove_record(const CIMName& classname, 
				   const CIMNamespaceName& ns, 
				   Uint32 type,
				   const Array<Uint8>& extended_type,
				   Uint32 flags,
				   const Array<Uint8>& extended_flags)
{
   MessageQueueService *svce = 0;
   
   reg_table_record rec(classname, ns, type, 
			extended_type, 
			flags, 
			extended_flags, 
			0);
   const reg_table_record *ret = _rep->release_extended(rec);

   if(ret)
      svce = rec.service;
   delete ret;
   return svce;
}


Uint32 
DynamicRoutingTable::remove_multiple_records(const CIMName& classname, 
					     const CIMNamespaceName& ns, 
					     Uint32 type,
					     Uint32 flags)
{
   Uint32 ret = 0;
   
   Array<reg_table_record *> results;
   
   reg_table_record rec(classname, ns, type, flags, 0);
   _rep->release(rec, &results);
   ret = results.size();
   for(Uint32 i = 0; i < results.size(); ++i)
   {
      delete results[i];
      results[i] = 0;
   }
   
   return ret;
   
}

      
Uint32
DynamicRoutingTable::remove_multiple_records(const CIMName& classname, 
					     const CIMNamespaceName& ns, 
					     Uint32 type,
					     const Array<Uint8>& extended_type,
					     Uint32 flags,
					     const Array<Uint8>& extended_flags)
{
   Uint32 ret = 0;
   
   Array<reg_table_record *> results;
   
   reg_table_record rec(classname, ns, type, extended_type, flags, extended_flags, 0);
   _rep->release_extended(rec, &results);
   ret = results.size();
   for(Uint32 i = 0; i < results.size(); ++i)
   {
      delete results[i];
      results[i] = 0;
   }
   
   return ret;
}


Uint32
DynamicRoutingTable::remove_router_records(const MessageQueueService* router)
{
   CIMName classname;
   classname.clear();
   CIMNamespaceName ns;
   ns.clear();
   reg_table_record rec(classname, ns, 0xffffffff, 
			0xffffffff,
			router);
   return _rep->remove_by_router(rec);
}


void
DynamicRoutingTable::dump_table(void)
{
   _rep->_dump_table();
}


PEGASUS_NAMESPACE_END
