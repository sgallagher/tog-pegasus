//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationContext_h
#define Pegasus_OperationContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/AcceptLanguages.h>  // l10n
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

#ifndef PEGASUS_REMOVE_DEPRECATED
static const Uint32 CONTEXT_EMPTY =                     0;
static const Uint32 CONTEXT_IDENTITY =                  1;
static const Uint32 CONTEXT_AUTHENICATION =             2;
static const Uint32 CONTEXT_AUTHORIZATION =             3;
static const Uint32 CONTEXT_OTHER_SECURITY =            4;
static const Uint32 CONTEXT_LOCALE =                    5;
static const Uint32 CONTEXT_OPTIONS =                   6;
static const Uint32 CONTEXT_VENDOR =                    7;
static const Uint32 CONTEXT_UID_PRESENT =               8;
static const Uint32 CONTEXT_UINT32_PRESENT =            9;
static const Uint32 CONTEXT_OTHER =                     10;
static const Uint32 CONTEXT_COPY_MEMORY =               11;
static const Uint32 CONTEXT_DELETE_MEMORY =             12;
static const Uint32 CONTEXT_POINTER =                   13;
static const Uint32 CONTEXT_PROVIDERID =                14;

static const Uint32 OPERATION_NONE =                    0x00000000;
static const Uint32 OPERATION_LOCAL_ONLY =              0x00000001;
static const Uint32 OPERATION_INCLUDE_QUALIFIERS =      0x00000002;
static const Uint32 OPERATION_INCLUDE_CLASS_ORIGIN =    0x00000004;
static const Uint32 OPERATION_DEEP_INHERITANCE =        0x00000008;
static const Uint32 OPERATION_PARTIAL_INSTANCE =        0x00000010;
static const Uint32 OPERATION_REMOTE_ONLY =             0x00000020;
static const Uint32 OPERATION_DELIVER =                 0x00000040;
static const Uint32 OPERATION_RESERVE =                 0x00000080;
static const Uint32 OPERATION_PROCESSING =              0x00000100;
static const Uint32 OPERATION_COMPLETE =                0x00000200;
#endif  // !PEGASUS_REMOVE_DEPRECATED

class OperationContextRep;

/**
Context information from client.

<p>The <tt>OperationContext</tt> class carries information about
the context in which the client program issued the request.
Currently, the identity of the user is the only supported
information. Providers must use this to determine whether
the requested operation should be permitted on behalf of
the issuing user.

For example, providers can get the user name information from the
IdentityContext in an OperationContext as shown below:

    <PRE>
    IdentityContainer container(context.get(IdentityContainer::NAME));
    String userName = container.getUserName();
    </PRE>
</p>
*/
class PEGASUS_COMMON_LINKAGE OperationContext
{
public:
    /**
        An element of client context information.
    
        <p>The <tt>Container</tt> object carries the information that
        the provider may access. A container name determines which
        container is being referenced. Currently, only the container
        carrying the user identity of the client is available.</p>
    */
    class PEGASUS_COMMON_LINKAGE Container
    {
    public:
#ifndef PEGASUS_REMOVE_DEPRECATED
        Container(const Uint32 key = CONTEXT_EMPTY);
#endif

        ///
        virtual ~Container(void);

        ///
        virtual String getName(void) const = 0;

#ifndef PEGASUS_REMOVE_DEPRECATED
        const Uint32 & getKey(void) const;
#endif

        /** Makes a copy of the Container object. Caller is responsible 
            for deleting dynamically allocated memory by calling 
            destroy() method.
        */
        virtual Container * clone(void) const = 0;

        /** Cleans up the object, including dynamically allocated memory.
            This should only be used to clean up memory allocated using 
            the clone() method.
        */
        virtual void destroy(void) = 0;

#ifndef PEGASUS_REMOVE_DEPRECATED
    protected:
        Uint32 _key;
#endif
    };

public:
    ///
    OperationContext(void);

    ///
    OperationContext(const OperationContext & context);

    ///
    virtual ~OperationContext(void);

    ///
    OperationContext & operator=(const OperationContext & context);

    /// Removes all containers in the current object.
    void clear(void);

    ///
    const Container & get(const String& containerName) const;

#ifndef PEGASUS_REMOVE_DEPRECATED
    const Container & get(const Uint32 key) const;
#endif

    ///
    void set(const Container & container);

    ///
    void insert(const Container & container);

    ///
    void remove(const String& containerName);

#ifndef PEGASUS_REMOVE_DEPRECATED
    void remove(const Uint32 key);
#endif

protected:
    OperationContextRep* _rep;

};


class IdentityContainerRep;

///
class PEGASUS_COMMON_LINKAGE IdentityContainer
    :
#ifdef PEGASUS_REMOVE_DEPRECATED  // include if NOT using deprecated API
      virtual
#endif
              public OperationContext::Container
{
public:
    static const String NAME;

    ///
    IdentityContainer(const OperationContext::Container & container);
    ///
    IdentityContainer(const IdentityContainer & container);
    ///
    IdentityContainer(const String & userName);
    ///
    virtual ~IdentityContainer(void);
    ///
    IdentityContainer & operator=(const IdentityContainer & container);
    ///
    virtual String getName(void) const;
    ///
    virtual OperationContext::Container * clone(void) const;
    ///
    virtual void destroy(void);
    ///
    String getUserName(void) const;

protected:
    IdentityContainerRep* _rep;

};


