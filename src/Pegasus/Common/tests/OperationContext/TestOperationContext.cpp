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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(void)
{
    OperationContext context;

    try
    {
        String userName("Yoda");

        context.insert(IdentityContainer(userName));

        IdentityContainer container = context.get(IdentityContainer::NAME);

        if(userName != container.getUserName())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Identity Container failed" << endl;

        exit(1);
    }

    try
    {
        String languageId("en-US");

        context.insert(LocaleContainer(languageId));

        LocaleContainer container = context.get(LocaleContainer::NAME);

        if(languageId != container.getLanguageId())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Locale Container failed" << endl;

        exit(1);
    }

    try
    {
        CIMInstance module(CIMName ("PG_ProviderModule"));
        CIMInstance provider(CIMName ("PG_Provider"));

        context.insert(ProviderIdContainer(module, provider));

        ProviderIdContainer container = context.get(ProviderIdContainer::NAME);

        if(!module.identical(container.getModule()) || !provider.identical(container.getProvider()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Provider Id Container failed" << endl;

        exit(1);
    }

    try
    {
        OperationContext scopeContext;

        scopeContext = context;

        scopeContext.remove(IdentityContainer::NAME);
        scopeContext.remove(LocaleContainer::NAME);
        scopeContext.remove(ProviderIdContainer::NAME);
    }
    catch(...)
    {
    }

    cout << "+++++ passed all tests." << endl;

    exit(0);
}
