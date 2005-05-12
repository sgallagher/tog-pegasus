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
// Author: Warren Otsuka, Hewlett-Packard Company
//         (warren_otsuka@hp.com)
//
// Modified By:
//         Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

package org.opengroup.pegasus.servlet;

import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.net.SocketException;
import java.net.Socket;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.EOFException;

import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Properties;

import javax.servlet.*;
import javax.servlet.http.*;

import org.opengroup.pegasus.servlet.LocalAuthHandler;
import org.opengroup.pegasus.servlet.AuthenticationFailedException;

/**
 *
 *  <h2>CIMServlet:</h2>
 *  <p>
 *  CIMServlet extends the standard HttpServlet interface defined in
 *  Java Servlet API.
 *  </p>
 *
 *  <p>
 *  Provides an interface to CIM Client applications to access CIM information
 *  via Web Server. That is, it Provide access to CIMOM via a Web Server. CIM
 *  clients those wishes to communicate with CIMOM will send XmlCIM data
 *  over HTTP interface and these requests are forwarded to CIM Servlet by
 *  the Web Server and CIM Servlet parses CIM client request and extract
 *  the HTTP headers and XmlCIM payload. Using this HTTP request information,
 *  it then reconstructs the HTTP headers and passes the headers and the
 *  xmlCIM payload to CIM Server over a socket connection.
 *  </p>
 *
 *  <p>
 *  You may turn on DEBUG logging in CIMServlet by specifying the
 *  cim.properties file location as a initialization parameter to CIM
 *  Servlet. You must also set DEBUG_XML to true in cim.properties file.
 *  CIM Servlet debug logging file name is  cimservlet.txt.
 *  </p>
 *
 *  <p>
 *  For example: In TOMCAT case, I would add the following to the web.xml
 *  file.
 *  </p>
 *
 *  <pre>
 *
 *     <init-param>
 *        <param-name>cimconfigfile</param-name>
 *        <param-value>/var/pegasus/conf/cimservlet.properties</param-value>
 *     </init-param>
 *
 *  </pre>
 *
 *  <p>
 *  This will log all incoming and outgoing request to/from CIM Servlet.
 *  </p>
 *
 *  <p>
 *  Servlet interface will include the implementations for
 *  doPost() and doOptions() of HTTP Servlet to handle POST and OPTIONS
 *  requests for CIM Clients.
 *  </p>
 *
 *  <p>
 *  This will not include HTTP request authentication. The
 *  authentication will be handled by the Web Server before forwarding
 *  the request to CIM Servlet.
 *  </p>
 *
 *  <p>
 *  @see         javax.servlet.HttpServlet
 *  </p>
 */

public class  CIMServlet extends HttpServlet
{

    // Error message constants
    public static final String CIM_PROTOCOL_VERSION  = "1.0";
    public static final String CIM_ERROR_FAILED      = "CIM_ERR_FAILED";
    public static final int    XML_ERROR_FAILED      = 1;

    static final int MAX_READ_LEN  = 8192;
    static final int MAX_WRITE_LEN = 8192;

    // Port number of xmlCIM Server
    private String     xmlCIMPort   = DEFAULT_CIMSERVER_PORT;

    // xmlCIM Server hostname
    private String     xmlCIMHost   = null;

    // Local auth enable flag used for local authentication
    private boolean     isAuthEnabled = false;

    // HTTP request type
    String  requestMethod = null;

    // Log file
    Log log = null;

    // Trace file
    Trace trace = null;

    // Connection Pool
    ConnectionPool servletConnectionPool = null;

    private static final String POST    = "POST";
    private static final String MPOST   = "M-POST";
    private static final String OPTIONS = "OPTIONS";

    private static final String HTTP10 = "HTTP/1.0";
    private static final String HTTP11 = "HTTP/1.1";

    private static final String HTTP_CONTENTTYPE   = "Content-Type";
    private static final String HTTP_CONTENTLENGTH = "Content-Length";
    private static final String HTTP_EXT           = "Ext";
    private static final String HTTP_CACHECONTROL  = "Cache-Control";
    private static final String HTTP_URLDELIMETER  = ":";
    private static final String HTTP_TAGDELIMETER  = ":";

    private static final String CIMServerURL = "/cimom";