class SubscriptionInstanceContainerRep;

class PEGASUS_COMMON_LINKAGE SubscriptionInstanceContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    SubscriptionInstanceContainer
        (const OperationContext::Container & container);
    SubscriptionInstanceContainer
        (const SubscriptionInstanceContainer & container);
    SubscriptionInstanceContainer(const CIMInstance & subscriptionInstance);
    virtual ~SubscriptionInstanceContainer(void);

    SubscriptionInstanceContainer & operator=
        (const SubscriptionInstanceContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    CIMInstance getInstance(void) const;

protected:
    SubscriptionInstanceContainerRep* _rep;

};

class SubscriptionFilterConditionContainerRep;

class PEGASUS_COMMON_LINKAGE SubscriptionFilterConditionContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    SubscriptionFilterConditionContainer
        (const OperationContext::Container & container);
    SubscriptionFilterConditionContainer
        (const SubscriptionFilterConditionContainer & container);
    SubscriptionFilterConditionContainer
        (const String & filterCondition, 
        const String & queryLanguage);
    virtual ~SubscriptionFilterConditionContainer(void);

    SubscriptionFilterConditionContainer & operator=
        (const SubscriptionFilterConditionContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    String getFilterCondition(void) const;
    String getQueryLanguage(void) const;

protected:
    SubscriptionFilterConditionContainerRep* _rep;

};

class SubscriptionInstanceNamesContainerRep;

class PEGASUS_COMMON_LINKAGE SubscriptionInstanceNamesContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    SubscriptionInstanceNamesContainer
        (const OperationContext::Container & container);
    SubscriptionInstanceNamesContainer
        (const SubscriptionInstanceNamesContainer & container);
    SubscriptionInstanceNamesContainer
        (const Array<CIMObjectPath> & subscriptionInstanceNames);
    virtual ~SubscriptionInstanceNamesContainer(void);

    SubscriptionInstanceNamesContainer & operator=
        (const SubscriptionInstanceNamesContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    Array<CIMObjectPath> getInstanceNames(void) const;

protected:
    SubscriptionInstanceNamesContainerRep* _rep;

};


class PEGASUS_COMMON_LINKAGE TimeoutContainer : virtual public OperationContext::Container
{
   public:
      static const String NAME;
      
      TimeoutContainer(const OperationContext::Container & container);
      TimeoutContainer(Uint32 timeout);
      virtual String getName(void) const;
      virtual OperationContext::Container * clone(void) const;
      virtual void destroy(void);
      
      Uint32 getTimeOut(void) const;
   protected:
      Uint32 _value;
   private:
      TimeoutContainer(void);
};


// l10n
/////////////////////////////////////////////////////////////////////////////
// Containers used for globalization
/////////////////////////////////////////////////////////////////////////////

class AcceptLanguageListContainerRep;

class PEGASUS_COMMON_LINKAGE AcceptLanguageListContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    AcceptLanguageListContainer
        (const OperationContext::Container & container);
    AcceptLanguageListContainer
        (const AcceptLanguageListContainer & container);
    AcceptLanguageListContainer(const AcceptLanguages & languages);
    virtual ~AcceptLanguageListContainer(void);

    AcceptLanguageListContainer & operator=
        (const AcceptLanguageListContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    AcceptLanguages getLanguages(void) const;

protected:
   AcceptLanguageListContainerRep* _rep;

};


class PEGASUS_COMMON_LINKAGE SubscriptionLanguageListContainer
    : virtual public AcceptLanguageListContainer
{
public:
    static const String NAME;
    
    SubscriptionLanguageListContainer
        (const OperationContext::Container & container)
        : AcceptLanguageListContainer(container)
        {};
    SubscriptionLanguageListContainer
        (const SubscriptionLanguageListContainer & container)
        : AcceptLanguageListContainer(container)
        {};
    SubscriptionLanguageListContainer(const AcceptLanguages & languages)
        : AcceptLanguageListContainer(languages)
        {};
    virtual ~SubscriptionLanguageListContainer(void) {};        

    virtual String getName(void) const;
};    

class ContentLanguageListContainerRep;

class PEGASUS_COMMON_LINKAGE ContentLanguageListContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    ContentLanguageListContainer
        (const OperationContext::Container & container);
    ContentLanguageListContainer
        (const ContentLanguageListContainer & container);
    ContentLanguageListContainer(const ContentLanguages & languages);
    virtual ~ContentLanguageListContainer(void);

    ContentLanguageListContainer & operator=
        (const ContentLanguageListContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    ContentLanguages getLanguages(void) const;

protected:
   ContentLanguageListContainerRep* _rep;

};

class SnmpTrapOidContainerRep;

class PEGASUS_COMMON_LINKAGE SnmpTrapOidContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    SnmpTrapOidContainer
        (const OperationContext::Container & container);
    SnmpTrapOidContainer
        (const SnmpTrapOidContainer & container);
    SnmpTrapOidContainer(const String & snmpTrapOid);
    virtual ~SnmpTrapOidContainer(void);

    SnmpTrapOidContainer & operator=
        (const SnmpTrapOidContainer & container);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    String getSnmpTrapOid(void) const;

protected:
    SnmpTrapOidContainerRep* _rep;

};


PEGASUS_NAMESPACE_END

#endif
