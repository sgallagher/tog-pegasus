//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//				Willis White (whiwill@us.ibm.com)
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
#include <Pegasus/Common/Stopwatch.h>
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
    char prevchar;
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
        cout << "Config file from " << configFile << endl;
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


Uint64  dateToInt(CIMDateTime date)
{
//debug code
//printf("we are in the DateTimetomicrosec function\n");

	String date_str = date.toString();
	String day_str = date_str.subString(0,8);
	String hour_str = date_str.subString(8,2);
	String min_str = date_str.subString(10,2);
	String sec_str = date_str.subString(12,2);
	String mil_str = date_str.subString(15,6);
		


	Uint64 day_int = (Uint64) atoi(day_str.getCString());
//	printf("this is day as a integer %d \n",day_int);
	Uint64 hour_int = (Uint64) atoi(hour_str.getCString());
//	printf("this is hour as a integer %d \n",hour_int);
	Uint64 min_int = (Uint64) atoi(min_str.getCString());
//	printf("this is minute as a integer %d \n",min_int);
	Uint64 sec_int = (Uint64) atoi(sec_str.getCString());
//	printf("this is second as a integer %d \n",sec_int);
	Uint64 mil_int = (Uint64) atoi(mil_str.getCString());
//	printf("this is millisec as a integer %d \n",mil_int);


	const Uint64 ndays = day_int*static_cast<Uint64>(864)*100000000;     //one day = 8.64*10^10 millisecond
	const Uint64 nhour = hour_int*static_cast<Uint64>(36)*100000000;     //one hour = 3.6*10^9 milliseconds
	const Uint64 nmin = min_int*60000000;            // one minute = 6*10^7
	const Uint64 nsecond = sec_int*1000000;          //one second = 10^6 milliseconds
	const Uint64 milTime = ndays+nhour+nmin+nsecond+mil_int;
	
//	printf("this is what is being passed back %d",milTime);

return milTime;
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

        /* this does nothing
    :
    if (om.valueEquals("verbose", "true"))
    {
                printHelpMsg(argv[0], usage, extra, om);
		exit(0);
    }       */

    // Establish the namespace from the input parameters
    String nameSpace = "root/cimv2";

    /* if flag is taken out this section of code should be taken out
    if(om.lookupValue("namespace", nameSpace))
    {
       cout << "Namespace = " << nameSpace << endl;

    }  */

   /*      this code serves no purpose
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;

    Boolean debug = (om.valueEquals("debug", "true")) ? true :false; */

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
    if(om.lookupValue("location", location))
    {
        cout << "Location = " << location << endl;
    }
    

    //Get port number from (option manager) command line if none use defualt
    String str_port;
    Uint32 port;
    if(om.lookupValue("port", str_port))
    {
        port = (Uint32) atoi(str_port.getCString());
        cout << "Port = " << port << endl;
    }

    //Get user name and password
    String userN;
    String passW;
    om.lookupValue("user name", userN);

    if (userN == String::EMPTY) {
        cout << "user name eqauls String::EMPTY" << endl;
    }
    else
        cout << "user name equals " << userN << endl;

    om.lookupValue("pass word", passW);
    if (passW == String::EMPTY) {
        cout << "pass wrod eqauls String::EMPTY" << endl;
    }
    else
        cout << "pass word equals equals " << passW << endl;
    

 /****************************************************
 The next sectoin of code connects to the server and enumerates all the instances of the 
 CIM_CIMOMStatisticalData class. The instances are held in an Array named instances. The
 output of cimperf is a table of averages, the last piece of code in this section prints the 
 header of this table
 */


	String className = "CIM_CIMOMStatisticalData";
    CIMClient client;

	//printf("this is right before connect\n");
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
	CIMClass performanceClass;
	try
	{
		performanceClass = client.getClass(nameSpace,
										   className,
										   false,
										   false,
										   false);    
	}

    catch(Exception& e)
    {
	  cerr << argv[0] << "Exception getClass : " << className
		   << e.getMessage() << endl;
	  exit(1);
    }
    try
    {
	//	printf("right befoe enumerateInstances\n");
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

	//	printf("right before for loop\n");
       
		
		for (Uint32 inst = 0; inst < instances.size(); inst++)
		{
			CIMInstance instance = instances[inst];

			// Get the request type property for this instance
			// Note that for the moment it is simply an integer.

			Uint32 pos;
			CIMProperty p;
			String statName;
			CIMValue v;

//debug code
	//		printf("this is right before the first if statment\n");

			if ((pos = instance.findProperty("ElementName")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
				if (v.getType() == CIMTYPE_STRING)
				{
					v.get(statName);

					//***********debug
			//		printf("in ElementName getType\n");

				}
			}
			else
			{
				statName = "UNKNOWN";
			}

			//debug code
	//		printf("this is before Number of Operations check\n");

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
			Uint64 totalCT;

//debug code 
//printf("this is right before CimomElapsedTime part\n");

			if ((pos = instance.findProperty("CimomElapsedTime")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();

				//debug code
		//		printf("this is the CimomElapsedTime outer loop\n");

				if (v.getType() == CIMTYPE_DATETIME)
				{
					v.get(totalCimomTime);
					totalCT = dateToInt(totalCimomTime);

					//***********debug
			//		printf("this is the CimomElapsedTime inner loop\n");
				}
				else
					cerr << "Error Property value " << "CimomElapsedTime" << endl;
			}
			else
				cerr << "Error Property " << "CimomElapsedTime" << endl;

			/* this can be taken out

			// look in C++ class StatisticalData and get CIMOMElapsedTime in integer format
			// get the average and then use toCIMDateTime function of the statsicaldata class 
			// (not sure if this is needed)to convert the average into a CIMDateTime format

	   // 	StatisticalData* statd = StatisticalData::current();  this 

		//	printf("this is after getting statd and befor division\n");     */

			if ((totalCT == 0) || (numberOfRequests == 0))
			{
				averageCimomTime =0;
			}
			else
				averageCimomTime = totalCT/numberOfRequests;
		


	
			// Get the total Provider Time property "ProviderElapsedTime"
			CIMDateTime totalProviderTime; 
			Uint64 averageProviderTime = 0;
			Uint64 totalPT;

//			printf("this is after figuring averageCimomTime and before provider Elapsed time\n");

			if ((pos = instance.findProperty("ProviderElapsedTime")) != PEG_NOT_FOUND)
			{
			 	p = (instance.getProperty(pos));
				v = p.getValue();
                                if (v.getType() == CIMTYPE_DATETIME)
                                {
					v.get(totalProviderTime);
					totalPT = dateToInt(totalProviderTime);

					//***********debug
				//	printf("the providerElapsed time inner loop\n");
					
				}
				else
					cerr << "Error Property Vlaue " << "ProviderElapsedTime" << endl;
			}
			else
				cerr << "Error Property " << "ProviderElapsedTime" << endl;

			// look in C++ class StatisticalData and get ProviderElapsedTime in integer format
			// get the average and then use toCIMDateTime function of the statsicaldata class 
			// (not sure if this is needed)to convert the average into a CIMDateTime format

                            //I think this comment should be taken out


			if ((totalPT == 0) || (numberOfRequests == 0))
			{
				averageProviderTime = 0;
		//		printf("one of the values equal 0 for provider time\n");
			}
			else
			averageProviderTime = totalPT/numberOfRequests;

		
			// Get the total Response size property "ResponseSize"

			Uint64 totalResponseSize = 0;
			Uint64 averageResponseSize = 0;

	//		printf("this is before Response Size\n");

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
//				printf("the averageResponseSize is %d\n",averageResponseSize);
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

			if (totalRequestSize != 0)
			{
				averageRequestSize = totalRequestSize / numberOfRequests;

			//***********debug
			//		printf("the elementName is %d",averageRequestSize);
			}

 

	//if StatisticalData::copyGSD is FALSE this will only return 0's
 
			printf(" %-25s"
			       "%9"  PEGASUS_64BIT_CONVERSION_WIDTH "u"
			       "%10" PEGASUS_64BIT_CONVERSION_WIDTH "u"
			       "%10" PEGASUS_64BIT_CONVERSION_WIDTH "u"
			       "%10" PEGASUS_64BIT_CONVERSION_WIDTH "u"
			       "%10" PEGASUS_64BIT_CONVERSION_WIDTH "u\n",
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


