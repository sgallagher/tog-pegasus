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
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean verbose = false;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/cimv2");


int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ": started" << endl;

    String repositoryRootPath =
       ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("repositoryDir"));

    CIMRepository *_repository = new CIMRepository(repositoryRootPath);

    try {
       CIMNamespaceName ok1(WildCardNamespaceNames::add(String("root/ci*")));
       if (verbose) cout<<argv[0]<<" --- ok1: "<<ok1.getString()<<endl;

       CIMNamespaceName ok0(WildCardNamespaceNames::add(String("root/cimv2")));
       if (verbose) cout<<argv[0]<<" --- ok0: "<<ok0.getString()<<endl;

       CIMNamespaceName ok2(WildCardNamespaceNames::add(String("root/cim*")));
       if (verbose) cout<<argv[0]<<" --- ok2: "<<ok2.getString()<<endl;

       CIMNamespaceName ok3(WildCardNamespaceNames::add(String("root/ci*")));
       if (verbose) cout<<argv[0]<<" --- ok3: "<<ok1.getString()<<endl;

       CIMNamespaceName ok11(WildCardNamespaceNames::check(String("root/ci33")));
       if (verbose) cout<<argv[0]<<" --- ok11: "<<ok11.getString()<<endl;

       CIMNamespaceName ok12(WildCardNamespaceNames::check(String("root/cim3")));
       if (verbose) cout<<argv[0]<<" --- ok12: "<<ok12.getString()<<endl;

       const Array<String> ar=WildCardNamespaceNames::getArray();
       for (int i=0,m=ar.size(); i<m; i++ ) {
	 if (verbose) cout<<argv[0]<<" --- "<<i<<" "<<ar[i]<<endl;
       }

       Array<CIMNamespaceName> nss=_repository->enumerateNameSpaces();
       for (int i=0,m=nss.size(); i<m; i++ ) {
          if (verbose) cout<<argv[0]<<" --- "<<i<<" "<<nss[i].getString()<<endl;
       }

    }

    catch(Exception& e) {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	PEGASUS_STD (cout) << argv[0] << " +++++ modify instances failed"
                           << PEGASUS_STD (endl);
	exit(-1);
    }

    PEGASUS_STD(cout) << argv[0] <<  " +++++ passed all tests" << PEGASUS_STD(endl);

    exit (0);
}
