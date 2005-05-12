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
// Author:
//         Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

package org.opengroup.pegasus.servlet;

import java.util.*;
import java.io.*;
import java.net.*;

// CIM classes
import org.opengroup.pegasus.servlet.LocalAuthHandler;
import org.opengroup.pegasus.servlet.AuthenticationFailedException;
import org.opengroup.pegasus.servlet.SocketUnavailableException;
import org.opengroup.pegasus.servlet.Log;
import org.opengroup.pegasus.servlet.Trace;


public final class  ConnectionPool
{

    private static final int MAX_CONNECTIONS =4;
    private static final int MIN_CONNECTIONS =1;

    private static final int MAX_READ_LEN  = 8192;
    private static final int MAX_WRITE_LEN = 8192;
    private static final int DEFAULT_SOCKET_TIMEOUT = 300000;


    // Array of CIMServerSocket objects
    private Object socketsPool[];

    // Max number of connections
    private int maxConnections;

    // Available number of connections
    private int available;

    // Currenlty number of connections opened and in use
    private int openConnections;

    boolean isAuthEnabled = false;
    private String       xmlCIMPort;
    private Log          log;
    private Trace        trace;


    // used for Synchronization of CIMServerSocket object to return
    static Object    syncLock = new Object();

    // used for Synchronization of Available count
    static Object    syncAvail = new Object();


    //*********************************************************************
    /**
     * Default Constructor
     *
     * @param xmlCimPort
     *   Cim Server socket port
     * @param servletLog
     *   Used to log messages
     * @param servletTrace
     *   Used to log trace messages
     */
    //*********************************************************************
    public ConnectionPool(String xmlCimPort,
                          Log    servletLog,
                          Trace  servletTrace,
                          boolean authEnabled )
    {

        xmlCIMPort =  xmlCimPort;
        log   = servletLog;
        trace = servletTrace;
        isAuthEnabled = authEnabled;

        trace.traceStr( "ConnectionPool(): ENTRY \n" );

        maxConnections = MAX_CONNECTIONS;

        socketsPool =new Object[maxConnections];
        available = 0;
        openConnections = 0;

        trace.traceStr( "ConnectionPool(): EXIT\n" );

    }

    //*********************************************************************
    /**
    * Default Constructor
    *
     * @param xmlCimPort
     *   Cim Server socket port
     * @param servletLog
     *   Used to log messages
     * @param servletTrace
     *   Used to log trace messages
     * @param  maxConn
     *   how many initial connections you want
    */
    //*********************************************************************
    public ConnectionPool(String xmlCimPort,
                          Log    servletLog,
                          Trace  servletTrace,
                          boolean authEnabled,
                          int maxConn )
    {

        xmlCIMPort =  xmlCimPort;
        log   = servletLog;
        trace = servletTrace;
        isAuthEnabled = authEnabled;
        this.maxConnections = maxConn;

        trace.traceStr( "ConnectionPool(): ENTRY \n" );

        socketsPool =new Object[maxConnections];
        available = 0;
        openConnections = 0;

        trace.traceStr( "ConnectionPool(): EXIT\n" );

    }


    //*********************************************************************
    /**
    * Add the object to the pool, silent nothing if the pool is full
    *
    * @param  thisSockObj
    *   CIMServer Object to add to pool
    */
    //*********************************************************************
    private void addToPool(CIMServerSocket thisSockObj)
    {
        trace.traceStr( "addToPool(): ENTRY \n" );

        //synchronized( lock )
        //synchronized( socketsPool )
        synchronized( syncAvail )
        {
            if( available <  maxConnections )
            {
                socketsPool[available] = (Object)thisSockObj;
                available = available + 1;
            }
            else
            {
                // Reached the limit
                // System.out.println("addToPool() failed");
                // Log Error
            }
        }
        trace.traceStr( "addToPool(): EXIT\n" );
     }

    //*********************************************************************
    /**
    * Return the size of the pool
    *
    * @return
    *    max connections
    */
    //*********************************************************************
    private int getMaxConnections()
    {
        trace.traceStr( "getMaxConnections(): ENTRY \n" );

        trace.traceStr( "getMaxConnections(): EXIT \n" );
        return maxConnections;
    }

