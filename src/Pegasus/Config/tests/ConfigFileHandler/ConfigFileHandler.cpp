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
// Author: Nag Boranna(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <Pegasus/Config/ConfigFileHandler.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


int main()
{
    try
    {
	Array<String> all;
	Array<String> values;

	ConfigFileHandler _config("cimserver_current.conf",  
            "cimserver_planned.conf", true);

	_config.loadAllConfigProperties();
	
	_config.getAllCurrentPropertyNames(all);

	for (int i = 0; i < all.size(); i++)
	{
            _config.getCurrentValue(all[i]); 
            _config.getPlannedValue(all[i]); 
	}

        all.clear();

	_config.getAllPlannedProperties(all, values);

	for (int i = 0; i < all.size(); i++)
	{
            _config.getPlannedValue(all[i]); 
	}

        all.clear();
        values.clear();

	_config.getAllCurrentProperties(all, values);

	for (int i = 0; i < all.size(); i++)
	{
            _config.getCurrentValue(all[i]); 
	}

    }
    catch (Exception& e)
    {
		cerr << "Exception: " << e.getMessage() << endl;
		exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}

