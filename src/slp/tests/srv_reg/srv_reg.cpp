//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/../slp/peg_slp_agent.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

/** Notes - 

The LONG_ATTRIBUTE_STRING defined below represents a typical list of attributes that a 
Pegasus service registration will contain. 

This test program uses a statically constructed Pegasus:slp_service_agent object
to create a background thread that creates and services four SLP service registrations. 

After 30 seconds the program stops the background thread and destroys the service agent. 

The way to use the Pegasus::slp_service_agent class is as follows: 

1) call slp_service_agent::srv_register once for each service advertisement
2) when all the services are registered, call slp_service_agent::start_listener
   This kicks off a background thread that maintains each registration indefinately. 
   Everything is done automatically, including finding a DA or, if one is not found, 
   initializing an SA. Re-registration to prevent expiration of the service registration
   is also automatically completed. 

3) When it is time to stop advertising services, call slp_service_agent::unregister. 


************/

#define LONG_ATTRIBUTE_STRING "(service-hi-name=Blip subsystem CIMOM), \
(service-hi-description=The Blip subsystem is managed by a Blip-Version-0.92 CIMOM.), \
(service-id=9a783b42-12783401-78FB92D0-9E82B83AA), \
(service-location-tcp=http://example.com:38294), \
(namespace=root,interop,pegasus), \
(CommunicationMechanism=cim-xml), \
(cim-xmlProtocolVersion=1.0), \
(ProfilesSupported=Basic Read, Basic Write, Schema Manipulation), \
(MultipleOperationsSupported=true), \
(AuthenticationMechanismSupported = basic), \
(CIM_InteropSchemaNamespace=/root/PG_Interop)"


// static construct/destruct of our service agent object

slp_service_agent slp_agent;

int main(int argc, char **argv)
{

   // test_registration will return zero if all the parameters have a valid syntax. 
   // positive error codes indicate which specific parameter was not parsable. 
   
   slp_agent.test_registration("service:serviceid:98432A98-B879E8FF-80342A89-43280B89C", 
                                LONG_ATTRIBUTE_STRING,
			       "service:wbem.pegasus",
			       "DEFAULT");
   
   // register 4 services. 

   // this first registration is the only one that complies to the DMTF template 
   slp_agent.srv_register("service:serviceid:98432A98-B879E8FF-80342A89-43280B89C", 
                           LONG_ATTRIBUTE_STRING,
			  "service:wbem.pegasus",
			  "DEFAULT", 
			  0xffff);

   slp_agent.srv_register("service:wbem.ibm://localhost", 
			  "(nothing=1),(version=2),(authentication=basic)",
			  "service:wbem.pegasus",
			  "DEFAULT", 
			  0xffff);

   slp_agent.srv_register("service:wbem.ibm://192.168.2.100", 
			  "(nothing=1),(version=2),(authentication=basic)",
			  "service:wbem.pegasus",
			  "DEFAULT", 
			  0xffff);

   slp_agent.srv_register("service:wbem.ibm://mday&192.168.2.100:5588", 
			  "(nothing=1),(version=2),(authentication=basic)",
			  "service:wbem.pegasus",
			  "DEFAULT", 
			  0xffff);


   // start the background thread - nothing is actually advertised until this 
   // function returns. 
   slp_agent.start_listener();

   Uint32 finish, now, msec;
   System::getCurrentTime(now, msec);
   finish = now + 30;
   
   // wait for 30 seconds. 
   while(finish > now) 
   {
      pegasus_sleep(1000);
      System::getCurrentTime(now, msec);
   }
   

   // shut down the background thread and remove all the registration data. 
   slp_agent.unregister();
   return 0;
}




