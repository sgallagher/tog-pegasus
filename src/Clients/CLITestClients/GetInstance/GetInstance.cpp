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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Amit K Arora (amita@in.ibm.com) for Bug# 1081 (mofFormat())
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String DEFAULT_NAMESPACE = "root/cimv2";

static const char * usage = "blah blah";
static const char * extra = "more blah";


//------------------------------------------------------------------------------
//
// _indent()
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentSize)
{
    Uint32 n = level * indentSize;
    if (n > 50)
    {
    cout << "Jumped Ship " << level << " size " << indentSize << endl;
    exit(1);
    }

    for (Uint32 i = 0; i < n; i++)
    os << ' ';
}
void mofFormat(
    PEGASUS_STD(ostream)& os,
    const char* text,
    Uint32 indentSize)
{
    char* var = new char[strlen(text)+1];
    char* tmp = strcpy(var, text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar = 0;
    while ((c = *tmp++))
    {
    count++;
    // This is too simplistic and must move to a token based mini parser
    // but will do for now. One problem is tokens longer than 12 characters that
    // overrun the max line length.
    switch (c)
    {
        case '\n':
        os << Sint8(c);
        prevchar = c;
        count = 0 + (indent * indentSize);
        _indent(os, indent, indentSize);
        break;

        case '\"':   // quote
        os << Sint8(c);
        prevchar = c;
        quoteState = !quoteState;
        break;

        case ' ':
        os << Sint8(c);
        prevchar = c;
        if (count > 66)
        {
            if (quoteState)
            {
            os << "\"\n";
            _indent(os, indent + 1, indentSize);
            os <<"\"";
            }
            else
            {
            os <<"\n";
            _indent(os, indent + 1,  indentSize);
            }
            count = 0 + ((indent + 1) * indentSize);
        }
        break;
        case '[':
        if (prevchar == '\n')
        {
            indent++;
            _indent(os, indent,  indentSize);
            qualifierState = true;
        }
        os << Sint8(c);
        prevchar = c;
        break;

        case ']':
        if (qualifierState)
        {
            if (indent > 0)
            indent--;
            qualifierState = false;
        }
        os << Sint8(c);
        prevchar = c;
        break;

        default:
        os << Sint8(c);
        prevchar = c;
    }

    }
    delete [] var;
}

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
    {"location", "localhost:5988", false, Option::STRING, 0, 0, "l",
                                        "specifies system and port" },

    {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                                        "specifies namespace to use for operation" },

        // error here just went to temp nothing
        //{"outputformats", "mof", false, Option::STRING, outputFormats,
        //                                 NUM_OUTPUTFORMATS, "-o",
        //                                "Output in xml, mof, txt"},


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

    Boolean isXMLOutput = false;

    if(argc < 2)
    {
        String header = " ";
    String trailer = " ";
    cerr << "Instance Name required" << endl;
    om.printOptionsHelpTxt(header, trailer);
        exit(0);

    }

    if(argc > 2)
    {
        String header = " ";
    String trailer = " ";
    cerr << "Only one Instance Name allowed" << endl;
    om.printOptionsHelpTxt(header, trailer);
        exit(0);

    }

    // Note that this name is does not exist in the system
    CIMObjectPath reference = CIMObjectPath ("Process.pid=123456");

    String myReference = argv[1];
    try
    {
    reference = myReference;
    }
    catch(Exception& e)
    {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    exit(1);
    }
    CIMClient client;

    try
    {
        Uint32 index = location.find (':');
        String host = location.subString (0, index);
        Uint32 portNumber = 0;
        if (index != PEG_NOT_FOUND)
        {
            String portStr = location.subString (index + 1, location.size ());
            sscanf (portStr.getCString (), "%u", &portNumber);
        }
        client.connect (host, portNumber, String::EMPTY, String::EMPTY);
    }
    catch(Exception& e)
    {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    exit(1);
    }
    try
    {
        CIMInstance instance = client.getInstance(nameSpace, reference);

        if(isXMLOutput)
        XmlWriter::printInstanceElement(instance, cout);
        else
        {
        Array<char> x;
        MofWriter::appendInstanceElement(x, instance);

                x.append('\0');

                mofFormat(cout, x.getData(), 4);
            }

    }
    catch(Exception& e)
    {
    PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
    exit(1);
    }

    return 0;
}

