
//%////////////////////////////////////////////////////////////////////////////
//
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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/MofWriter.h>
#include "clientRepositoryInterface.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

String nameSpace = "root/cimv2";
/*
class classNameList
{
public:
    classNameList(String& nameSpace, clientRepositoryInterface& cli)
    {
    };
};
*/

/* class classNameList - Provides creation and management of classNames in a
    list form.
*/
class classNameList
{
public:
    // constructor - Creates empty list
    classNameList()
    {
    _nameSpace = String::EMPTY;
    //_cli = 0;
    _index = 0;
    }
    // constructor - with nameSpace and Interface
    classNameList(const String& nameSpace, const clientRepositoryInterface& cli)

    {
    _nameSpace = nameSpace,
    _cli = cli,
    _index = 0;
    }
    ~classNameList()
    {

    }
    // Set Namespace
    void setNameSpace (const String& nameSpac)
    {
	_nameSpace = nameSpace;
    }
    // enumerate - Executes the enumerate
    void enumerate(String& className, Boolean deepInheritance)
    {
    	/*cout << "enumerate function " << className 
	    << " namespace " << _nameSpace
	    << endl;*/
	// put try around this
	_classNameList =  _cli.enumerateClassNames(
	    _nameSpace, className, deepInheritance);
	//cout << "enum finished " << _classNameList.size() << endl;
    }

    /* filter - Filters the list against a defined pattern using the
        glob type filter.
       NOTICE: This method no longer performs glob-style matching.  The
       list is now filtered using exact matching, except that it also
       recognizes the "*" pattern which matches everything.  While the new
       functionality is quite limited, this change is consistent with
       the way this method was being used at the time of the change.
        @param pattern -String defining the pattern used to filter the
        list. 
        @return Result is the list with all names that do not pass the
        filter removed.
        
    */
    void filter(String& pattern)
    {
	// Filter the list in accordance with the pattern
	//cout << "Filter Start " << _classNameList.size() << endl;
	Array<String> tmp;
	for (Uint32 i = 0; i < _classNameList.size(); i++)
	{
	   /* cout << "Loop in filter " << i << " " 
		<<  _classNameList[i] << " pattern " 
		<<  pattern
		<< " Return is " << String::equalNoCase(_classNameList[i], pattern)
	        << " size " <<  _classNameList.size()
		<< endl; */
	    if (String::equalNoCase(_classNameList[i], pattern))
		tmp.append(_classNameList[i]);
	}
	_classNameList.swap(tmp);
      }
	//cout << "Filter finished " << _classNameList.size() << endl;

    /* getIndex - Get the current index in the list.  This is used with
        next and start functions to get entries in the list one by one.
    */
    Uint32 getIndex() {
	return _index;
    }
    /* size - returns the number of entires in the list
        @return Uint32 with number of entires in the list.
    */
    Uint32 size() {
	return _classNameList.size();
    }
    /* next - get the next entry in the list.  If there are no more entires
       returns String:EMPTY
       @return String containing next entry or String::EMPTY if at end of list
    */
    String next()
    {
	if (_index < _classNameList.size())
	    return _classNameList[_index++];
	else
	    return String::EMPTY;
    }
    /* start - Set the index to the start of the list
    */
    void start()
    {
        _index = 0;
    }
private:
    Array<String> _classNameList;
    clientRepositoryInterface _cli;
    String _nameSpace;
    Uint32 _index;
};

/** ErrorExit - Print out the error message as an
    and get out.
    @param - Text for error message
    @return - None, Terminates the program
    @execption - This function terminates the program
*/
void ErrorExit(const String& message)
{

    cerr << message << endl;
    exit(1);
}

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

///////////////////////////////////////////////////////////////
//    OPTION MANAGEMENT
///////////////////////////////////////////////////////////////

