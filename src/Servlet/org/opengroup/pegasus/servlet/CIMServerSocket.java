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
// Authors:
//        Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//        Bapu Patil, Hewlett-Packard Company (bapu_patil@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

package org.opengroup.pegasus.servlet;


import java.net.InetAddress;
import java.net.UnknownHostException;
import java.net.SocketException;
import java.net.Socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 *
 *  <h2>CIMServerSocket</h2>
 *  <p>
 *  CIMServerSocket contains the socket to the CIMServer and the
 *  associated input and output streams.
 *  </p>
 *  <p>
 *  @see         org.opengroup.pegasus.servlet.CIMServlet
 *  </p>
 */

public class CIMServerSocket
{
    private static final int DEFAULT_SOCKET_TIMEOUT = 300000;

    private Socket       serverSocket;
    private InputStream  socketIS;
    private OutputStream socketOS;
    private Log          log;
    private Trace        trace;

    // ************************************************************************
    /**
     *   The constructor
     *
     * @param servletLog
     *   Used to log messages
     * @param servletTrace
     *   Used to log trace messages
     **/
    // ************************************************************************
    public CIMServerSocket( Log    servletLog,
			    Trace  servletTrace )
    {
	log   = servletLog;
	trace = servletTrace;
	trace.traceStr( "CIMServerSocket(): ENTRY \n" );

        // Does nothing

	trace.traceStr( "CIMServerSocket(): EXIT \n" );
     }

    // ************************************************************************
    /**
     *   This constructor creates a socket connection the to CIMServer and
     *   gets the associated input and output streams.
     * @param xmlCIMPort
     *   Server port number
     * @param servletLog
     *   Used to log messages
     * @param servletTrace
     *   Used to log trace messages
     **/
    // ************************************************************************
    public CIMServerSocket( String xmlCIMPort,
			    Log    servletLog,
			    Trace  servletTrace )
	throws NumberFormatException,
               UnknownHostException,
               SocketException,
               IOException
    {

	log   = servletLog;
	trace = servletTrace;
	trace.traceStr( "CIMServerSocket(): ENTRY \n" );


        InetAddress ia    = InetAddress.getLocalHost();
        String xmlCIMHost = ia.getHostName();
        int    port       = Integer.parseInt( xmlCIMPort );

        serverSocket = new Socket( ia, port );
        int rcvBufSize = serverSocket.getReceiveBufferSize();
        int sndBufSize = serverSocket.getSendBufferSize();
        serverSocket.setSoTimeout( DEFAULT_SOCKET_TIMEOUT );
        serverSocket.setReceiveBufferSize( CIMServlet.MAX_READ_LEN );
        log.logStr( "CIM Servlet Connected to CIM Server on port " +
		    xmlCIMPort);

	// OutputStream for CIM Server request
	socketOS = serverSocket.getOutputStream();

	// InputStream for CIM Server request
	socketIS = serverSocket.getInputStream();
	trace.traceStr( "CIMServerSocket(): EXIT \n" );
    }

    //*********************************************************************
    /**
    * set the socket.
    *
    * @param
    *   Socket - socket connection to CIM Server
    */
    //*********************************************************************
    public void setSocket(Socket thisSock)
    {
	serverSocket = thisSock;
    }

    //*********************************************************************
    /**
    * Returns the socket.
    *
    * @return
    *   Socket - socket connection to CIM Server
    */
    //*********************************************************************
    public Socket getSocket()
    {
	return( serverSocket );
    }

    //*********************************************************************
    /**
    * sets the socket input stream.
    *
    * @param
    *   Socket input stream;
    */
    //*********************************************************************
    public void setSocketIS(InputStream isStream)
    {
	socketIS = isStream;
    }

    //*********************************************************************
    /**
    * Returns the socket input stream.
    *
    * @return
    *   Socket input stream;
    */
    //*********************************************************************
    public InputStream getSocketIS()
    {
	return( socketIS );
    }

    //*********************************************************************
    /**
    * set the socket output stream.
    *
    * @param
    *   Socket output stream;
    */
    //*********************************************************************
    public void setSocketOS(OutputStream osStream)
    {
	socketOS = osStream;
    }

    //*********************************************************************
    /**
    * Returns the socket output stream.
    *
    * @return
    *   Socket output stream;
    */
    //*********************************************************************
    public OutputStream getSocketOS()
    {
	return( socketOS );
    }

    //*********************************************************************
    /**
    * Closes the socket and its streams.
    *
    */
    //*********************************************************************
    public void closeSocket()
    {
	try
	{
	    serverSocket.close();
	}
	catch ( IOException ioe)
	{
	    log.logStr("CIMServerSocket Exception: " + ioe.getMessage());
	    trace.traceStr("closeSocket(): EXIT \n" );
	}
	serverSocket = null;
    }
}
