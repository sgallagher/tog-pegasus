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
//
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By:
//      Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Config/ConfigManager.h>

#include <iostream>

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

static char * verbose = 0;

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    try
    {
        ConfigManager * _config = 0;

        _config = ConfigManager::getInstance();

        if(_config == 0)
        {
            throw Exception("ConfigManager::getInstance() failed.");
        }

        _config->useConfigFiles = true;

        _config->mergeConfigFiles();

        Array<String> propertyNames;

        _config->getAllPropertyNames(propertyNames, true);

        for(Uint32 i = 0, n = propertyNames.size(); i < n; i++)
        {
            Array<String> info;

            _config->getPropertyInfo(propertyNames[i], info);

            if(verbose)
            {
                cout << "property name = " << propertyNames[i] << endl;

                /*
                cout << "property info = ";

                for(Uint32 j = 0, m = info.size(); j < m; j++)
                {
                    cout << info[j] << " ";
                }

                cout << endl;
                */
            }

            String currentValue = _config->getCurrentValue(propertyNames[i]);

            if(verbose)
            {
                cout << "current value = " << currentValue << endl;
            }

            String plannedValue = _config->getPlannedValue(propertyNames[i]);

            if(verbose)
            {
                cout << "planned value = " << plannedValue << endl;
            }
        }
    }
    catch(Exception& e)
    {
        cerr << "Exception: " << e.getMessage() << endl;

        return(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
