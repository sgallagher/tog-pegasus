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

#include <iostream>
#include <fstream>
#include <Pegasus/Client/CIMClientRep.h>
#include <Pegasus/Common/PegasusVersion.h>
#include "CIMDirectAccess.h"
#include "CIMDirectAccessRep.h"





PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN


#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL

CIMDirectAccess::CIMDirectAccess()
{
    rep_ = CIMDirectAccessRep::get();
}

CIMDirectAccess::~CIMDirectAccess() { rep_->release(); }


//--------------------------------------------------------
Boolean CIMDirectAccess::_isLocalHost(const String& host) 
{
    if ( host == String::EMPTY  || 
         String::equalNoCase(host,"localhost") ) return true;
    return System::sameHost( host );
}


#if 0
void CIMDirectAccess::connect(
    const String& host,
    const Uint32 portNumber,
    const String& userName,
    const String& password)
{
   if (_isLocalHost(host)) {
       CIMClient::connectLocal();
       return;
   }
   else throw CannotConnectException(
              String("CIMDirectAccess can only be used to access the local host"));
}



void CIMDirectAccess::connect(
    const String& host,
    const Uint32 portNumber,
    const SSLContext& sslContext,
    const String& userName,
    const String& password)
{
   if (_isLocalHost(host)) {
       CIMClient::connectLocal();
       return;
   }
   else throw CannotConnectException(
              String("CIMDirectAccess can only be used to access the local host"));
}


void CIMDirectAccess::connectLocal() {
    CIMClient::connectLocal();
}
void CIMDirectAccess::disconnect() {
    CIMClient::disconnect();
}
#endif


void CIMDirectAccess::addSubscription( cimSubscription& sub )
{
         rep_->addSubscription( sub ); 
}

void CIMDirectAccess::removeSubscription( const cimSubscription& sub )
{
         rep_->removeSubscription( sub );
}

#if 0
void CIMDirectAccess::donotuseDirectAccessforLocal()
{
         //rep_->donotuseDirectAccessforLocal();
         // 
         // do what here??            fix
}
#endif



cimSubscription::cimSubscription() 
    : subscriptionName(String::EMPTY),  
      indicationSourceNamespace(String::EMPTY),
      filterQueryLanguage(String("DMTF:CQL")),  
      filterQuery(String::EMPTY), 
      indicationCallback(NULL),         callerhandle(this)  {} 
      
cimSubscription::cimSubscription( const String& n, const String& ns, 
                                  const indicationListener l          ) 
    : subscriptionName(n),   
      indicationSourceNamespace(ns), 
      filterQueryLanguage(String("DMTF:CQL")),   
      filterQuery(String::EMPTY), 
      indicationCallback(l),            callerhandle(this)   {} 

#if 0 // default cp ctor is fine     
cimSubscription::cimSubscription( const cimSubscription& s ) 
    : subscriptionName(s.subscriptionName),   
      indicationSourceNamespace(s.indicationSourceNamespace), 
      filterQueryLanguage(s.filterQueryLanguage),   
      filterQuery(s.filterQuery), 
      indicationCallback(s.indicationCallback),
      callerhandle(s.callerhandle)                 {} 
#endif      

Boolean cimSubscription::operator==(const cimSubscription& s ) const {
    return (subscriptionName == s.subscriptionName) &&
           (indicationSourceNamespace == s.indicationSourceNamespace);
           //&&
           //(filterQuery == s.filterQuery) &&
           //(indicationCallback == s.indicationCallback);
}
Boolean cimSubscription::isvalid() const {
    if (!String::equalNoCase(filterQueryLanguage,"DMTF:CQL")) return false;
    if (indicationCallback == NULL) return false;
    return true;
}   

#endif // PEGASUS_USE_DIRECTACCESS_FOR_LOCAL
#endif // PEGASUS_USE_EXPERIMENTAL_INTERFACES
PEGASUS_NAMESPACE_END