/** GetOptions function - This function defines the Options Table
    and sets up the options from that table using the option manager.
    const char* optionName;
    const char* defaultValue;
    int required;
    Option::Type type;
    char** domain;
    Uint32 domainSize;
    const char* commandLineOptionName;
    const char* optionHelpMessage;
    
*/
void GetOptions(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& pegasusHome)
{
    static struct OptionRow optionsTable[] =
    {
        //optionname defaultvalue rqd  type domain domainsize clname hlpmsg
         
         {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
                       "Specifies namespace to use for"},

         {"version", "false", false, Option::BOOLEAN, 0, 0, "v",
                       "Displays Program Version identification"},
         // set to true for testing
         {"verbose", "true", false, Option::BOOLEAN, 0, 0, "verbose",
                       "Displays Extra information"},

         {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
                       "Prints help message with command line options"},

         {"qualifiers", "false", false, Option::BOOLEAN, 0, 0, "q", 
                      "If set, show the qualifier declarations"},

         {"instances", "false", false, Option::BOOLEAN, 0, 0, "i", 
                      "If set, show the instances"},

         {"all", "false", false, Option::BOOLEAN, 0, 0, "a", 
                      "If set, show qualifiers, classes, and instances"},

         {"summary", "false", false, Option::BOOLEAN, 0, 0, "s", 
                      "Print only a summary count at end"},

         {"location", ".", false, Option::STRING, 0, 0, "l", 
        "Repository directory (/run if repository directory is /run/repository"},

         {"client", "false", false, Option::BOOLEAN, 0, 0, "c", 
                      "Runs as Pegasus client using client interface"},

         {"onlynames", "false", false, Option::BOOLEAN, 0, 0, "o", 
                      "Show Names only, not the MOF"},

         {"xml", "false", false, Option::BOOLEAN, 0, 0, "x", 
                      "Output result in XML rather than MOF"},

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = pegasusHome + "/cimserver.conf";

    if (FileSystem::exists(configFile))
    {
        om.mergeFile(configFile);
        cout << "Config file from " << configFile << endl;
    }

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();
}

void printHelp(char* name, OptionManager om)
{
    String header = "Usage ";
    header.append(name);
    header.append(" -parameters [class]");
    header.append("  - Generate MOF output from the repository or client\n");

    String trailer = "\nAssumes  PEGASUS_HOME for repository unless -r specified";
    trailer.append("\nClient location assumesIf class only that class mof is output.");

    trailer.append("\nIf class only that class mof is output.");
    trailer.append("\nExamples:");
    trailer.append("\n  tomof CIM_DOOR - Shows mof for CIM_Door");
    trailer.append("\n  tomof *door* - Shows mof for classes with 'door' in name.");
    trailer.append("\n  tomof -o *software* - Lists Class names with 'door' in name.");
    trailer.append("\n  tomof - outputs mof for all classes");
    trailer.append("\n  tomof -c - outputs mof for all classes using client interface.");
    trailer.append("\n  tomof -q - Outputs mof for qualifiers and classes");

    om.printOptionsHelpTxt(header, trailer);
}




///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{   

    String pegasusHome;
    pegasusHome = "/";

    // GetEnvironmentVariables(argv[0], pegasusHome);

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    // Get options (from command line and from configuration file); this
    // removes corresponding options and their arguments fromt he command
    // line.

    OptionManager om;

    try
    {
        GetOptions(om, argc, argv, pegasusHome);

    }
    catch(Exception& e)
    {
        cout << "Error Qualifier Enumeration:" << endl;
        cerr << argv[0] << ": " << e.getMessage() << endl;
        exit(1);
    }

    // Check to see if user asked for help (-h otpion):
    if(om.isTrue("help"))
    {
        printHelp(argv[0], om);
        exit (0);
    }
    // Check for Version flag and print version message
    if(om.isTrue("version"))
    {
        cout << "Pegasus Version " << PEGASUS_VERSION 
                << " " << argv[0] << " version 1.0 " << endl;
        return 0;
    }

    Boolean showOnlyNames = om.isTrue("onlynames");


    // Check for namespace flag and set namespace
    String localNameSpace;
    if(om.lookupValue("namespace", localNameSpace))
    {
        cout << "Namespace = " << localNameSpace << endl;
    }


    // Check for the show qualifiers flag
    Boolean showQualifiers = om.isTrue("qualifiers");

    // Check for the summary count flag
    Boolean summary = om.isTrue("summary");
    Uint32 qualifierCount = 0;
    Uint32 classCount = 0;
    Uint32 classCountDisplayed = 0;
    Uint32 instanceCount = 0;

    // Check for the instances 
    Boolean showInstances = om.isTrue("instances");

    Boolean showAll = om.isTrue("all");

    Boolean verbose = om.isTrue("verbose");

    Boolean singleClass = true;
    String className;

    // Set the flags to determine whether we show all or simply some classes
    if(showAll)
        singleClass = false;
    else    // if no classes in list.
        if(argc < 2)
        singleClass = false;

    // Build the classList from the input parameters
    Array<String> classList;

    if(argc > 1)
    {
        // while there are entries in the argv list
        for(int i = 1; i < argc; i++)
            classList.append(argv[i]);
    }
    else
        classList.append("*");

    if(verbose)
    {
        if(verbose) cout << "Class list is ";
        for(Uint32 i = 0 ; i < classList.size(); i++)
        {
            if(verbose) cout << classList[i] << " ";
        }
        cout << endl;
    }

    // Test to determine if we have a filter pattern to limit classes displayed
    String pattern;
    Boolean doWildCard = false;

    // if "Client" set, open a connected and set 
    Boolean isClient = om.isTrue("client");

    // if "Client" set, open a connected and set 
    Boolean isXMLOutput = om.isTrue("xml");

    // Check for repository path flag and set repository directory
    // ATTN: P4 Defer Apr 2002KS Clean this up so the priority is better defined.
    // Should probably also put under directory functions also
    String location = "";

    if(om.lookupValue("location", location))
    {
        if(verbose) cout << "location Path = " << location << endl;

    }

    const char* tmp = getenv("PEGASUS_HOME");


    // Need to set the location value
    // clientCommonDefs::operationType _ot;
    clientRepositoryInterface clRepository; // the repository interface object
    clientRepositoryInterface::_repositoryType rt;

    if(isClient)
    {
        location = "localhost:5988";
        rt = clientRepositoryInterface::REPOSITORY_INTERFACE_CLIENT;
    }
    else
    {
        rt = clientRepositoryInterface::REPOSITORY_INTERFACE_LOCAL;
        location += "/repository";
        location = location;
    }

    // How do we set a context that would be either client or repository
    // Test to determine if we have proper location, etc. for opening

    if(verbose)
    {
        cout << "Get from " << ((isClient) ? "client" : "repository ") 
        << " at " << location << endl;
    }

    // Create the repository object
    CIMRepository repository(location);
    CIMClient client;

    // if client mode, do client connection, else do repository connection
    try
    {
        clRepository.init(rt, location);
    }
    catch(Exception &e)
    {
        // add message here
        return false;
    }
    // Get the complete class name list	before we start anything else

    if(showQualifiers || showAll || summary)
    {
        try
        {
            // Enumerate the qualifiers:

            Array<CIMQualifierDecl> qualifierDecls 
                    = clRepository.enumerateQualifiers(nameSpace);
            qualifierCount = qualifierDecls.size();

            if(showOnlyNames)
            {
                for(Uint32 i = 0; i < qualifierDecls.size(); i++)
                    cout << "Qualifier " << qualifierDecls[i].getName() << endl;    
            }
            if(showQualifiers || showAll)
            {
                for(Uint32 i = 0; i < qualifierDecls.size(); i++)
                {
                    CIMQualifierDecl tmp = qualifierDecls[i];

                    if(isXMLOutput)
                    {
                        XmlWriter::printQualifierDeclElement(tmp, cout);            
                    }
                    else
                    {
                        Array<Sint8> x;
                        MofWriter::appendQualifierDeclElement(x, tmp);

                        x.append('\0');

                        mofFormat(cout, x.getData(), 4);
                    }
                }
            }
        }
        catch(Exception& e)
        {
            ErrorExit(e.getMessage());
        }
    }
    // Show classes from the list of input classes
    for(Uint32 i = 0; i < classList.size(); i++)
    {
        try
        {
            Boolean localOnly = true;
            Boolean includeQualifiers = true;
            Boolean includeClassOrigin = true;

            classNameList list(nameSpace, clRepository);

            String temp = "";
            list.enumerate(temp,true);

            // Filter to this class specification
            list.filter(classList[i]);
            classCount = list.size();

            if(showOnlyNames)
            {
                for(Uint32 j = 0; j < list.size(); j++)
                    cout << "Class " << list.next();
            }
            if(!summary)
            {
                // Print out the MOF for those found
                for(Uint32 j = 0; j < list.size(); j++)
                {

                    CIMClass cimClass = clRepository.getClass(nameSpace, list.next(),
                                                              localOnly, includeQualifiers, includeClassOrigin);

                    // Note we get and print ourselves rather than use the generic printMof
                    if(isXMLOutput)
                        XmlWriter::printClassElement(cimClass, cout);
                    else
                    {
                        Array<Sint8> x;
                        MofWriter::appendClassElement(x, cimClass);

                        x.append('\0');

                        mofFormat(cout, x.getData(), 4);
                    }
                }
            }
        }
        catch(Exception& e)
        {
            // ErrorExit(e.getMessage());
            cout << "Class get error " << e.getMessage() << " Class " << classList[i];
        }
    }

    // Note that we can do this so we get all instances or just the given class

    if(showInstances | showAll)
    {
        for(Uint32 i = 0; i < classList.size(); i++)
        {
            // Get Class Names
            Array<String> classNames;

            // try Block around basic instance processing
            try
            {
                Boolean deepInheritance = true;
                Boolean localOnly = false;
                Boolean includeClassOrigin = false;
                Boolean includeQualifiers = false;

                String className = "";

                //	classNames = clRepository.enumerateClassNames(
                //		nameSpace, className, deepInheritance);
                // Start with List from class enum
                classNameList myClassNameList(nameSpace, clRepository);

                String temp = "";
                myClassNameList.enumerate(temp,true);

                // Filter to this class specification
                myClassNameList.filter(classList[i]);
                classCount = myClassNameList.size();

                if(showOnlyNames)
                {
                    for(Uint32 j = 0; j < myClassNameList.size(); j++)
                    {
                        Array<CIMReference> instanceNames;
                        instanceNames = clRepository.enumerateInstanceNames(nameSpace,
                                                                            className);
                        for(Uint32 j = 0; j < instanceNames.size(); j++)
                            cout << "Instance " << instanceNames[i];
                    }
                }
                else    // Process complete instances
                {
                    // Process classlist to enumerate and print instances
                    for(Uint32 j = 0; j < myClassNameList.size(); j++)
                    {
                        Array<CIMNamedInstance> namedInstances;
                        namedInstances = clRepository.enumerateInstances(nameSpace,
                                                                         className,
                                                                         deepInheritance,
                                                                         localOnly,
                                                                         includeQualifiers,
                                                                         includeClassOrigin);
                        // const CIMPropertyList& propertyList = CIMPropertyList());

                        // Process and output each instance
                        for(Uint32 k = 0; k < namedInstances.size(); k++)
                        {

                            CIMInstance instance = namedInstances[i].getInstance();
                            if(isXMLOutput)
                                XmlWriter::printInstanceElement(instance, cout);
                            else
                            {
                                Array<Sint8> x;
                                MofWriter::appendInstanceElement(x, instance);

                                x.append('\0');

                                mofFormat(cout, x.getData(), 4);
                            }
                        }
                    }
                }
            }
            catch(Exception& e)
            {
                cout << "Error Class Name Enumeration:" << endl;
                cout << e.getMessage() << endl;
            }

            // Get instances for each class
            /*  Double check.  Think this just for delete now.
            try
            {
                for(Uint32 i = 0; i < classNames.size(); i++)
                {
                    // Enumerate the Instances of this class
                    // ENUM and DISPLAY CODE HERE
                    //ATTN: KS P3 17 APR 2002 Instance display incomplete
                    cout << "WORK In Process" << endl;
                }
            }
            catch(Exception& e)
            {
                cout << "Error Instance Enumeration:" << endl;
                cout << e.getMessage() << endl;
            }
            */
        }
    }

    if(summary)
    {
        if(qualifierCount != 0)
            cout << "Qualifiers - " << qualifierCount << endl;
        if(classCount != 0)
            cout << "Classes - " << classCount << " found and " << classCountDisplayed
                    << " output" << endl;
        if(instanceCount != 0)
            cout << "Instances - " << instanceCount << endl;
    }
    exit(0); 
} 
//PEGASUS_NAMESPACE_END