    private static final int HTTP_VER_BAD = 0;
    private static final int HTTP_VER_10  = 1;
    private static final int HTTP_VER_11  = 2;

    private static final String CIMOBJECT          = "CIMObject";
    private static final String CIMMETHOD          = "CIMMethod";
    private static final String CIMPROTOCOLVERSION = "CIMProtocolVersion";
    private static final String CIMOPERATION       = "CIMOperation";
    private static final String CIMVALIDATION      = "CIMValidation";

    private static final String AUTHORIZATION = "Authorization";
    private static final String CIMFUNCTIONLGROUPS =
        "CIMSupportedFunctionalGroups";

    private static final String CONTENTTYPE   =
        "application/xml; charset=\"utf-8\"";

    private static final String UTF8          = "\"utf-8\"";
    private static final String MAN           = "Man";
    private static final String BASIC         = "Basic";

    private static final String  CIMCONFIGFILE            = "cimconfigfile";
    private static final String  PROPERTY_CIMSERVER_PORT  = "port";
    private static final String  PROPERTY_SERVLET_LOGGING = "servletLog";
    private static final String  PROPERTY_LOGDIR          = "logDir";
    private static final String  PROPERTY_LOGFILENAME     = "servletLogFileName";
    private static final String  PROPERTY_SERVLET_TRACING = "servletTrace";
    private static final String  PROPERTY_TRACEFILENAME   = "servletTraceFileName";
    private static final String  DEFAULT_CIMSERVER_PORT   = "5988";
    private static final boolean DEFAULT_SERVLET_LOGGING  = true;
    private static final boolean DEFAULT_SERVLET_TRACING  = false;
    private static final String  DEFAULT_TRACEFILENAME    = "PegasusServlet.trace";
    private static final String  DEFAULT_LOGFILENAME      = "PegasusServlet.log";
    private static final String  DEFAULT_LOGDIR           = "/var/pegasus/logs/";

    // Bapu: added For local authentication
    private static final String  PROPERTY_LOCAL_AUTH      = "localAuthentication";
    private static final boolean DEFAULT_AUTH_ENABLED     = false;

    private static final String CIM_SERVER_ERROR =
        "CIM Server may not be running.";

    private static final String CIM_CONFIGFILE_ERROR =
        "Cannot open servlet config file ";

    private static final String LOCAL_AUTHENTICATION_ERROR =
        "Failed to connect to CIM Server: Authentication Error";

  /**
   *  Internal xmlCIM Server Error. This occurs in init() when
   *  Servlet is unable to connect to xmlCIM Server.
   */
    private static final String CONNECT_ERROR =
        "Unable to connect to CIM Server, please check CIM Server status";

    /**
   * cim configuration file location
   */
    private static String cimConfigFile = null;

