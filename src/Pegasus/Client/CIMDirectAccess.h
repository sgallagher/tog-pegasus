#ifndef Pegasus_CIMDirectAccess_h
#define Pegasus_CIMDirectAccess_h
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


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/CIMClientException.h>
#include <Pegasus/Client/Linkage.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Client/ClientOpPerformanceDataHandler.h>
#include "CIMDirectAccessRep.h"
#include <Pegasus/Client/CIMClient.h>


PEGASUS_NAMESPACE_BEGIN
#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES
#ifdef PEGASUS_USE_DIRECTACCESS_FOR_LOCAL


//class CIMClientInterface;
struct cimSubscription;


class CIMDirectAccessRep;

/**
    The CIMDirectAccess class provides an interface for a CIM exploiting
    application to communicate with local CIM providers.  That is, providers
    that reside on the same system as the CIMDirectAccess caller.

    If you need to communicate with remote CIM servers, or want to have
    local/remote transparency for your application, then use CIMClient.h.

    See CIMClient.h for its documentation on its methods.  
    

*/
class PEGASUS_CLIENT_LINKAGE CIMDirectAccess {
    
public:


    CIMDirectAccess();
   ~CIMDirectAccess(); 


    /** A simple way to subscribe to CIM indications, and get the indication
        via callback.  Works for direct access only.  Throws an exception if 
        called in remote mode or if not connected, or cimSubscription is
        invalid.  The persistence of subscriptions created via this method 
        depends on what type of direct access build was  used (see below). 
        In all cases, subscriptions created via this method are not shared 
        with the CIM server.    (See PEPs 273,274.)
    */
    void addSubscription( cimSubscription& );
    
    
    /** Removes a subscription created with addSubscription(). Works in local
        mode only. Throws an exception if called in remote node or if not
        connected. If a subscription does not exist, nothing happens. If 
        cimSubscription::subscriptionName is "*" all subscriptions created 
        by this process are removed.
    */
    void removeSubscription( const cimSubscription& );

#if 0   
    /** Call this to override the default remote/local mode determination.
        By default this mode is set when one of the connect* methods is 
        invoked. Mode is set to local if the local host (same system) is 
        being accessed. Has effect only for a single connect*() call; the 
        next one. 
    */
    void donotuseDirectAccessforLocal();   // ???????????  fix.  needed??

    
    /** The following methods relating to connection function are present
        in the CIMDirectAccess interface for compatibility with CIMClient.

        If a connect() method is called with a non-local host, an exception
        is thrown.
    */
    void CIMDirectAccess::connect(
              const String& host,
              const Uint32 portNumber,
              const String& userName,
              const String& password);
    void CIMDirectAccess::connect(
              const String& host,
              const Uint32 portNumber,
              const SSLContext& sslContext,
              const String& userName,
              const String& password);

    void CIMDirectAccess::connectLocal();
    void CIMDirectAccess::disconnect();
#endif    

#if 0    
    enum dacim_buildtype {integrated, separate_repository, stand_alone};
    
    /** Returns direct access build type.
    */
    dacim_buildtype getDirectAccessBuildtype();
#endif    
    



private:

    Boolean _isLocalHost(const String&);  
    CIMDirectAccess(const CIMDirectAccess&);
    CIMDirectAccess& operator=(const CIMDirectAccess&);
    CIMDirectAccessRep *rep_;
    
};




typedef void (*indicationListener)( const CIMInstance&, void* );

struct PEGASUS_CLIENT_LINKAGE cimSubscription {
                                   //* <-- indicates required
    String                          
        subscriptionName,          //*any name, should be unique. ignored.
        indicationSourceNamespace, //*NS where indi originates
        filterQueryLanguage,       // language of filtering expression
        filterQuery;               // filtering expression
    indicationListener 
        indicationCallback;        //*routine to call when an indi happens
    const void *callerhandle;       //*pointer to what caller wants. Ignored.
                                   // Returned to callback with indication.
    CIMDateTime   
        subscriptionAddTime;       // start is when subscri is added
    cimSubscription();             // will set all to default values.
    cimSubscription( const String& name, const String& namspace, 
                     indicationListener );
    cimSubscription( const cimSubscription& );
    Boolean operator==( const cimSubscription& ) const;
    Boolean isvalid() const;
    };


#endif
#endif
PEGASUS_NAMESPACE_END
#endif /* Pegasus_CIMDirectAccess_h */
