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
// Author: Bapu Patil, Hewlett-Packard Company 
//         (bapu_patil@hp.com)
//
// Modified By:
//         Warren Otsuka, Hewlett-Packard Company
//         (warren_otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

Configuring CIMServlet with Apache and Tomcat
=============================================

The CIMServlet interface provides a way for CIM client applications to access
CIM information via a Web Server running on the system. CIM clients who wish to
communicate with CIMServer running in conjunction with a Web Server may send
xmlCIM data over HTTP to the CIMServlet without changing their current
code. This allows TCP port 80 to be used for both HTTP and xmlCIM requests.

Please note that there may be other ways of doing the setup. This a simple
example of how to do the configuration.

1. To enable servlet input
--------------------------
In order to enable CIMServer to process the servlet requests the following may
be set in cimservlet.properties:

port=5988
   This is the default TCP port used by CIMServer to receive xmlCIM requests.

logDir=/var/pegasus/logs/
   This is the directory for both logging and tracing.

servletLog=true
   Setting to true enables logging.

servletLogFileName=PegasusServlet.log
   File name for the logging file.

servletTrace=true
   Setting to true enables tracing.

servletTraceFileName=PegasusServlet.trace
   File name for the tracing file.

localAuthentication=false
   Setting to true enables local authentication for the servlet.

2. Configuring CIMServlet in Tomcat
-----------------------------------

There are two configuration files that need to be configured in order for
CIMServlet to communicate with CIMServer.

Use of either the servlet interface or the Pegasus HTTP server interface for
xmlCIM requests should be transparent to the CIM client. In order to
forward all /CIMOM HTTP requests to CIMServlet, the following should be done:


tomcat.conf - tomcat configuration file
------------------------------------------

Add  the following entry into tomcat.conf:

############ Context mapping  for CIMOM
#
ApJServMount  /cimom  /examples


This makes the /cimom directory the 'ROOT' directory for the CIM
servlet. All the requests that come into /cimom shoud be forwarded
to TOMCAT.  
 
web.xml  - tomcat configuration file
---------------------------------------

Add the following entry into the file web.xml to set an alias to
CIMServlet:

<web-app>
    <servlet>
        <servlet-name>cimom</servlet-name>
        <servlet-class> org.opengroup.pegasus.servlet.CIMServlet </servlet-class>
        <load-on-startup> -2147483646 </load-on-startup>
    </servlet>
    <servlet-mapping>
        <servlet-name>cimom</servlet-name>
        <url-pattern> /cimom </url-pattern>
    </servlet-mapping>
</web-app>


3. To turn on DEBUG Tracing in CIMServlet
------------------------------

If you would like to turn on debug logging in CIMServlet, you will need specify
the cimservlet.properties file location. The following properties
should be set to the appropriate values in  cimservlet.properties:

logDir=/var/pegasus/logs/
servletTrace=true
servletTraceFileName=PegasusServlet.trace

The following should be added to web.xml file:

<web-app>
   <servlet>
      <servlet-name>cimom</servlet-name>
      <servlet-class> org.opengroup.pegasus.servlet.CIMServlet 
        </servlet-class>
      <init-param>
         <param-name>cimconfigfile</param-name>
         <param-value> /var/pegasus/conf/cimservlet.properties 
              </param-value>
      </init-param>
      <load-on-startup> -2147483646 </load-on-startup>
    </servlet>
    <servlet-mapping>
        <servlet-name>cimom</servlet-name>
        <url-pattern> /cimom </url-pattern>
    </servlet-mapping>
  </web-app>

Where 
<param-value>Absolute Path of cimservlet.properties file</param-value>


4. Set CLASSPATH to CIMServlet class 
------------------------------------
Make sure that TOMCAT knows where the CIMServlet class is located before 
starting Tomcat.

5. Stop and Restart Tomcat
--------------------------
   Stop  Tomcat by running $TOMCAT_HOME/bin/shutdown.sh
   Start Tomcat by running $TOMCAT_HOME/bin/startup.sh


For additional documentation on the Apache Web Server and Tomcat see
http://www.apache.org/.  

