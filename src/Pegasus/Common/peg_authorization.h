//%////-*-c++-*-////////////////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com) << Fri Mar 29 09:28:11 2002 mdd >>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_authorization_h
#define Pegasus_authorization_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <bitset>

PEGASUS_NAMESPACE_BEGIN


// these constants will certainly change with implementation
class PEGASUS_COMMON_LINKAGE peg_identity_types
{
   public:
      static const Uint32 USERNAME;
      static const Uint32 LDAP_DN;
      static const Uint32 GUID;
      static const Uint32 X509;
      static const Uint32 PKCS6;
      static const Uint32 HTTP_DIGEST_USER_AND_REALM;
      static const Uint32 UNIX_ID;
      static const Uint32 INTERNAL;
};

// these constants will certainly change with implementation
class PEGASUS_COMMON_LINKAGE peg_credential_types
{
   public:
      static const Uint32 CLEAR_PASSWORD;
      static const Uint32 CRYPT_PASSWORD;
      static const Uint32 HTTP_DIGEST;
      static const Uint32 SERVICE;
      static const Uint32 MODULE;
      static const Uint32 PROVIDER;
};


class PEGASUS_COMMON_LINKAGE pegasus_identity_rep
{
   public:
      pegasus_identity_rep(Uint32 identity_type, 
		       void *identity,
		       Uint32 credential_type,
		       void *credential)
	 : _identity_type(identity_type),
	   _identity(identity),
	   _credential_type(credential_type),
	   _credential(credential),
	   _reference(1), 
	   _thread_safety()
      {

      }
      
      virtual ~pegasus_identity_rep(void)
      {
      }

      void reference(void) 
      {
	 _reference++;
      }
      
      void dereference(void)
      {
	 _reference--;
      }
      
      Uint32 get_reference(void)
      {
	 return  _reference.value();
      }

      Uint32 get_identity_type(void)
      {
	 return _identity_type;
      }
      
      Uint32 get_credential_type(void)
      {
	 return _credential_type;
      }
      
      void *get_identity(void)
      {
	 if( _reference.value() )
	    return _identity;
	 return NULL;
      }
      
      void *get_credential(void)
      {
	 if( _reference.value() )
	    return _credential;
	 return NULL;
      }
		       
      void lock(void) 
      {
	 _thread_safety.lock(pegasus_thread_self());
      }
      
      void unlock(void)
      {
	 _thread_safety.unlock();
      }
      

   private:
      Uint32 _identity_type;
      void *_identity;
      Uint32 _credential_type;
      void *_credential;
      AtomicInt _reference;
      Mutex _thread_safety;
            
      pegasus_identity_rep();
      pegasus_identity_rep(const pegasus_identity_rep & rep);
      pegasus_identity_rep & operator = (const pegasus_identity_rep & rep);
};


class PEGASUS_COMMON_LINKAGE pegasus_base_identity
{
   public:
      pegasus_base_identity(Uint32 identity_type, 
			    void *identity,
			    Uint32 credential_type,
			    void *credential);
      
      pegasus_base_identity & operator=(const pegasus_base_identity & id);
      pegasus_base_identity(const pegasus_base_identity & id);

      virtual ~pegasus_base_identity(void);
      virtual Boolean authenticate(void) = 0;

      Uint32 get_base_reference_count(void)
      {
	 return _rep->get_reference();
      }
      
      Uint32 get_base_id_type(void)
      {
	 return _rep->get_identity_type();
      }
      
      Uint32 get_base_cred_type(void)
      {
	 return _rep->get_credential_type();
      }
      
      void *get_base_identity(void)
      {
	 return _rep->get_identity();
      }
      
      void *get_base_credential(void)
      {
	 return _rep->get_credential();
      }
      
   private:
      pegasus_identity_rep * _rep;
};



class PEGASUS_COMMON_LINKAGE pegasus_basic_identity : protected  pegasus_base_identity
{
   public:
      typedef pegasus_base_identity Base;
      
      pegasus_basic_identity(String & username, String & password);
      pegasus_basic_identity( const pegasus_basic_identity & id);
      pegasus_basic_identity & operator= (const pegasus_basic_identity & id);
      
      virtual ~pegasus_basic_identity(void);
      
      const String & get_username(void) ;
      const String & get_password(void) ;

      Boolean authenticate(void);
      
   private:
      pegasus_basic_identity(void);
};


class PEGASUS_COMMON_LINKAGE pegasus_internal_identity : public pegasus_base_identity
{
   public:
      typedef pegasus_base_identity Base;
      pegasus_internal_identity(Uint32 identity);
      pegasus_internal_identity(const pegasus_internal_identity & id);
      pegasus_internal_identity & operator= ( const pegasus_internal_identity & id);
      
      virtual ~pegasus_internal_identity(void);
      const Uint32 get_credential(void);
      Boolean authenticate(void);
   private:
      pegasus_internal_identity(void);
};



class PEGASUS_COMMON_LINKAGE pegasus_authorization_handle
{
   public:
      pegasus_authorization_handle( )
      {
      }

      virtual ~pegasus_authorization_handle(void)
      {
      }
      
      virtual Boolean authorized(pegasus_base_identity *, Uint32 operation) = 0;
      virtual Boolean authorized(pegasus_base_identity *) = 0;
   protected:
      pegasus_authorization_handle(const pegasus_authorization_handle & auth)
      {
      }
      
      pegasus_authorization_handle & operator=(const pegasus_authorization_handle & auth)
      {
	 if(this != &auth)
	 {
	 }
	 return *this;
      }
      
   private:

};



PEGASUS_NAMESPACE_END

#endif
