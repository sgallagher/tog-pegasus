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

pegasus_basic_identity::pegasus_basic_identity(const pegasus_basic_identity & id)
   : Base(), _username(id._username), _password(id._password)
{
   
}

pegasus_basic_identity::~pegasus_basic_identity(void)
{
}


pegasus_basic_identity & pegasus_basic_identity::operator= (const pegasus_basic_identity & id)
{
   if(this != &id)
   {
      _username.clear();
      _username = id._username;
      _password.clear();
      _password = id._password;
   }
   return *this;
}

Boolean pegasus_basic_identity::operator== (const pegasus_basic_identity & id) const
{
   if ( _username == id._username) 
      return true;
   return false;
}

String pegasus_basic_identity::get_id_string(void) const 
{
   return String(_username);
}

String pegasus_basic_identity::get_cred_string(void) const 
{
   return String(_password);
}

Uint32 pegasus_basic_identity::get_id_type(void) const
{
   return peg_identity_types::USERNAME;
}

Uint32 pegasus_basic_identity::get_credential_type(void) const 
{
   return peg_credential_types::CLEAR_PASSWORD;
}

Boolean pegasus_basic_identity::authenticate(void)
{
   return true;
}

pegasus_identity *pegasus_basic_identity::create_id(void) const
{
   return(new pegasus_basic_identity(*this));
}

pegasus_internal_identity::pegasus_internal_identity(Uint32 cred)
   :_id(peg_identity_types::INTERNAL), _credential(cred)
{
   
}

pegasus_internal_identity::pegasus_internal_identity(const pegasus_internal_identity & id)
   :Base(), _id(id._id), _credential(id._credential)
{
   
}


pegasus_internal_identity & pegasus_internal_identity::operator= ( const pegasus_internal_identity & id)
{
   if(this != &id)
   {
      _id = id._id;
      _credential = id._credential;
   }
   
   return *this;
}

Boolean pegasus_internal_identity::operator== (const pegasus_internal_identity & id) const 
{
   if( _id == id._id)
      if( _credential == id._credential)
	 return true;
   return false;
}


String pegasus_internal_identity::get_id_string(void) const
{
   if( _credential == peg_credential_types::SERVICE )
      return String("INTERNAL::SERVICE");
   if( _credential == peg_credential_types::MODULE)
      return String("INTERNAL::MODULE");
   if( _credential == peg_credential_types::PROVIDER)
      return String("INTERNAL::PROVIDER");
   return String("INTERNAL::UNKNOWN");
}

String pegasus_internal_identity::get_cred_string(void) const
{
   return get_id_string(); 

}


Uint32 pegasus_internal_identity::get_id_type(void) const 
{
   return _id;
}

Uint32 pegasus_internal_identity::get_credential_type(void) const
{
   return _credential;
}

Boolean pegasus_internal_identity::authenticate(void)
{
   if( _id == peg_identity_types::INTERNAL)
   {
      if( _credential == peg_credential_types::SERVICE || 
	  _credential == peg_credential_types::MODULE || 
	  _credential == peg_credential_types::PROVIDER)
	 return true;
   }
   return false;
}

pegasus_identity * pegasus_internal_identity::create_id() const 
{
   return new pegasus_internal_identity(*this);
}


PEGASUS_NAMESPACE_END
