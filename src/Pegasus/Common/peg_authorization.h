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
// Author: Mike Day (mdday@us.ibm.com) << Fri Mar 29 09:28:11 2002 mdd >>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_authorization_h
#define Pegasus_authorization_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>
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


class PEGASUS_COMMON_LINKAGE pegasus_identity
{
   public:
      pegasus_identity(void) { }
      virtual ~pegasus_identity(void){ };
      virtual String get_id_string(void) const = 0;
      virtual String get_cred_string(void) const = 0;
      virtual Uint32 get_id_type(void) const = 0;
      virtual Uint32 get_credential_type(void) const = 0;
      virtual Boolean authenticate(void) = 0;
      virtual pegasus_identity *create_id(void) const = 0;
      virtual Boolean get_auth_bit(Uint32 index, Uint32 bit) const = 0;
};


class PEGASUS_COMMON_LINKAGE pegasus_basic_identity : public pegasus_identity
{
   public:
      typedef pegasus_identity Base;
      
      pegasus_basic_identity(const String & username, 
			     const String & password);
      pegasus_basic_identity( const pegasus_basic_identity & id);
      virtual ~pegasus_basic_identity(void);

      pegasus_basic_identity & operator= (const pegasus_basic_identity & id);
      Boolean operator==(const pegasus_basic_identity & id) const;
      virtual String get_id_string(void) const;
      virtual String get_cred_string(void) const;
      virtual Uint32 get_id_type(void) const ;
      virtual Uint32 get_credential_type(void) const ;
      virtual Boolean authenticate(void) ;
      virtual pegasus_identity *create_id(void) const;
      virtual Boolean get_auth_bit(Uint32 index, Uint32 bit) const { return true ;  }
      
   private:
      pegasus_basic_identity(void);

      String _username;
      String _password;
};


class PEGASUS_COMMON_LINKAGE pegasus_internal_identity : public pegasus_identity
{
   public:
      typedef pegasus_identity Base;
      pegasus_internal_identity(Uint32 identity);
      pegasus_internal_identity(const pegasus_internal_identity & id);
      virtual ~pegasus_internal_identity(void) {};

      pegasus_internal_identity & operator= ( const pegasus_internal_identity & id);
      Boolean operator== (const pegasus_internal_identity & id) const ;
      virtual String get_id_string(void) const;
      virtual String get_cred_string(void) const;
      virtual Uint32 get_id_type(void) const;
      virtual Uint32 get_credential_type(void) const;
      virtual Boolean authenticate(void);
      virtual pegasus_identity *create_id(void) const;
      virtual Boolean get_auth_bit(Uint32 index, Uint32 bit) const { return true ;  }
   private:
      pegasus_internal_identity(void);
      Uint32 _id;
      Uint32 _credential;
};


class PEGASUS_COMMON_LINKAGE pegasus_auth_handle
{
   public:
      pegasus_auth_handle(const pegasus_identity & id) 
      {
	 _id = id.create_id();
      }
      
      virtual ~pegasus_auth_handle(void)
      {
	 delete _id;
      }
      
      virtual Boolean authorized(void) = 0;
      virtual Boolean authorized(Uint32 ) = 0;
      virtual Boolean authorized(Uint32, Uint32) = 0;

   private:
      pegasus_identity *_id;
};



PEGASUS_NAMESPACE_END

#endif
