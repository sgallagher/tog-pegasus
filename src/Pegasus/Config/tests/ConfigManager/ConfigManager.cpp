//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
//
// Author: Nag Boranna, Hewlett-Packard Company
//         (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


int main()
{
    try
    {
	Array<String> all;
	Array<String> info;

	ConfigManager* _config;
	_config = ConfigManager::getInstance();

	_config->mergeConfigFiles();

	_config->getAllPropertyNames(all);

	for (int i = 0; i < all.size(); i++)
	{
		info.clear();
		_config->getPropertyInfo(all[i], info);

	}

	_config->updateCurrentValue(all[7], "New Current Value");
	_config->updatePlannedValue(all[7], "New Planned Value");

	for (int i = 0; i < all.size(); i++)
	{
                info.clear();
                _config->getPropertyInfo(all[i], info);

	}
    }
    catch (Exception& e)
    {
		cerr << "Exception: " << e.getMessage() << endl;
		exit(1);
    }

    cout << "++++ passed all tests" << endl;

    return 0;
}

