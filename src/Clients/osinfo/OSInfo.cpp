//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//         Mike Day (mdday@us.ibm.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Susan Campbell, Hewlett-Packard Company (scampbell@hp.com)
//         Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Destroyer.h>


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
    cerr << "osinfo error: " << message << endl;
    exit(1);
}

/** _usage method for osinfo - only accept one option
    -c for raw CIM formatting
*/
void OSInfo::_usage()
{
  cerr << "Usage: osinfo [-c]" << endl;
  cerr << "Example:" << endl;
  cerr << "  osinfo " << endl;
  cerr << "  osinfo -c " << endl;
}

/**
   displayProperties method of the osinfo Test Client
  */
void OSInfo::displayProperties()
{
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
   formatCIMDateTime method takes a string with CIM formatted
   DateTime and returns a user-readable string of the format
   month day-of-month, year  hour:minute:second (value-hrs-GMT-offset)
   */
static void formatCIMDateTime (const char* cimString, char* dateTime)
{
   int year = 0;
   int month = 0;
   int day = 0;
   int hour = 0;
   int minute = 0;
   int second = 0;
   int microsecond = 0;
   int timezone = 0;
   sscanf(cimString, "%04d%02d%02d%02d%02d%02d.%06d%04d",
          &year, &month, &day, &hour, &minute, &second,
          &microsecond, &timezone);
   char monthString[5];
   switch (month)
   {
      case 1 : { sprintf(monthString, "Jan"); break; }
      case 2 : { sprintf(monthString, "Feb"); break; }
      case 3 : { sprintf(monthString, "Mar"); break; }
      case 4 : { sprintf(monthString, "Apr"); break; }
      case 5 : { sprintf(monthString, "May"); break; } 
      case 6 : { sprintf(monthString, "Jun"); break; }
      case 7 : { sprintf(monthString, "Jul"); break; }
      case 8 : { sprintf(monthString, "Aug"); break; }
      case 9 : { sprintf(monthString, "Sep"); break; }
      case 10 : { sprintf(monthString, "Oct"); break; }
      case 11 : { sprintf(monthString, "Nov"); break; }
      case 12 : { sprintf(monthString, "Dec"); break; }
      // covered all knowned cases, if get to default, just 
      // return the input string as received.
      default : { strcpy(dateTime, cimString); return; }
   }
   
   sprintf(dateTime, "%s %d, %d  %d:%d:%d (%03d%02d)",
           monthString, day, year, hour, minute, second, 
           timezone/60, timezone%60);

   return;
}

/**
   gatherProperties method of the osinfo Test Client
  */
void OSInfo::gatherProperties(CIMInstance &inst, Boolean cimFormat) 
{
   // don't have a try here - want it to be caught by caller

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
         char bdateString[80];

         inst.getProperty(j).getValue().get(bdate);
         CString dtStr = bdate.toString().getCString();
         if (!cimFormat) 
         { // else leave in raw CIM
            formatCIMDateTime(dtStr, bdateString);
         }
         else
         {
            sprintf(bdateString,"%s",(const char*)dtStr);
         }
         osBootUpTime.assign(bdateString);
      }   // end if LastBootUpTime 
      
      else if (String::equalNoCase(propertyName,
                                   "LocalDateTime"))
      {
         CIMDateTime ldate;
         char ldateString[80];

         inst.getProperty(j).getValue().get(ldate);
         CString dtStr = ldate.toString().getCString();
         if (!cimFormat) 
         { // else leave in raw CIM
            formatCIMDateTime(dtStr, ldateString);
         }
         else
         {
            sprintf(ldateString,"%s",(const char*)dtStr);
         }
         osLocalDateTime.assign(ldateString);
      }   // end if LocalDateTime 
      
      else if (String::equalNoCase(propertyName,
                                   "SystemUpTime"))
      {
         Uint64 total;
         char   uptime[80];
         inst.getProperty(j).getValue().get(total);

         if (!cimFormat) 
         { // else leave in raw CIM
            // let's make things a bit easier for our user to read
            Uint64 days = 0;
            Uint64 hours = 0;
            Uint64 minutes = 0;
            Uint64 seconds = 0;
            Uint64 totalSeconds = total;
            seconds = total%60;
            total = total/60;
            minutes = total%60;
            total = total/60;
            hours = total%24;
            total = total/24;
            days = total;

            // now deal with the proper singular/plural
            char dayString[20];
            char hourString[20];
            char minuteString[20];
            char secondString[20];
   
            sprintf(dayString, (days == 0?"":
                               (days == 1?"1 day,":
                               "%lld days,")), days);
        
            // for other values, want to display the 0s 
            sprintf(hourString, (hours == 1?"1 hr,":
                                "%lld hrs,"), hours);
         
            sprintf(minuteString, (minutes == 1?"1 min,":
                                  "%lld mins,"), minutes);
         
            sprintf(secondString, (seconds == 1?"1 sec":
                                  "%lld secs"), seconds);
         
            sprintf(uptime, "%lld seconds = %s %s %s %s",
                    totalSeconds,
                    dayString,
                    hourString,
                    minuteString,
                    secondString);
            osSystemUpTime.assign(uptime);
         }  // end of if wanted nicely formatted vs. raw CIM
         else 
         {
            sprintf(uptime,"%lld",total);
         }

         osSystemUpTime.assign(uptime);

      }   // end if SystemUpTime 

   }  // end of for looping through properties
}

/* 
   getOSInfo of the OS provider. 
*/
void OSInfo::getOSInfo(const int argc, const char** argv)
{

// ATTN-SLC-16-May-02-P1  enhance to take host & user info
//  Decided to keep local only for first release 

    Boolean cimFormat = false;

    // before we even connect to CIMOM, make sure we're
    // syntactically valid

    if (argc > 2)
    {
       _usage();
       exit(1);
    }

    if (argc == 2)
    {
       // only support one option, -c for CIM formatting
       const char *opt = argv[1];

       if (strcmp(opt,"-c") == 0)
       {
          cimFormat = true;
       }
       else
       {
          _usage();
          exit(1);
       }
    }

    // need to first connect to the CIMOM

    try
    {
        // specify the timeout value for the connection (if inactive)
        // in milliseconds, thus setting to one minute
        CIMClient client;
        client.setTimeout(60 * 1000);
	client.connectLocal();
        
        Boolean deepInheritance = true;
        Boolean localOnly = true;
        Boolean includeQualifiers = false;
        Boolean includeClassOrigin = false;
        Uint32 numberInstances;

        Array<CIMInstance> cimNInstances = 
	       client.enumerateInstances(NAMESPACE, CLASSNAME, 
                                         deepInheritance,
				         localOnly,  includeQualifiers,
				         includeClassOrigin );
	  
        numberInstances = cimNInstances.size();

        // while we only have one instance (the running OS), we can take the
        // first instance.  When the OSProvider supports installed OSs as well,
        // will need to select the runningOS instance

        for (Uint32 i = 0; i < cimNInstances.size(); i++)
        {
           CIMObjectPath instanceRef = cimNInstances[i].getPath ();
           if ( !(String::equalNoCase(instanceRef.getClassName(), 
                                      CLASSNAME ) ) )
           {
              errorExit("EnumerateInstances failed");
           }

           // first gather the interesting properties
           gatherProperties(cimNInstances[i], cimFormat);
         
           // then display them
           displayProperties();

      }   // end for looping through instances
    
    }  // end try 
   
    catch(Exception& e)
    {
      errorExit(e.getMessage());
    }

}

///////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////

int main(const int argc, const char** argv)
{
   OSInfo osInfo;
   osInfo.getOSInfo(argc, argv);
   return 0;
}

