//%//-*-c++-*-//////////////////////////////////////////////////////////////////
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationContext_h
#define Pegasus_OperationContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

#define CONTEXT_EMPTY                       0x00000000
#define CONTEXT_IDENTITY                    0x00000001
#define CONTEXT_AUTHENICATION               0x00000002
#define CONTEXT_AUTHORIZATION               0x00000004
#define CONTEXT_OTHER_SECURITY              0x00000008
#define CONTEXT_LOCALE                      0x00000010
#define CONTEXT_OPTIONS                     0x00000020
#define CONTEXT_VENDOR                      0x00000040
#define CONTEXT_UID_PRESENT                 0x00000080
#define CONTEXT_UINT32_PRESENT              0x00000100
#define CONTEXT_OTHER                       0x00000200
#define CONTEXT_COPY_MEMORY                 0x00000400
#define CONTEXT_DELETE_MEMORY               0x00000800
#define CONTEXT_POINTER                     0x00001000

#define OPERATION_NONE                      0x00000000
#define OPERATION_LOCAL_ONLY                0x00000001
#define OPERATION_INCLUDE_QUALIFIERS        0x00000002
#define OPERATION_INCLUDE_CLASS_ORIGIN      0x00000004
#define OPERATION_DEEP_INHERITANCE          0x00000008
#define OPERATION_PARTIAL_INSTANCE          0x00000010
#define OPERATION_REMOTE_ONLY               0x00000020
#define OPERATION_DELIVER                   0x00000040
#define OPERATION_RESERVE                   0x00000080
#define OPERATION_PROCESSING                0x00000100
#define OPERATION_COMPLETE                  0x00000200

class PEGASUS_COMMON_LINKAGE OperationContext
{
public:
    class PEGASUS_COMMON_LINKAGE Container
    {
    public:
        ///
        Container(const Uint32 key);

        virtual ~Container(void);

        ///
        const Uint32 & getKey(void) const;

    protected:
        Uint32 _key;

    };

public:
    ///
    OperationContext(void);

    ///
    OperationContext(const OperationContext & context);

    ///
    virtual ~OperationContext(void);

    ///
    void clear(void);

    ///
    const Container get(const Uint32 key) const;

    ///
    void set(const Container & container);

    ///
    void insert(const Container & container);

    ///
    void remove(const Uint32 key);

protected:
    Array<Container> _containers;

};

inline const Uint32 & OperationContext::Container::getKey(void) const
{
    return(_key);
}

class PEGASUS_COMMON_LINKAGE IdentityContainer : public OperationContext::Container
{
public:
    IdentityContainer(const String & userName);
    virtual ~IdentityContainer(void);

    String getUserName(void) const;

protected:
    String _userName;

};

class PEGASUS_COMMON_LINKAGE LocaleContainer : public OperationContext::Container
{
public:
    LocaleContainer(const String & languageId);
    virtual ~LocaleContainer(void);

    String getLanguageId(void) const;

protected:
    String _languageId;

};

PEGASUS_NAMESPACE_END

#endif
