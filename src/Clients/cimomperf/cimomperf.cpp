//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

/* This is a simplistic display program for the CIMOM performance characteristics.
	This version simply gets the instances of the performace class and displays
	the resulting average counts.
	TODO  KS
	1. Convert to use the correct class when it is available.
	2. Get the header information from the class, not fixed.
	3. Keep history and present so that there is a total
	4. Do Total so that we have overall counts.
	5. Do percentages
*/
#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/FileSystem.h>

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
    char* tmp = strcpy(new char[strlen(text) + 1], text);
    Uint32 count = 0;
    Uint32 indent = 0;
    Boolean quoteState = false;
    Boolean qualifierState = false;
    char c;
    char prevchar;
    while (c = *tmp++)
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

	    case '\"':	 // quote 
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
	{"location", "localhost:5988", false, Option::STRING, 0, 0, "-n",
                                        "specifies system and port" },
        
	{"namespace", "root/cimv2", false, Option::STRING, 0, 0, "-n",
                                        "specifies namespace to use for operation" },
        


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

    Boolean debug = (om.valueEquals("debug", "true")) ? true :false;
    
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

	String className = "IBM_CIMOMStatData";

    CIMClient client;

    try
    {
       	client.connect(location);
    } 
    
    catch(CIMClientException& e)
    {
	  cerr << argv[0] << " CIMClientException connecting to : " << location << endl;
	  cerr << e.getMessage() << endl;
    }
    catch(Exception &e) 
    {
	  cerr << argv[0] << " Internal Error during connect: " << e.getMessage() <<
	      " connecting to " << location << endl;
    }
	CIMClass performanceClass;
	try
	{
		performanceClass = client.getClass(nameSpace,
										   className,
										   false,
										   false,
										   false);
	}

    catch(CIMClientException& e)
    {
	  cerr << argv[0] << "CIMClientException getClass : " << className
		   << e.getMessage() << endl;
	  exit(1);
    }
    catch(Exception& e)
    {
		cerr  << argv[0]   << "Client Error getClass: "  << className 
			<< e.getMessage() << endl;
		exit(1);
    }
    try
    {
		Boolean localOnly = false;
		Boolean deepInheritance = false;
		Boolean includeQualifiers = false;
		Boolean includeClassOrigin = false;
		
		Array<CIMNamedInstance> namedInstances; 
		namedInstances = client.enumerateInstances(nameSpace,
							   className,
							   deepInheritance,
							   localOnly,
							   includeQualifiers,
							   includeClassOrigin);
		
		// Output a table with the values
		
		// First build the header from the values strings in the class
		printf("%-25s%10s %10s %10s %10s %10s\n%-25s%10s %10s %10s %10s %10s\n",
			   "CIM", "Number of", "CIMOM", "Provider",
			    "Request", "Response",
			   "Operation", "Requests", "Time", "Time", "Size", "Size");
		
		// Output the returned instances
		
		for (Uint32 i = 0; i < namedInstances.size(); i++)
		{
			CIMInstance instance = namedInstances[i].getInstance();
			if (debug) 
			{
				Array<Sint8> x;
				instance.toMof(x);
			
				x.append('\0');
			
				mofFormat(cout, x.getData(), 4);
			}
			// Build a line for every information entry.

			// Get the request type property for this instance
			// Note that for the moment it is simply an integer.
			Uint32 pos;
			CIMProperty p;

			String statName;
			CIMValue v;

			if ((pos = instance.findProperty("name")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::STRING)
				{
					v.get(statName);
				}
			}
			else
			{
				statName = "UNKNOWN";
			}

			// now get number of requests property "NumberofRequests"
			Uint64 numberOfRequests = 0;
			if ((pos = instance.findProperty("NumberofRequests")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::UINT64)
				{
					v.get(numberOfRequests);
				}
			}
			// Get the total CIMOM Time property "TotalCimomTime"
			Uint64 totalCimomTime = 0;
			Uint64 averageCimomTime = 0;

			if ((pos = instance.findProperty("TotalCimomTime")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::UINT64)
				{
					v.get(totalCimomTime);
				}
				else
					cerr << "Error Property value " << "TotalCimomTime" << endl;
			}
			else
				cerr << "Error Property " << "TotalCimomTime" << endl;

			if (totalCimomTime != 0) {
				averageCimomTime =  totalCimomTime / numberOfRequests;
			}
			// Get the total Provider Time property "TotalProviderTime"

			Uint64 totalProviderTime = 0; 
			Uint64 averageProviderTime = 0;

			if ((pos = instance.findProperty("TotalProviderTime")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::UINT64)
				{
					v.get(totalProviderTime);
				}
				else
					cerr << "Error Property Vlaue " << "TotalProviderTime" << endl;
			}
			else
				cerr << "Error Property " << "TotalCimomTime" << endl;

			if (totalCimomTime != 0) {
				averageProviderTime =  totalProviderTime / numberOfRequests;
			}
			// Get the total Response size property "TotalResponseSize"

			Uint64 totalResponseSize = 0;
			Uint64 averageResponseSize = 0;


			if ((pos = instance.findProperty("TotalResponseSize")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::UINT64)
				{
					v.get(totalResponseSize);
				}
			}
			if (totalCimomTime != 0) {
				averageResponseSize =  totalResponseSize / numberOfRequests;
			}
			//Get the total request size property "TotalRequestSize"

			Uint64 totalRequestSize = 0;
			Uint64 averageRequestSize = 0;


			if ((pos = instance.findProperty("TotalRequestSize")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMType::UINT64)
				{
					v.get(totalRequestSize);
				}
			}
			if (totalRequestSize != 0)
			{
				averageRequestSize = totalRequestSize / numberOfRequests;
			}
			char* pStatName = statName.allocateCString();
			// If there are requests made, output one line with the total
			//if (numberOfRequests > 0) {
				printf(" %-25s%9lu %10lu %10lu %10lu %10lu\n", pStatName,
					   numberOfRequests, averageCimomTime,
					   averageProviderTime, averageRequestSize, 
					   averageResponseSize);
			//}
			delete [] pStatName;
		}
    }
    catch(CIMClientException& e)
    {
	  cerr << argv[0] << "CIMClientException : " << className
			<< e.getMessage() << endl;

	  exit(1);
    }
    catch(Exception& e)
    {
		cerr  << argv[0]   << "Error: " << e.getMessage() << endl;
		exit(1);
    }

    return 0;
}

