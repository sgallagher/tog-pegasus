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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Clients/repupgrade/SSPModuleTable.h>

PEGASUS_USING_STD;

PEGASUS_USING_PEGASUS;

static char const* UPGRADE_TEST_FILE_LIST[] =
{
    "test#repupgrade/qualifiers/ASSOCIATION",
    "test#repupgrade/qualifiers/Description",
    "test#repupgrade/qualifiers/Key",
    "test#repupgrade/qualifiers/Version",
    "test#repupgrade/classes/TST_LabeledLineage.#",
    "test#repupgrade/classes/TST_LabeledLineageDynamic.#",
    "test#repupgrade/classes/TST_Lineage.#",
    "test#repupgrade/classes/TST_LineageDynamic.#",
    "test#repupgrade/classes/TST_LineageDynamicSubClass.#",
    "test#repupgrade/classes/TST_Person.#",
    "test#repupgrade/classes/TST_PersonDynamic.TST_Person",
    "test#repupgrade/classes/TST_PersonDynamicSubClass.TST_PersonDynamic",
    "test#repupgrade/classes/TST_PersonS.TST_Person",
    "test#repupgrade/classes/associations",
    "test#repupgrade/instances/TST_Person.idx",
    "test#repupgrade/instances/TST_Person.instances",
    "test#repupgrade/instances/TST_PersonS.idx",
    "test#repupgrade/instances/TST_PersonS.instances",
};

int main(int argc, char** argv)
{
    String 	oldRepositoryPath = argv[1];
    String 	newRepositoryPath = argv[2];
    Boolean 	fileFound 	  = false;
    Boolean 	fileNotFound 	  = false;
    Uint32  	i		  = 0;
    Uint32 	count             = 0;
    static 	char* verbose;

    verbose = getenv("PEGASUS_TEST_VERBOSE");
    count = sizeof(UPGRADE_TEST_FILE_LIST)/sizeof(UPGRADE_TEST_FILE_LIST[0]);

    for ( i = 0; i < count  && fileNotFound == false ; i++ )
    {
        String fileName = newRepositoryPath + "/" + UPGRADE_TEST_FILE_LIST[i];

        if (verbose)
        {
            cout << "Now checking for file : " 
                 << UPGRADE_TEST_FILE_LIST[i] << endl;
        }
        //
        // Check if the file has been created in the new repository.
        //
        if (!FileSystem::exists(fileName))
        {
            fileNotFound = true;

            if (verbose)
            {
                cout << "Failed to find file : " << fileName 
                     << " in the new repository. " << endl;
            }
        }
    }
 
    if (fileNotFound)
    {
        cout << "Upgrade test failed. " << endl;
        return 1;
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}
