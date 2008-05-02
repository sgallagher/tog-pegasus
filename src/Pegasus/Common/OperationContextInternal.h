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

#ifndef Pegasus_OperationContextInternal_h
#define Pegasus_OperationContextInternal_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/ObjectNormalizer.h>

PEGASUS_NAMESPACE_BEGIN

// This class should be moved to the OperationContext module when localization
// support is added to Pegasus and this class definition is firm.
class PEGASUS_COMMON_LINKAGE LocaleContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    LocaleContainer(const OperationContext::Container& container);
    LocaleContainer(const String& languageId);
    virtual ~LocaleContainer();

    // NOTE: The compiler default implementation of the copy constructor
    // is used for this class.

    LocaleContainer& operator=(const LocaleContainer& container);

    virtual String getName() const;
    virtual OperationContext::Container* clone() const;
    virtual void destroy();

    String getLanguageId() const;

protected:
    String _languageId;

};

class PEGASUS_COMMON_LINKAGE ProviderIdContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    ProviderIdContainer(const OperationContext::Container& container);
    ProviderIdContainer(
        const CIMInstance& module,
        const CIMInstance& provider,
        Boolean isRemoteNameSpace = false,
        const String& remoteInfo = String::EMPTY);
    virtual ~ProviderIdContainer();

    // NOTE: The compiler default implementation of the copy constructor
    // is used for this class.

    ProviderIdContainer& operator=(const ProviderIdContainer& container);

    virtual String getName() const;
    virtual OperationContext::Container* clone() const;
    virtual void destroy();

    CIMInstance getModule() const;
    CIMInstance getProvider() const;
    Boolean isRemoteNameSpace() const;
    String getRemoteInfo() const;
    String getProvMgrPath() const;
    void setProvMgrPath(String &path);

protected:
    CIMInstance _module;
    CIMInstance _provider;
    Boolean _isRemoteNameSpace;
    String _remoteInfo;
    String _provMgrPath;

};

class PEGASUS_COMMON_LINKAGE CachedClassDefinitionContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    CachedClassDefinitionContainer(
        const OperationContext::Container& container);
    CachedClassDefinitionContainer(const CIMClass& cimClass);
    virtual ~CachedClassDefinitionContainer();

    // NOTE: The compiler default implementation of the copy constructor
    // is used for this class.

    CachedClassDefinitionContainer& operator=(
        const CachedClassDefinitionContainer& container);

    virtual String getName() const;
    virtual OperationContext::Container* clone() const;
    virtual void destroy();

    CIMClass getClass() const;

protected:
    CIMClass _cimClass;

};

class PEGASUS_COMMON_LINKAGE NormalizerContextContainer
    : virtual public OperationContext::Container
{
public:
    static const String NAME;

    NormalizerContextContainer(const OperationContext::Container& container);
    NormalizerContextContainer(AutoPtr<NormalizerContext>& context);
    NormalizerContextContainer(const NormalizerContextContainer& container);
    virtual ~NormalizerContextContainer();

    NormalizerContextContainer& operator=(
        const NormalizerContextContainer& container);

    virtual String getName() const;
    virtual OperationContext::Container* clone() const;
    virtual void destroy();

    NormalizerContext* getContext() const;

protected:
    AutoPtr<NormalizerContext> normalizerContext;

private:
    NormalizerContextContainer();
};

PEGASUS_NAMESPACE_END

#endif