    //*********************************************************************
    /**
    * Get a socket object from the pool if available else throw
    * SocketUnavailableException
    *
    * @return
    *     CIMServerSocket  return a socket connection
    *
    * @throws
    *     SocketUnavailableException  for  socket not available
    *     AuthenticationFailedException for failure to authenticate
    */
    //*********************************************************************
    public  CIMServerSocket borrowConnection()
                   throws SocketUnavailableException,
                          AuthenticationFailedException
    {
        CIMServerSocket returnSock = null;

        trace.traceStr( "borrowConnection(): ENTRY \n" );
        trace.traceStr( "Available is " + available + "\n" );

        //synchronized( lock )
        //synchronized( socketsPool )
        synchronized( syncAvail )
        {

            // If there are sockets available
            if( available > 0 )
            {
                trace.traceStr( "Available is " + available + "\n" );

                // because the array position is zero
                available = available - 1;

                returnSock = (CIMServerSocket) socketsPool[available];
                socketsPool[available] = null;

                // We will just return
                return returnSock;
            }
         }  // end syncAvail

         //else if ( openConnections < maxConnections )
        synchronized( syncLock )
        {
            if ( openConnections < maxConnections )
            {
                // Check whether we have any open connections in use
                // If not available, can we create one
                trace.traceStr( "OpenConnections is " + openConnections + " - will get one \n" );
                try
                {
                        returnSock = getConnectionToCimServer();
                        openConnections = openConnections + 1;
                }
                catch ( AuthenticationFailedException afe)
                {
                       trace.traceStr( "AuthenticationFailedException" + afe.getMessage() + " \n" );
                       throw new AuthenticationFailedException("Authentication Failed");
                }

                // We will just return
                return returnSock;
             }
             else
             {
                    // We reached MAX connections..wait till get Free
                    //trace.traceStr( "Available is " + available + " - All sockets used \n" );
                    //throw new SocketUnavailableException("Socket failed");

                    // I believe we should wait for socket to get freed
                    trace.traceStr( "Available is " + available + " - All sockets used \n" );
                    try
                    {
                          trace.traceStr( "Wait for to notify.\n");
                          syncLock.wait();
                    }
                    catch (InterruptedException e) {}

                    // Why is this so complicated...

                    //
                    // OK There is a socket available
                    //
                    if( available > 0 )
                    {
                        trace.traceStr( "Available is " + available + "\n" );

                        // because the array position is zero
                        available = available - 1;

                        returnSock = (CIMServerSocket) socketsPool[available];
                        socketsPool[available] = null;

                        // We will just return
                        return returnSock;
                    }
                }
          }

        trace.traceStr( "borrowConnection(): EXIT\n" );
        return returnSock;
    }
    //*********************************************************************
    /**
    * Take your socket back since I don't need it anymore. Thanks.
    *
    * @param
    *   CIMServerSocket - returned CIMServer socket  object
    * @param
    *   isCloseSocket - true if there was a failure in accessing
    */
    //*********************************************************************
    public void returnConnection(CIMServerSocket takeBackObject,
                                 boolean isCloseSocket)
    {
      trace.traceStr("returnConnection(): ENTRY \n" );

      //
      // close the socket in case of failure else addToPool
      // isClose is decided by CIMServlet while trying to talk to
      // CIMServer
      //
      if ( isCloseSocket )
      {
         takeBackObject.closeSocket();
      }
      else
      {
        addToPool(takeBackObject);
      }

      // Once we got a socket back we notify to waiting thread
      // in borrowConnection
      synchronized (syncLock)
      {
           trace.traceStr("Notify borrowConnection to continue.\n");
           syncLock.notifyAll();
      }

      trace.traceStr("returnConnection(): EXIT\n" );
    }

