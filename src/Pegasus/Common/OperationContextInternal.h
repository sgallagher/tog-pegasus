//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationContextInternal_h
#define Pegasus_OperationContextInternal_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

// This class should be moved to the OperationContext module when localization
// support is added to Pegasus and this class definition is firm.
class PEGASUS_COMMON_LINKAGE LocaleContainer
    :
#ifndef PEGASUS_USE_DEPRECATED_INTERFACES // include if NOT using deprecated API
      virtual
#endif
              public OperationContext::Container
{
public:
    static const String NAME;

    LocaleContainer(const OperationContext::Container & container);
    LocaleContainer(const String & languageId);
    virtual ~LocaleContainer(void);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    String getLanguageId(void) const;

protected:
    String _languageId;

};

class PEGASUS_COMMON_LINKAGE ProviderIdContainer
    :
#ifndef PEGASUS_USE_DEPRECATED_INTERFACES // include if NOT using deprecated API
      virtual
#endif
              public OperationContext::Container
{
public:
    static const String NAME;

    ProviderIdContainer(const OperationContext::Container & container);
    ProviderIdContainer(const CIMInstance & module, const CIMInstance & provider);
    virtual ~ProviderIdContainer(void);

    virtual String getName(void) const;
    virtual OperationContext::Container * clone(void) const;
    virtual void destroy(void);

    CIMInstance getModule(void) const;
    CIMInstance getProvider(void) const;

protected:
    CIMInstance _module;
    CIMInstance _provider;

};

PEGASUS_NAMESPACE_END

#endif
