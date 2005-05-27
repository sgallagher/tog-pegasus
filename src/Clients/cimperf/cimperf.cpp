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
//                (carolann_graves@hp.com)
//              Amit K Arora (amita@in.ibm.com) for Bug# 1081 (mofFormat())
//              Willis White (whiwill@us.ibm.com)
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for Bug#3449
//              Aruran, IBM(ashanmug@in.ibm.com) for Bug# 3684
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
#include <stdlib.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/CIMDateTime.h>

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
    char prevchar='\0';
    while ((c = *tmp++) != '\0')
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


/* method to build an OptionManager object - which holds and organizes options and the properties */

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
        //The values in the OptionRows below are:
        //optionname, defaultvalue, is required, type, domain, domainsize, flag, hlpmsg
    {
        {"port", "5988", false, Option::INTEGER, 0, 0, "p",
                                        "specifies port" },

        {"location", "localhost", false, Option::STRING, 0, 0, "l",
                                        "specifies hostname of system" },

        {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
                                        "Displays software Version "},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                            "Prints help message with command line options "},

        {"help1", "false", false, Option::BOOLEAN, 0, 0, "-help",
                            "Prints help message with command line options "},

        {"user name","",false,Option::STRING, 0, 0, "u",
                             "specifies user loging in"},

        {"pass word","",false,Option::STRING, 0, 0, "pw",
                             "login pass word for user"},

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    //We want to make this code common to all of the commands

    String configFile = "/CLTest.conf";

    if (FileSystem::exists(configFile))
    {
        om.mergeFile(configFile);
    }


    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

