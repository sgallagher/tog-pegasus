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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//         Mike Day (mdday@us.ibm.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Susan Campbell, Hewlett-Packard Company (scampbell@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////



// Using the general CIMOM TestClient as an example, developed an
// osinfo client that does an enumerateInstances of the
// PG_OperatingSystem class and displays properties of interest.
// Using PG_OperatingSystem versus CIM_OperatingSystem to get
// SystemUpTime and OSCapability properties.

// At this time, there is only one instance (the running OS).  When
// add installed OSs, will need to select proper instance.

#include "OSInfo.h" 

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

#define NAMESPACE "root/cimv2"
#define CLASSNAME "PG_OperatingSystem"

/**  Constructor for OSInfo Client
  */

OSInfo::OSInfo(void)
{
   osCSName = String::EMPTY;
   osName = String::EMPTY;
   osVersion = String::EMPTY;
   osOtherInfo = String::EMPTY;
   osLicensedUsers = String::EMPTY;
   osCapability = String::EMPTY;
   osBootUpTime = String::EMPTY;
   osLocalDateTime = String::EMPTY;
   osSystemUpTime = String::EMPTY;

}

OSInfo::~OSInfo(void)
{
}         

/** ErrorExit - Print out the error message as an
    and get out.
    @param - Text for error message
    @return - None, Terminates the program
    @exception - This function terminates the program
*/
void OSInfo::errorExit(const String& message)
{
    cerr << "os info error: " << message << endl;
    cerr << "Re-run with verbose for details (osinfo -verbose)" <<endl;
    exit(1);
}


/**
   displayProperties method of the osinfo Test Client
  */
void OSInfo::displayProperties(Boolean verboseTest)
{
   // don't have a try here - want it to be caught by caller

   if (verboseTest)
      cout << "Displaying properties . . . " << endl;

   // interesting properties are stored off in class variables

   cout << "OperatingSystem Information" << endl;

   // expect to have values for the keys (even if Unknown)
   cout << "  Host: " << osCSName << endl;
   cout << "  Name: " << osName << endl;

   // on Linux, the OtherTypeDescription field had distribution info
   // wrote to display this info whenever it's present (any OS)
   if (osOtherInfo != String::EMPTY)
   {
      // put in parens after the name
      cout << "   ( " << osOtherInfo << " ) " << endl;
   }

   if (osVersion != String::EMPTY)
      cout << "  Version: " << osVersion << endl;
   else
      cout << "  Version: Unknown" << endl;
   
   if (osLicensedUsers != String::EMPTY)
      cout << "  UserLicense: " << osLicensedUsers << endl;
   else
      cout << "  UserLicense: Unknown" << endl;

   if (osCapability != String::EMPTY)
      cout << "  OSCapability: " << osCapability << endl;
   else
      cout << "  OSCapability: Unknown" << endl;
   
   if (osBootUpTime != String::EMPTY)
      cout << "  LastBootTime: " << osBootUpTime << endl;
   else
      cout << "  LastBootTime: Unknown" << endl;
   
   if (osLocalDateTime != String::EMPTY)
      cout << "  LocalDateTime: " << osLocalDateTime << endl;
   else
      cout << "  LocalDateTime: Unknown" << endl;
   
   if (osSystemUpTime != String::EMPTY)
      cout << "  SystemUpTime: " << osSystemUpTime << endl;
   else
      cout << "  SystemUpTime: Unknown" << endl;
}

/**
   gatherProperties method of the osinfo Test Client
  */
