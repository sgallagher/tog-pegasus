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
// Author: Mike Day (mdday@us.ibm.com) << Fri Mar 29 10:43:02 2002 mdd >>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include "peg_authorization.h"

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

const Uint32 peg_identity_types::USERNAME =                   0x00000001;
const Uint32 peg_identity_types::LDAP_DN  =                   0x00000002;
const Uint32 peg_identity_types::GUID     =                   0x00000003;
const Uint32 peg_identity_types::X509     =                   0x00000004;
const Uint32 peg_identity_types::PKCS6    =                   0x00000005;
const Uint32 peg_identity_types::HTTP_DIGEST_USER_AND_REALM = 0x00000006;
const Uint32 peg_identity_types::UNIX_ID =                    0x00000007;
const Uint32 peg_identity_types::INTERNAL =                   0x00000008;

const Uint32 peg_credential_types::CLEAR_PASSWORD =           0x00000001;
const Uint32 peg_credential_types::CRYPT_PASSWORD =           0x00000002;
const Uint32 peg_credential_types::HTTP_DIGEST =              0x00000003;
const Uint32 peg_credential_types::SERVICE =                  0x00000004;
const Uint32 peg_credential_types::MODULE =                   0x00000005;
const Uint32 peg_credential_types::PROVIDER =                 0x00000006;

pegasus_base_identity::pegasus_base_identity(Uint32 identity_type, 
					     void *identity,
					     Uint32 credential_type,
					     void *credential)
{
   _rep = new pegasus_identity_rep(identity_type, 
				   identity,
				   credential_type,
				   credential);
   
}

pegasus_base_identity::pegasus_base_identity(const pegasus_base_identity & id)
{
   _rep = id._rep;
}

pegasus_base_identity & pegasus_base_identity::operator=(const pegasus_base_identity & id)
{
   if( &id != this)
   {
      _rep->dereference();
      if( _rep->get_reference() == 0 )
	 delete _rep;
      _rep = id._rep;
   }
   return *this;
}

pegasus_base_identity *pegasus_base_identity::operator=(const pegasus_base_identity *id)
{
   if(id == NULL)
      throw NullPointer();
    
   if( id != this)
   {
      _rep->dereference();
      if( _rep->get_reference() == 0 )
	 delete _rep;
      _rep = id->_rep;
   }
   return this;
}


pegasus_base_identity::~pegasus_base_identity(void)
{
   _rep->dereference();

   if ( _rep->get_reference() == 0 )
   {
      delete _rep;
   }
}


pegasus_basic_identity::pegasus_basic_identity(String & username,
					       String & password)
   : Base(peg_identity_types::USERNAME, 
	  new String(username), 
	  peg_credential_types::CLEAR_PASSWORD, 
	  new String(password))
	  
{
   
}

pegasus_basic_identity::pegasus_basic_identity(const pegasus_basic_identity & id)
   :Base(id)
{
   
}
	 
pegasus_basic_identity::~pegasus_basic_identity(void)
{
   // if we are the last handle to the identity, we need to clean up our 
   // identity and credential types
   // our parent will delete the actual representation of the identity
   if( get_base_reference_count() == 1 )
   {
      delete reinterpret_cast<String *>(get_base_identity());
      delete reinterpret_cast<String *>(get_base_credential());
   }
}

const String & pegasus_basic_identity::get_username(void)
{
   return *(reinterpret_cast<const String *>( get_base_identity()));
}

const String & pegasus_basic_identity::get_password(void)
{
   return *(reinterpret_cast<const String *>( get_base_credential()));
}

Boolean pegasus_basic_identity::authenticate(void)
{
   return true;
}


// credential should be SERVICE, MODULE, or PROVIDER
pegasus_internal_identity::pegasus_internal_identity(Uint32 credential)
   : Base(peg_identity_types::INTERNAL, 
	  (void *)0,
	  credential, 
	  (void *)credential)
{
}

pegasus_internal_identity::pegasus_internal_identity(const pegasus_internal_identity & id)
   : Base(id)
{
}

pegasus_internal_identity::~pegasus_internal_identity(void)
{
}

const Uint32 pegasus_internal_identity::get_credential(void)
{
   return reinterpret_cast<Uint32>(get_base_credential());
}

Boolean pegasus_internal_identity::authenticate(void)
{
   Uint32 cred = get_base_cred_type();
   
   if( (cred == peg_credential_types::SERVICE) ||
       (cred == peg_credential_types::MODULE) ||
       (cred == peg_credential_types::PROVIDER))
      return true;
   return false;
}


PEGASUS_NAMESPACE_END