    //*********************************************************************
    /**
    *   Connects to CIMOM using a Socket interface.
    *   It assumes that initCimConfiguration is called before.
    *
    *   sets xmlCIMSocket else sets to null if an error.
    *
    * @return
    *   CIMServerSocket - socket connection to CIM Server
    */
    //*********************************************************************
    private CIMServerSocket getConnectionToCimServer()
          throws AuthenticationFailedException
    {

      CIMServerSocket cimServerSocket = null;

      Socket serverSocket = null;
      OutputStream xmlCIMServerOutStream = null;
      BufferedOutputStream xmlCIMServerOut = null;
      InputStream xmlCIMServerInStream = null;
      String xmlCIMHost = null;

      trace.traceStr("getConnectionToCimServer(): ENTRY \n" );

      try
      {
        InetAddress ia  = InetAddress.getLocalHost();
        xmlCIMHost      = ia.getHostName();
        int port        = Integer.parseInt( xmlCIMPort );

        serverSocket = new Socket( ia, port );
        int rcvBufSize = serverSocket.getReceiveBufferSize();
        int sndBufSize = serverSocket.getSendBufferSize();
        serverSocket.setSoTimeout( DEFAULT_SOCKET_TIMEOUT );
        serverSocket.setReceiveBufferSize( MAX_READ_LEN );

        log.logStr("CIM Servlet Connected to CIM Server.");
        log.logStr("Server port: " + xmlCIMPort);

        // OutputStream for CIM Server request
        xmlCIMServerOutStream = serverSocket.getOutputStream();

        // Buffered OutputStream for CIM Server resquest
        xmlCIMServerOut =
          new BufferedOutputStream( xmlCIMServerOutStream, MAX_WRITE_LEN );

        // InputStream for CIM Server response
        xmlCIMServerInStream = serverSocket.getInputStream();


        // CIMServerSocket object will be returned to caller
        cimServerSocket = new CIMServerSocket(log, trace);
        cimServerSocket.setSocket(serverSocket);
        cimServerSocket.setSocketIS(xmlCIMServerInStream);
        cimServerSocket.setSocketOS(xmlCIMServerOutStream);
      }
      catch ( NumberFormatException nfex)
      {
        log.logStr("Exception: " + nfex.getMessage());
        trace.traceStr("getConnectionToCimServer(): EXIT \n" );
        return( null );
      }
      catch ( java.net.UnknownHostException uex)
      {
         log.logStr("Exception: " + uex.getMessage());
         trace.traceStr("getConnectionToCimServer(): EXIT \n" );
         return( null );
      }
      catch ( java.net.SocketException soex)
      {
         log.logStr("Exception: " + soex.getMessage());
         trace.traceStr("getConnectionToCimServer(): EXIT \n" );
         return( null );
      }
      catch ( IOException ioe)
      {
         log.logStr("Exception: " + ioe.getMessage());
         log.logStr("Server hostname: " + xmlCIMHost );
         log.logStr("Server port: "     + xmlCIMPort );
         trace.traceStr("getConnectionToCimServer(): EXIT \n" );
         return( null );
      }

      // Bapu: Now  handle Authentication
      //boolean isAuthEnabled = false;
      if ( isAuthEnabled )
      {
            //process authentication
            LocalAuthHandler  authHandler = new LocalAuthHandler();

            try
            {
                  authHandler.processAuthentication(xmlCIMServerInStream,
                                                    xmlCIMServerOut);
            }
            catch ( AuthenticationFailedException afe)
            {
                  log.logStr("Exception: " + afe.getMessage());
                  trace.traceStr("getConnectionToCimServer(): EXIT \n" );
                  throw afe;
            }

       }

       trace.traceStr("getConnectionToCimServer(): EXIT \n" );
       return( cimServerSocket );
    }

    //********************************************************
    /**
    * Close all available sockets in the pool
    */
    //********************************************************
  public void closePool()
    {
      trace.traceStr("closePool(): ENTRY\n" );
      while ( available > 0 )
        {
          available -= 1;
          CIMServerSocket serverSock =
            (CIMServerSocket) socketsPool[available];
          serverSock.closeSocket();
          socketsPool[available] = null;
        }
      trace.traceStr("closePool(): EXIT \n" );
    }
}
