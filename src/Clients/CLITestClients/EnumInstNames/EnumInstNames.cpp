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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String DEFAULT_NAMESPACE = "root/cimv2";

static const char * usage = "blah blah";
static const char * extra = "more blah";

void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& testHome)
{
    static const char* outputFormats[] = { "xml", "mof", "txt"};
    static const Uint32 NUM_OUTPUTFORMATS = sizeof(outputFormats) /
                                            sizeof(outputFormats[0]);

    static struct OptionRow optionsTable[] =
        //optionname defaultvalue rqd  type domain domainsize clname hlpmsg
    {
    {"location", "localhost:5988", false, Option::STRING, 0, 0, "-n",
                                        "specifies system and port" },

    {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
                                        "specifies namespace to use for operation" },

        //{"outputformats", "mof", false, Option::STRING, outputFormats,
        //                                 NUM_OUTPUTFORMATS, "o",
        //                                "Output in xml, mof, txt"},

        {"deepinheritance", "false", false, Option::BOOLEAN, 0, 0, "d",
                                        "If set does deep enum "},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
                                        "Displays software Version "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help message with command line options "},

        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
                     "Not Used "},
    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    //We want to make this code common to all of the commands

    String configFile = "/CLTest.conf";

    cout << "Config file from " << configFile << endl;

    if (FileSystem::exists(configFile))
             om.mergeFile(configFile);

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

// ATTN: KS Think I stuck these together.
void printHelp(char* name, OptionManager om)
{
    String header = "Usage ";
    header.append(name);

    //om.printOptionsHelpTxt(header, trailer);
}

/* PrintHelp - This is temporary until we expand the options manager to allow
   options help to be defined with the OptionRow entries and presented from
   those entries.
*/
void printHelpMsg(const char* pgmName, const char* usage, const char* extraHelp,
                OptionManager om)
{
    cout << endl << pgmName << endl;
    cout << "Usage: " << pgmName << endl << usage << endl;
    cout << endl;
    // ATTN: KS om.printHelp(const char* pgmName, OptionManager om);
    cout << extraHelp << endl;

}


int main(int argc, char** argv)
{

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;

    try
    {
         String testHome = ".";
                 GetOptions(om, argc, argv, testHome);
         // om.print();
    }
    catch (Exception& e)
    {
         cerr << argv[0] << ": " << e.getMessage() << endl;
         exit(1);
    }


    // Check to see if user asked for help (-h otpion):
    if (om.valueEquals("verbose", "true"))
    {
                printHelpMsg(argv[0], usage, extra, om);
        exit(0);
    }

    // Establish the namespace from the input parameters
    String nameSpace;
    if(om.lookupValue("namespace", nameSpace))
    {
       cout << "Namespace = " << nameSpace << endl;

    }

    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;
    /*
    Boolean activeTest = false;
    if (om.valueEquals("active", "true"))
         activeTest = true;
    */
    // Now develop the target instname from the arglist
    // For this one, at least one arguement is required
    // Not sure if this is true.
    // Think we should be able to get by with no argument.
    if (om.valueEquals("help", "true"))
    {
        String header = "Usage ";
        String trailer = "";
        om.printOptionsHelpTxt(header, trailer);
        exit(0);
    }

    String location =   "localhost:5988";
    if(om.lookupValue("location", location))
    {
       cout << "Location = " << location << endl;

    }

    // here we assume class name = null if none specified.
    String className = "";
    if (argc > 1)
    {
    className = argv[1];
    }

    CIMClient client;

    try
    {
        client.connect(location, String::EMPTY, String::EMPTY);
    }

    catch(Exception &e)
    {
      cerr << "Internal Error:" << e.getMessage() << endl;
    }


    Array<CIMObjectPath> instanceNames =
        client.enumerateInstanceNames(nameSpace, className);
    try
    {

        //simply output the list one per line for the moment.
    for (Uint32 i = 0; i < instanceNames.size(); i++)
        cout << instanceNames[i] << endl;
    }
    catch(Exception& e)
    {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    exit(1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

    return 0;
}