    //*********************************************************************
    /**
    * Writes a CIM Error response using the specified response and request
    *
    * @param  request
    *    Servlet Request
    * @param  response
    *    Servlet Reponse
    * @param  errorMesg
    *    Error message to be appended to the CIM error string
    */
    //*********************************************************************
    static void sendCIMErrorResponse( HttpServletRequest  request,
                                      HttpServletResponse response,
                                      String errorMesg )
    {
    try
      {
        PrintWriter out = response.getWriter();

        String cimMethod      = request.getHeader(CIMMETHOD);

        //
        // The Servlet doesn't know the request message ID since
        // id is part of XML payload. Servlet will not try to XML
        // parse it. So it will use generic ID as '0000'
        // Note: the reason we are sending this message is that
        // CIM Servlet is  unable to talk to CIM server.
        int messageID=0000;

        out.println("<?xml version=\"1.0\" ?>");
        out.println("<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">");
        out.println("<MESSAGE ID=\"" + messageID + "\"" +
                    " PROTOCOLVERSION=\"" + CIM_PROTOCOL_VERSION + "\">");

        out.println("<SIMPLERSP>");
        out.println("<METHODRESPONSE NAME=\"" + cimMethod + "\"" + ">");

        out.println("<ERROR Code=\"" + XML_ERROR_FAILED +  "\" " +
                    "Description=\"" + CIM_ERROR_FAILED +
                    "( " + errorMesg + " )\">");

        out.println("</ERROR>");
        out.println("</METHODRESPONSE>");
        out.println("</SIMPLERSP></MESSAGE></CIM>");
        out.flush();
        out.close();
      }
    catch ( IOException ioe)
      {
        return;
      }
    }

    //*********************************************************************
    /**
    * Depending on the web Server, this is called in one of the following
    * stages: <br>
    *    - The Web Server starts <br>
    *    - When the servlet gets the first request, just before the
    *      service(doPost, doOptions etc) request <br>
    *    - A request sent by Server Administrator<br>
    *
    * @param config
    *   Servlet initialization parameters
    *
    * @throws ServletException
    *   A generic servlet exception
    */
    //*********************************************************************
    public void init(ServletConfig config)
           throws ServletException
    {
      try
      {
          //
          // Always call super.init so that we can be
          // safe to use ServletConfig object later.
          //
          super.init(config);

          //
          // Check to see CIM config (cimservlet.properties) location
          // passed as an argument.
          // This is defined in tomcat/conf/web.xml file
          //
          cimConfigFile = config.getInitParameter(CIMCONFIGFILE);
      }
      catch ( ServletException svrEx)
      {
          throw svrEx;
      }

      //
      // intializes port number, xmlCIM Server, and DEBUG
      // from cim.properties file
      //
      if( cimConfigFile != null )
        {
          initCimConfiguration( cimConfigFile );
        }

      // Connection pool
       servletConnectionPool = new ConnectionPool(
                                            xmlCIMPort, log, trace, isAuthEnabled);

      trace.traceStr("init: EXIT \n" );
    }

    //*********************************************************************
    /**
    * Handles HTTP POST requests sent by CIM clients.
    *
    * @param request
    *   HTTP Servlet Request
    * @param response
    *   HTTP Servlet Response
    * @exception IOException
    *   In case of any I/O errors such as reading InputStream or writing to
    *   OutputStream
    * @exception ServletException
    *   A Generic Exception thrown by servlets encountering difficulties.
    */
    //*********************************************************************
    public void doPost(HttpServletRequest request,
                       HttpServletResponse response)
                       throws IOException, ServletException
    {
      trace.traceStr("************************************\n" );
      trace.traceStr("doPost(): ENTRY \n" );
      processRequest(request, response);
      trace.traceStr("doPost(): EXIT \n" );
    }

    //*********************************************************************
    /**
    *  Handles HTTP OPTIONS requests from clients. This method returns
    *  features supported by CIMOM. This gets called when the client
    *  sends HTTP 'OPTIONS" request. It passes the HTTP Input to CIMOM.
    *
    * @param request
    *   HTTP Servlet Request
    * @param response
    *   HTTP Servlet Response
    * @exception IOException
    *   In case of any IO errors such as reading InputStream or writing to
    *   OutputStream
    * @exception ServletException
    *   A Generic Exception thrown by servlets encountering difficulties.
    */
    //*********************************************************************
    public void doOptions(HttpServletRequest request,
                          HttpServletResponse response)
                          throws IOException, ServletException
    {
      trace.traceStr("************************************\n" );
      trace.traceStr("doOptions(): ENTRY \n" );
      processRequest(request, response);
      trace.traceStr("doOptions(): EXIT \n" );
    }

    //*********************************************************************
    /**
    * The server calls this method when the servlet is about to be
    * unloaded.
    *
    */
    //*********************************************************************
    public void destroy()
    {
       super.destroy();

       // Release all socket connections to CIMServer
       if( servletConnectionPool != null )
         {
           servletConnectionPool.closePool();
         }
    }

    //*********************************************************************
    /**
     * Read  port number, and debug from cim.properties
     * @param cimConfigFile
     *   The path to the servlet config file.
     */
    //*********************************************************************
    private void initCimConfiguration( String configFilePath )
    {
        // Logging flag used for logging requests and responses
        boolean     isLogging = true;

        // Tracing flag used for tracing requests and responses
        boolean     isTracing   = false;

        String logFileName   = DEFAULT_LOGFILENAME;
        String traceFileName = DEFAULT_TRACEFILENAME;
        String logDir        = DEFAULT_LOGDIR;

        Properties servletProperties = new Properties();

        xmlCIMPort    = DEFAULT_CIMSERVER_PORT;
        isLogging     = DEFAULT_SERVLET_LOGGING;
        isTracing     = DEFAULT_SERVLET_TRACING;
        isAuthEnabled = DEFAULT_AUTH_ENABLED;

        if( configFilePath == null )
          {
            System.out.println( "configFilePath==null \n" );
            return;
          }

        // Read the config file
        try
        {
            FileInputStream fileInput = new FileInputStream( configFilePath );
            servletProperties.load( fileInput );

        }
        catch( FileNotFoundException e )
        {
          // Use defaults
          System.out.println( "FileNotFoundException: " + e.getMessage() +"\n" );
          return;
        }
        catch( IOException e )
        {
          // Use defaults
          System.out.println( "IOException: " + e.getMessage() +"\n" );
          return;
        }

        // Check for logging directory
        String logDirProp = servletProperties.getProperty( PROPERTY_LOGDIR );
        if( logDirProp != null )
        {
          logDir = logDirProp;
        }

        // Check for logging filename
        String logFileNameProp =
            servletProperties.getProperty( PROPERTY_LOGFILENAME );
        if( logFileNameProp != null )
        {
          logFileName = logFileNameProp;
        }

        // Check for tracing filename
        String traceFileNameProp =
            servletProperties.getProperty( PROPERTY_TRACEFILENAME );
        if( traceFileNameProp != null )
        {
          traceFileName = traceFileNameProp;
        }

        // Check for logging
        String logging = servletProperties.getProperty( PROPERTY_SERVLET_LOGGING );
        if( logging != null )
        {
            if ( logging == null )
            {
                isLogging = DEFAULT_SERVLET_LOGGING;
            }
            else if ( logging.equalsIgnoreCase("true") )
            {
                isLogging = true;
            }
            else if ( logging.equalsIgnoreCase("false") )
            {
                isLogging = false;
            }
            else
            {
                isLogging = DEFAULT_SERVLET_LOGGING;
            }
        }

        // Check for tracing
        String tracing = servletProperties.getProperty( PROPERTY_SERVLET_TRACING );
        if( tracing != null )
        {
            if ( tracing == null )
            {
                isTracing = DEFAULT_SERVLET_TRACING;
            }
            else if ( tracing.equalsIgnoreCase("true") )
            {
                isTracing = true;
            }
            else if ( tracing.equalsIgnoreCase("false") )
            {
                isTracing = false;
            }
            else
            {
                isTracing = DEFAULT_SERVLET_TRACING;
            }
        }

        // Check local Authentication enabled
        String localauth = servletProperties.getProperty( PROPERTY_LOCAL_AUTH );
        if( localauth != null )
        {
            if ( localauth == null )
            {
                isAuthEnabled = DEFAULT_AUTH_ENABLED;
            }
            else if ( localauth.equalsIgnoreCase("true") )
            {
                isAuthEnabled = true;
            }
            else if ( localauth.equalsIgnoreCase("false") )
            {
                isAuthEnabled = false;
            }
            else
            {
                isAuthEnabled = DEFAULT_AUTH_ENABLED;
            }
        }

      if( isLogging )
          {
              //
              // CIMservlet will have its own logging file
              //
              log = new Log( logDir, logFileName);
          }
      else
          {
              log = new Log();
          }


      if( isTracing )
          {
              //
              // CIMservlet will have its own tracing file
              //
              trace = new Trace( logDir, traceFileName);
          }
      else
          {
              trace = new Trace();
          }

        // Parse the properties
        String portNumber = servletProperties.getProperty( PROPERTY_CIMSERVER_PORT );
        if( portNumber != null )
          {
            int port;
            try
              {
                port = Integer.parseInt( portNumber );
                if ( (port < 0) || (port > 65535) )
                  {
                    //Log error and use the defaults
                    log.logStr( "initCimConfiguration(): Invalid port number " + port);
                  }
                xmlCIMPort = portNumber;
                trace.traceStr( "initCimConfiguration(): portNumber= " + portNumber + " \n" );
              }
            catch (NumberFormatException e)
              {
                  //Log error and use the defaults
                  log.logStr( "Number Format Exception: " + e.getMessage() );
              }
          }

        trace.traceStr("initCimConfiguration(): EXIT \n" );
    }

    //*********************************************************************
    /**
    * processes the requests sent by CIM clients and this is called
    * by doPost() and doOptions()
    *
    * @param request
    *   HTTP Servlet Request
    * @param response
    *   HTTP Servlet Response
    * @exception IOException
    *   In case of any IO errors such as InputStream/OutputStream
    * @exception ServletException
    *   A Generic Exception thrown by servlets encountering difficulties.
    */
    //*********************************************************************
    private void processRequest( HttpServletRequest  request,
                                 HttpServletResponse response)
                       throws IOException, ServletException
    {

      // Socket connection to CIM Server
      CIMServerSocket cimServerSocket = null;

      // to determine bad sockets
      boolean isCloseSocket = false;

      trace.traceStr("processRequest(): ENTRY \n" );

      // Store the request Type
      String thisRequestType = request.getMethod();

      //
      //  Connect to CIM server
      //
      try
      {
              // Open socket to CIM server
              //cimServerSocket = connectToCimServer();
              if ( servletConnectionPool == null )
              {
                // Connection pool
                servletConnectionPool = new ConnectionPool(
                                               xmlCIMPort, log, trace, isAuthEnabled);
              }

              // borrow the connection ..here is 5 bucks
              cimServerSocket = servletConnectionPool.borrowConnection();

      }
      catch ( SocketUnavailableException sue)
      {
              log.logStr("Exception: " + sue.getMessage());

              // CIM Server socket Error
              sendCIMErrorResponse( request, response, CIM_SERVER_ERROR);

              // close the socket connection
              isCloseSocket = true;
              servletConnectionPool.returnConnection(cimServerSocket, isCloseSocket);
              trace.traceStr("processRequest(): EXIT \n" );
      }
      catch ( AuthenticationFailedException afe)
      {
              log.logStr("Exception: " + afe.getMessage());
              sendCIMErrorResponse( request, response, LOCAL_AUTHENTICATION_ERROR );

              // close the socket connection
              isCloseSocket = true;
              servletConnectionPool.returnConnection(cimServerSocket, isCloseSocket);
              trace.traceStr("processRequest(): EXIT \n" );
      }

      //
      // Reconstruct HTTP headers and send to CIM Server
      //
      sendRequest( request, cimServerSocket );

      //
      // Process response
      //

      try
      {
          // InputStream for CIM Server response
          InputStream responseIS = cimServerSocket.getSocketIS();
          CIMOMResponse cimomResponse = new CIMOMResponse( responseIS, log, trace );
          cimomResponse.setResponse( request, response );


      }
      catch ( SocketException soe)
      {
	  log.logStr("stream2String: "+soe.toString());
	  sendCIMErrorResponse( request, response, soe.toString() );
      }
      catch ( IOException ioe )
      {
	  log.logStr("Exception: " + ioe.getMessage());
	  sendCIMErrorResponse( request, response, ioe.toString() );
      }
      catch ( NumberFormatException nfe)
      {
	  log.logStr("Exception: " + nfe.getMessage());
	  sendCIMErrorResponse( request, response, nfe.toString() );
      }

       // close the socket connection
       servletConnectionPool.returnConnection(cimServerSocket, isCloseSocket);

       trace.traceStr("processRequest(): EXIT \n" );
    }

    //*********************************************************************
    /**
    *   Connects to CIMOM using a Socket interface.
    *   It assumes that initCimConfiguration is called before.
    *
    *   sets xmlCIMSocket else sets to null if an error.
    *
    * @return
    *   Socket - socket connection to CIM Server
    */
    //*********************************************************************
  private CIMServerSocket connectToCimServer()
          throws AuthenticationFailedException
  {
      CIMServerSocket  cimServerSocket = null;

      trace.traceStr("connectToCimServer(): ENTRY \n" );

      try
      {
          cimServerSocket = new CIMServerSocket( xmlCIMPort, log, trace );
      }
      catch ( NumberFormatException nfex)
      {
        log.logStr("Exception: " + nfex.getMessage());
        System.err.println("Exception: " + nfex.getMessage());
        trace.traceStr("connectToCimServer(): EXIT \n" );
        return( null );
      }
      catch ( UnknownHostException uex)
      {
        log.logStr("Exception: " + uex.getMessage());
        System.err.println("Exception: " + uex.getMessage());
        trace.traceStr("connectToCimServer(): EXIT \n" );
        return( null );
      }
      catch ( SocketException soex)
      {
        log.logStr("Exception: " + soex.getMessage());
        System.err.println("Exception: " + soex.getMessage());
        trace.traceStr("connectToCimServer(): EXIT \n" );
        return( null );
      }
      catch ( IOException ioe)
      {
        log.logStr("Exception: " + ioe.getMessage());
        log.logStr("Server hostname: " + xmlCIMHost );
        log.logStr("Server port: "     + xmlCIMPort );
        trace.traceStr("connectToCimServer(): EXIT \n" );
        return( null );
      }

      // Bapu: Now  handle Authentication
      if ( isAuthEnabled )
      {
            //process authentication
            LocalAuthHandler  authHandler = new LocalAuthHandler();

            try
            {

                // OutputStream for CIM Server request
                OutputStream xmlCIMServerOutStream = cimServerSocket.getSocketOS();

                // Buffered OutputStream for CIM Server resquest
                BufferedOutputStream xmlCIMServerOut =
                    new BufferedOutputStream( xmlCIMServerOutStream, MAX_WRITE_LEN );

                // InputStream for CIM Server response
                InputStream xmlCIMServerInStream = cimServerSocket.getSocketIS();
                authHandler.processAuthentication(xmlCIMServerInStream,
                                                  xmlCIMServerOut);
            }
            catch ( AuthenticationFailedException afe)
            {
                log.logStr("Exception: " + afe.getMessage());
                trace.traceStr("connectToCimServer(): EXIT \n" );
                throw afe;

                // close the socket connection
                //closeSocket();
            }
      }

      trace.traceStr("connectToCimServer(): EXIT \n" );
      return( cimServerSocket );
    }

    //*********************************************************************
    /**
    * Write line to HTTP socket
    *
    * @param line
    *    String to write
    */
    //*********************************************************************
    private void writeToSocket( String line,
                                BufferedOutputStream xmlCIMServerOut )
  {
    trace.traceStr("writeToSocket(): ENTRY \n" );
    try
      {
        // Log HTTP output
        trace.traceStr( "IN: " + line );

        if( line != null )
          {
            xmlCIMServerOut.write( (line + "\r\n").getBytes() );
          }
        else
          {
            // End of headers
            xmlCIMServerOut.write( ("\r\n").getBytes() );
          }
        trace.traceStr("writeToSocket(): EXIT \n" );
      }
    catch ( IOException ioe)
      {
        log.logStr("Exception: " + ioe.getMessage());
        trace.traceStr("writeToSocket(): EXIT \n" );
      }
  }

    //*********************************************************************
    /**
    * Reconstructs the HTTP Request headers and sends the request to
    * the CIM Server.
    * @param
    *   httpRequest - HTTP Request that was sent by the client
    */
    //*********************************************************************
  private void sendRequest( HttpServletRequest httpRequest,
                            CIMServerSocket    cimServerSocket )
  {

    // OutputStream for CIM Server request
    OutputStream xmlCIMServerOutStream = null;

    // Buffered OutputStream for CIM Server resquest
    BufferedOutputStream xmlCIMServerOut = null;

    int httpVersion = HTTP_VER_BAD;
    String line     = null;

    String  httpProtocol  = null;
    String  remoteUser    = null;
    String  authorization = null;
    String  contentLength = null;
    String  cimOperation  = null;
    String  cimProtocolVersion = null;
    String  cimMethod     = null;
    String  cimObject     = null;

    trace.traceStr("sendRequest(): ENTRY \n" );
    try
      {

          // OutputStream for CIM Server request
          xmlCIMServerOutStream = cimServerSocket.getSocketOS();

          // Buffered OutputStream for CIM Server resquest
          xmlCIMServerOut =
              new BufferedOutputStream( xmlCIMServerOutStream, MAX_WRITE_LEN );

        httpProtocol  = httpRequest.getProtocol();
        requestMethod = httpRequest.getMethod();
        int len       = httpRequest.getContentLength();
        contentLength = String.valueOf( len );

        // Check for HTTP Version
        if( ( httpProtocol.equalsIgnoreCase(HTTP10) ||
              ( httpProtocol.equalsIgnoreCase(OPTIONS) ) ) )
          {
            trace.traceStr("sendRequest(): Version is 1.0\n" );
            httpVersion = HTTP_VER_10;
          }
        else if ( httpProtocol.equalsIgnoreCase(HTTP11) )
          {
            trace.traceStr("sendRequest(): Version is 1.1\n" );
            httpVersion = HTTP_VER_11;
          }
        else
          {
            log.logStr("sendRequest(): Version is BAD\n" );
            // TODO: Handle this error condition
            httpVersion = HTTP_VER_BAD;
          }

        if  ( (requestMethod.equalsIgnoreCase(POST)) )
          {
            trace.traceStr("sendRequest(): requestMethod is POST \n" );

            // Write common headers out

            writeToSocket( requestMethod + " " + CIMServerURL + " " + httpProtocol,
                           xmlCIMServerOut );
            Enumeration headerNames = httpRequest.getHeaderNames();
            String headerName;
            String headerValue;
            while( headerNames.hasMoreElements() )
              {
                headerName = (String) headerNames.nextElement();
                headerValue = httpRequest.getHeader( headerName );
                if( ( headerValue != null ) && ( headerValue != "" ) )
                  {
                    writeToSocket( headerName + ": " + headerValue,
                                   xmlCIMServerOut );

                  }
              }
            writeToSocket( null, xmlCIMServerOut );  // Terminate headers

            if( len > 0 )
              {
                // Write xmlCIM payload to server socket

                trace.traceStr("sendRequest(): Writing payload\n" );
                InputStream xmlCIMIn = httpRequest.getInputStream();
                DataInputStream inStream = new DataInputStream( xmlCIMIn );
                String payload = stream2String( inStream );
                trace.traceStr("sendRequest(): Payload-> \n" + payload );
                byte xmlBuffer[] = payload.getBytes();
                xmlCIMServerOut.write( xmlBuffer, 0, payload.length() );
                xmlCIMServerOut.flush();
                trace.traceStr("sendRequest(): Payload sent\n" );
              }
          }
        else if ( (requestMethod.equalsIgnoreCase(MPOST)) )
          {
          }
        trace.traceStr("sendRequest() EXIT \n" );
      }
    catch ( IOException ioe )
      {
        log.logStr("CIMServlet Exception: " + ioe.getMessage());
        trace.traceStr("sendRequest() EXIT \n" );
      }
  }


    //*********************************************************************
    /**
    * Constructs a String from all data on a DataInputStream
    *
    * @param
    *   is - DataInputStream instance
    * @return
    *   String Object
    */
    //*********************************************************************
  private String stream2String( DataInputStream is )
    {
        final int SLEEP_TIME = 500; // sleep time until next read try

      trace.traceStr("stream2String: ENTRY \n" );
      if( is != null )
        {
          trace.traceStr("stream2String: is: " + is.toString() + "\n" );
        }
       String line       = null;
       StringBuffer sbuf = new StringBuffer();
       int bytesRead     = 0;

       byte[] buf = new byte[MAX_READ_LEN];
       while( true )
         {
           try
             {
                 trace.traceStr("stream2String: Reading... \n" );
                 bytesRead = is.read( buf, 0,MAX_READ_LEN );
                 if( bytesRead < 0 )
                 {
                   throw new EOFException();
                 }

               if( bytesRead > 0 )
                   {
                       String inputString = new String( buf, 0, bytesRead );
                       trace.traceStr( "Stream bytesRead: " + bytesRead);
                       trace.traceStr( "Stream IN:\n" + inputString);
                       sbuf.append( inputString );
                       int availableBytes = is.available();
                       trace.traceStr( "Stream availableBytes:" + availableBytes );
                       if( availableBytes == 0 )
                           throw new EOFException();
                   }
               else
                   {
                       trace.traceStr("stream2String: Data is not available \n" );
                       try
                           {
                               Thread.sleep( SLEEP_TIME );
                           }
                       catch( InterruptedException ie )
                           {
                           }
                   }
           }
       catch ( SocketException soe)
       {
         log.logStr("stream2String: "+soe.toString());
         trace.traceStr("stream2String: EXIT \n" );
         return( sbuf.toString() );
       }
       catch ( EOFException eofe)
       {
         trace.traceStr("stream2String: EXIT \n" );
         return( sbuf.toString() );
       }
       catch ( IOException ioe)
       {
         log.logStr("stream2String: "+ioe.toString());
         trace.traceStr("stream2String: EXIT \n" );
         return(sbuf.toString());
       }
    }
  }
}