void OSInfo::gatherProperties(CIMInstance &inst, 
                              Boolean verboseTest)
{
   // don't have a try here - want it to be caught by caller

   if (verboseTest)
      cout << "Gathering " <<inst.getPropertyCount()<<" properties"<<endl;

   // loop through the properties
   for (Uint32 j=0; j < inst.getPropertyCount(); j++)
   {
      String propertyName = inst.getProperty(j).getName();

      // only pull out those properties of interest
      if (String::equalNoCase(propertyName,"CSName"))
      {
         inst.getProperty(j).getValue().get(osCSName);
      }  // end if CSName
      
      if (String::equalNoCase(propertyName,"Name"))
      {
         inst.getProperty(j).getValue().get(osName); 
      }  // end if Name

      if (String::equalNoCase(propertyName,"Version"))
      {
         inst.getProperty(j).getValue().get(osVersion); 
      }  // end if Version 

      else if (String::equalNoCase(propertyName,
                                   "OperatingSystemCapability"))
      {
         inst.getProperty(j).getValue().get(osCapability);
      }   // end if OSCapability

      else if (String::equalNoCase(propertyName,
                                   "OtherTypeDescription"))
      {
         inst.getProperty(j).getValue().get(osOtherInfo);
      }   // end if OtherTypeDescription

      else if (String::equalNoCase(propertyName,
                                   "NumberOfLicensedUsers"))
      {
         Uint32 propertyValue;
         inst.getProperty(j).getValue().get(propertyValue);
         // special consideration for HP-UX
         if (propertyValue == 0)
         {
            if (String::equalNoCase(osVersion,"HP-UX"))
            {
               osLicensedUsers.assign("128, 256, or unlimited users");
            }
            else
            {
               osLicensedUsers.assign("Unlimited user license");
            }
         }  // end if 0 as number of licensed users
         else  // standard number of users
         {
            char users[80];
            sprintf(users, "%lld users", propertyValue);
            osLicensedUsers.assign(users);
         }
      }   // end if NumberOfLicensedUsers 
      
      else if (String::equalNoCase(propertyName,
                                   "LastBootUpTime"))
      {
         CIMDateTime bdate;
         inst.getProperty(j).getValue().get(bdate);
// ATTN-SLC-17-May-02-P2  Format this nicely
         osBootUpTime.assign(bdate.getString());
      }   // end if LastBootUpTime 
      
      else if (String::equalNoCase(propertyName,
                                   "LocalDateTime"))
      {
         CIMDateTime ldate;
         inst.getProperty(j).getValue().get(ldate);
// ATTN-SLC-17-May-02-P2  Format this nicely
         osLocalDateTime.assign(ldate.getString());
      }   // end if LocalDateTime 
      
      else if (String::equalNoCase(propertyName,
                                   "SystemUpTime"))
      {
         Uint64 propertyValue;
         inst.getProperty(j).getValue().get(propertyValue);
         char   uptime[80];
         sprintf(uptime, "%lld seconds",propertyValue);
// ATTN-SLC-17-May-02-P2  Would be nice to convert seconds in uptime
         osSystemUpTime.assign(uptime);

      }   // end if SystemUpTime 

   }  // end of for looping through properties
}

/* 
   getOSInfo of the OS provider. 
*/
void OSInfo::getOSInfo(CIMClient &client,
                       Boolean verboseTest)
{
  try
    {
      Boolean deepInheritance = true;
      Boolean localOnly = true;
      Boolean includeQualifiers = false;
      Boolean includeClassOrigin = false;
      Uint32 numberInstances;

      Array<CIMNamedInstance> cimNInstances = 
	       client.enumerateInstances(NAMESPACE, CLASSNAME, 
                                         deepInheritance,
				         localOnly,  includeQualifiers,
				         includeClassOrigin );
	  
      numberInstances = cimNInstances.size();
      if (verboseTest)
	cout << numberInstances << " instances of PG_OperatingSystem" <<endl;


      // while we only have one instance (the running OS), we can take the
      // first instance.  When the OSProvider supports installed OSs as well,
      // will need to select the runningOS instance

      for (Uint32 i = 0; i < cimNInstances.size(); i++)
      {
         CIMObjectPath instanceRef = cimNInstances[i].getInstanceName();
         //String instanceRef = cimNInstances[i].getInstanceName().toString();
         if (verboseTest)
             cout<<"Instance ClassName is "<<instanceRef.getClassName()<<endl; 
	 if( !(String::equalNoCase(instanceRef.getClassName(), CLASSNAME ) ) )
         {
	    errorExit("EnumerateInstances failed");
	 }

         // first gather the interesting properties
         gatherProperties(cimNInstances[i].getInstance(),
                           verboseTest);
         
         // then display them
         displayProperties(verboseTest);

      }   // end for looping through instances
    
    }  // end try 
   
    catch(CIMClientException& e)
    {
      errorExit(e.getMessage());
    }
}

///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

// ATTN-SLC-16-Mat-02-P1  enhance to take host & user info

    String     host = String::EMPTY;
    String     userName; // = String::EMPTY;
    String     password; // = String::EMPTY;

    Boolean    verboseTest = false;

    // check if have a "verbose" on the command line
    if (argv[1] != 0)
    {
       const char *arg = argv[1];
       String arg1;
           verboseTest = true;
    }

    if (host == String::EMPTY)
    {
       host.assign("localhost:5988");
    }
    
    // need to first connect to the CIMOM
    // use null string for user and password, port 5988

    try
    {
        if (verboseTest)
           cout << "Create client" << endl;

        // specify the timeout value for the connection (if inactive)
        // in milliseconds, thus setting to one minute
        CIMClient client(60 * 1000);
        if (verboseTest)
           cout << "Client created" << endl;

        if (verboseTest)
           cout << "osinfo client connecting to " << host << endl;
	client.connect(host, userName, password);
        if (verboseTest)
           cout << "osinfo client Connected" << endl;

        OSInfo testClient;
        testClient.getOSInfo(client, verboseTest);
        
        if (verboseTest)
           cout << "osinfo client disconnecting from CIMOM " << endl;
        client.disconnect();
  }
  catch(CIMClientException& e)
  {
     cout << "osinfo error: " << e.getMessage() << endl;
  }
    return 0;
}