/*void printHelp(char* name, OptionManager om)
{
    String header = "Usage ";
    header.append(name);

    //om.printOptionsHelpTxt(header, trailer);
}       */

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
         String header = "Usage ";
         String trailer = "";
         om.printOptionsHelpTxt(header, trailer);
         exit(1);
    }


    // Establish the namespace from the input parameters
    String nameSpace = "root/cimv2";


    // Check to see if user asked for help (-h or --help otpion)
    if (om.valueEquals("help", "true") || om.valueEquals("help1", "true"))
    {
        String header = "Usage ";
        String trailer = "";
        om.printOptionsHelpTxt(header, trailer);
        exit(0);
    }


    //Get hostname form (option manager) command line if none use default
    String location;
    om.lookupValue("location", location);


    //Get port number from (option manager) command line if none use defualt
    String str_port;
    Uint32 port;
    if(om.lookupValue("port", str_port))
    {
        port = (Uint32) atoi(str_port.getCString());
    }

    //Get user name and password
    String userN;
    String passW;
    om.lookupValue("user name", userN);
    om.lookupValue("pass word", passW);

 /****************************************************
 The next sectoin of code connects to the server and enumerates all the instances of the
 CIM_CIMOMStatisticalData class. The instances are held in an Array named instances. The
 output of cimperf is a table of averages, the last piece of code in this section prints the
 header of this table
 */


    String className = "CIM_CIMOMStatisticalData";
    CIMClient client;

    try
    {
        client.connect(location, port, userN, passW);
    }

    catch(Exception& e)
    {
      cerr << argv[0] << " Exception connecting to : " << location << endl;
      cerr << e.getMessage() << endl;
      exit(1);
    }


    try
    {
    //  printf("right befoe enumerateInstances\n");
        Boolean localOnly = false;
        Boolean deepInheritance = false;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;

        Array<CIMInstance> instances;
        instances = client.enumerateInstances(nameSpace,
                               className,
                               deepInheritance,
                               localOnly,
                               includeQualifiers,
                               includeClassOrigin);




        // First print the header for table of values
        printf("%-25s%10s %10s %10s %10s %10s\n%-25s%10s %10s %10s %10s %10s\n",
               "CIM", "Number of", "CIMOM", "Provider",
                "Request", "Response",
               "Operation", "Requests", "Time", "Time", "Size", "Size");



  /*****************************************************************************
  This section of code loops through all the instances of CIM_CIMOMStatisticalData
  (one for each intrinsic request type) and gathers the NumberofOperations, CIMOMElapsedTime,
   ProviderElapsedTime, ResponseSize and RequestSize for each instance. Averages are abtained
   be dividing times and sizes by NumberofOperatons.
  */



        for (Uint32 inst = 0; inst < instances.size(); inst++)
        {
            CIMInstance instance = instances[inst];

            // Get the request type property for this instance
            // Note that for the moment it is simply an integer.

            Uint32 pos;
            CIMProperty p;
            String statName;
            CIMValue v;


            if ((pos = instance.findProperty("ElementName")) != PEG_NOT_FOUND)
            {
                p = (instance.getProperty(pos));
                v = p.getValue();
                if (v.getType() == CIMTYPE_STRING)
                {
                    v.get(statName);

            
                }
            }
            else
            {
                statName = "UNKNOWN";
            }

 
            //get number of requests property - "NumberofOperations"
            Uint64 numberOfRequests = 0;
            if ((pos = instance.findProperty("NumberOfOperations")) != PEG_NOT_FOUND)
            {

                p = (instance.getProperty(pos));
                v = p.getValue();

                if (v.getType() == CIMTYPE_UINT64)
                {
                    v.get(numberOfRequests);

                }
                else
                    cerr << "NumberofOperations was not a CIMTYPE_SINT64 and should be" << endl;
            }
            else
                cerr << "Could not find NumberofOperations" << endl;

            // Get the total CIMOM Time property "CIMOMElapsedTime"
            CIMDateTime totalCimomTime;
            Sint64 averageCimomTime = 0;
            Uint64 totalCT=0;

//debug code
//printf("this is right before CimomElapsedTime part\n");

            if ((pos = instance.findProperty("CimomElapsedTime")) != PEG_NOT_FOUND)
            {
                p = (instance.getProperty(pos));
                v = p.getValue();

                //debug code
            //printf("this is the CimomElapsedTime outer loop\n");

                if (v.getType() == CIMTYPE_DATETIME)
                {
                    v.get(totalCimomTime);
                    totalCT = totalCimomTime.toMicroSeconds();

                    //***********debug
            //      printf("this is the CimomElapsedTime inner loop\n");
                }
                else
                    cerr << "Error Property value " << "CimomElapsedTime" << endl;
            }
            else
                cerr << "Error Property " << "CimomElapsedTime" << endl;

            
            if ((totalCT == 0) || (numberOfRequests == 0))
            {
                averageCimomTime =0;
            }
            else
                averageCimomTime = totalCT/numberOfRequests;




            // Get the total Provider Time property "ProviderElapsedTime"
            CIMDateTime totalProviderTime;
            Uint64 averageProviderTime = 0;
            Uint64 totalPT=0;

         //printf("this is after figuring averageCimomTime and before provider Elapsed time\n");

            if ((pos = instance.findProperty("ProviderElapsedTime")) != PEG_NOT_FOUND)
            {
                p = (instance.getProperty(pos));
                v = p.getValue();
                                if (v.getType() == CIMTYPE_DATETIME)
                                {
                    v.get(totalProviderTime);
                    totalPT = totalProviderTime.toMicroSeconds();

                    //***********debug
                //  printf("the providerElapsed time inner loop\n");

                }
                else
                    cerr << "Error Property Vlaue " << "ProviderElapsedTime" << endl;
            }
            else
                cerr << "Error Property " << "ProviderElapsedTime" << endl;


            if ((totalPT == 0) || (numberOfRequests == 0))
            {
                averageProviderTime = 0;
            }
            else
            averageProviderTime = totalPT/numberOfRequests;


            // Get the total Response size property "ResponseSize"

            Uint64 totalResponseSize = 0;
            Uint64 averageResponseSize = 0;


            if ((pos = instance.findProperty("ResponseSize")) != PEG_NOT_FOUND)
            {
                p = (instance.getProperty(pos));
                v = p.getValue();

                if (v.getType() == CIMTYPE_UINT64)
                {
                    v.get(totalResponseSize);

                    //***********debug
                    //printf("the totalResponsSize innner loop \n");
                }
                else
                    cerr << "RequestSize is not of type CIMTYPE_SINT64" << endl ;
            }
            else
                cerr << "Could not find ResponseSize property" << endl;

            if ((totalCimomTime != 0) && (totalResponseSize != 0)) {
                averageResponseSize =  totalResponseSize / numberOfRequests;


                //***********debug
              //printf("the averageResponseSize is %d\n",averageResponseSize);
            }
            else
  
            totalResponseSize = 0;
            //Get the total request size property "RequestSize"

            Uint64 totalRequestSize = 0;
            Uint64 averageRequestSize = 0;


            if ((pos = instance.findProperty("RequestSize")) != PEG_NOT_FOUND)
            {
                p = (instance.getProperty(pos));
                v = p.getValue();

                if (v.getType() == CIMTYPE_UINT64)
                {
                    v.get(totalRequestSize);

                    //***********debug
                    //printf("the totalRequestSize inner loop\n");
                }
                else
                    cerr << "RequestSize is not of type CIMTYPE_SINT64" << endl ;
            }
            else
                cerr << "Could not find RequestSize property" << endl;

            if (numberOfRequests != 0)
            {
                averageRequestSize = totalRequestSize / numberOfRequests;

            //***********debug
                 // printf("the elementName is %d",averageRequestSize);
            }



    //if StatisticalData::copyGSD is FALSE this will only return 0's

            printf("%-25s"
                   "%10"  PEGASUS_64BIT_CONVERSION_WIDTH "u"
                   "%11" PEGASUS_64BIT_CONVERSION_WIDTH "u"
                   "%11" PEGASUS_64BIT_CONVERSION_WIDTH "u"
                   "%11" PEGASUS_64BIT_CONVERSION_WIDTH "u"
                   "%11" PEGASUS_64BIT_CONVERSION_WIDTH "u\n",
               (const char*)statName.getCString(),
               numberOfRequests, averageCimomTime,
               averageProviderTime, averageRequestSize,
               averageResponseSize);
        }
    }
    catch(Exception& e)
    {
      cerr << argv[0] << "Exception : " << className
            << e.getMessage() << endl;

      exit(1);
    }

    return 0;
}


