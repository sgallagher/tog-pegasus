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
//
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

import java.util.Date;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.Properties;

import javax.servlet.*;
import javax.servlet.http.*;

/**
 *
 *  <h2>CIMOMResponse:</h2>
 *  <p>
 *  CIMOMResponse reads the CIM-XML response from CIMOM and provides
 *  methods to access the contents of the stream.
 *  </p>
 *
 *  @see         org.opengroup.pegasus.servlet.CIMServlet
 *  <p>
 *  @see         javax.servlet.HttpServlet
 *  </p>
 */

public class CIMOMResponse
{

    // Log file
    Log log = null;

    // Trace file
    Trace trace = null;

    // Variables for parsing CIMServer response
    private StringBuffer sb;
    private int sbLen;
    private int sbIndex = 0;


    private static final String GENERIC_IO_ERROR =
	"An I/O error occurred during " +
	"the processing of the CIM request.";

    private static final String NO_DATA_ERROR =
	"No response was returned by CIM Server.";

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

    private static final int MAX_READ_LEN  = 8192;
    private static final int MAX_WRITE_LEN = 8192;

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


    //*********************************************************************
    /**
     * Constructor reads entire HTTP response from CIMOM
     *
     * @param responseIS
     *   HTTP CIMOM Response input stream
     * @param responseLog
     *   Used to log messages
     * @param responseTrace
     *   Used to log trace messages
     * @exception IOException
     *   In case of any I/O errors such as reading InputStream
     */
    //*********************************************************************
	public CIMOMResponse( InputStream responseIS,
			      Log responseLog,
			      Trace responseTrace )
	    throws IOException, SocketException, NumberFormatException
    {
	trace = responseTrace;
	log   = responseLog;
	trace.traceStr("getCIMResponse: ENTRY \n" );
	trace.traceStr("getCIMResponse: responseIS " + responseIS.toString());

	sb = new StringBuffer( MAX_READ_LEN );

	DataInputStream inStream = new DataInputStream( responseIS );
	String responseString = stream2String( inStream );
	sb.append( responseString );
	trace.traceStr("getCIMResponse: Response from cimserver-> \n" + responseString );
	sbIndex = 0;
	sbLen   = sb.length();
	trace.traceStr("getCIMResponse: sb.length-> " + String.valueOf(sbLen) );
	trace.traceStr("getCIMResponse: sb-> \n" + sb.toString() );
	trace.traceStr("getCIMResponse: EXIT \n" );
    }


    //*********************************************************************
    /**
    * Parses the HTTP Response headers and sets the appropriate
    * HttpServletResponse fields.
    * @param
    *   httpRequest - HTTP Request that was sent by the client
    * @param
    *   httpResponse - HTTP Response that is sent back to the client
    */
    //*********************************************************************
  public void setResponse( HttpServletRequest  request,
			    HttpServletResponse response )
  {

    // Variables for parsing CIMServer response
    StringBuffer b = null;
    int sbLen = 0;
    int sbIndex = 0;

    int httpVersion = HTTP_VER_BAD;
    String line     = null;
    StringTokenizer strToken = null;
    String  nextToken        = null;
    boolean endOfStream      = false;
    int contentLength        = 0;

    //
    // Send CIM response back to the caller
    //

    // Set the HTTP headers
    trace.traceStr("setResponse(): ENTRY \n" );
    try
      {
        String inputString = getNextLine();
        if( inputString != null )
          {
            // Parse HTTP header and set response code

            strToken  = new StringTokenizer( inputString, " " );
            nextToken = strToken.nextToken();  // HTTP Version

            // Check for HTTP Version
            if ( nextToken.equalsIgnoreCase(HTTP10))
              {
                httpVersion = HTTP_VER_10;
              }
            else if ( nextToken.equalsIgnoreCase(HTTP11) )
              {
                httpVersion = HTTP_VER_11;
              }
            else
              {
                httpVersion = HTTP_VER_BAD;
                // TODO: Handle this error condition
                trace.traceStr("setResponse(): Bad HTTP version " + nextToken );
              }
            nextToken = strToken.nextToken();  // Status
            int status = Integer.parseInt( nextToken );
            response.setStatus( status );
          }
        else
          {
            // No data on the CIM Server socket
            CIMServlet.sendCIMErrorResponse( request, response, NO_DATA_ERROR );
            return;
          }


        // Parse response headers

        for (;;)
          {
            inputString = getNextLine();
	    trace.traceStr( "setResponse(): inputString=" + inputString + "\n");
            if( inputString.equalsIgnoreCase( "\r\n" ) )
              {
                // End of header fields
		trace.traceStr( "setResponse(): end of headers\n" );
                break;
              }
            if( inputString == null )
              {
                // End of input
		trace.traceStr( "setResponse(): end of stream\n" );
                endOfStream = true;
                break;
              }

            strToken =
              new StringTokenizer( inputString, HTTP_TAGDELIMETER );
            nextToken = strToken.nextToken();      // Tag
	    trace.traceStr("setResponse(): Tag=" + nextToken + "\n");

            if( nextToken.equals( HTTP_CONTENTTYPE ) )
              {
                // Set content type
                nextToken = strToken.nextToken();  // Content type value
                response.setContentType( HTTP_CONTENTTYPE + nextToken );
                continue;
              }

            if( nextToken.equals( HTTP_CONTENTLENGTH ) )
              {
                // Set content length
                nextToken = strToken.nextToken();  // Content length value
                trace.traceStr("setResponse():val=" + nextToken + "\n" );
                trace.traceStr("setResponse():val len=" + nextToken.length() + "\n" );
		String valString = nextToken.trim();
                trace.traceStr("setResponse():valString=" + valString + "\n" );

		contentLength = Integer.parseInt( valString );
		trace.traceStr("setResponse(): content-length val=" + contentLength + "\n" );
                response.setIntHeader( HTTP_CONTENTLENGTH, contentLength );
                continue;
              }

            if( nextToken.equals( HTTP_EXT ) )
              {
		  if( strToken.hasMoreTokens() )
		  {
		      nextToken = strToken.nextToken();  // Ext value
		  }
		  else
		  {
		      nextToken = null;
		  }
                response.setHeader( HTTP_EXT, nextToken );
                continue;
              }

            if( nextToken.equals( HTTP_CACHECONTROL ) )
              {

                nextToken = strToken.nextToken();  // Cache-Control value
                response.setHeader( HTTP_CACHECONTROL, nextToken );
                continue;
              }

            if( nextToken.equals( MAN ) )
              {
                nextToken = strToken.nextToken();  // Man http string
                StringBuffer manSb = new StringBuffer( nextToken );
                manSb.append( HTTP_URLDELIMETER );
                nextToken = strToken.nextToken();  // Rest of Man string
                manSb.append( nextToken );
                response.setHeader( MAN, manSb.toString() );
                manSb = null;
                continue;
              }

            if( nextToken.equals( CIMVALIDATION ) )
              {
                nextToken = strToken.nextToken();  // CIMValidation
                response.setHeader( CIMVALIDATION, nextToken );
                continue;
              }

            if( nextToken.equals( CIMFUNCTIONLGROUPS ) )
              {
                nextToken = strToken.nextToken();  // CIMSupportedFunctionalGroups
                response.setHeader( CIMFUNCTIONLGROUPS, nextToken );
                continue;
              }

            if( nextToken.indexOf( CIMOPERATION ) != -1 )
              {
                nextToken = strToken.nextToken();
                response.setHeader( CIMOPERATION, nextToken );
                continue;
              }

            if( nextToken.indexOf( CIMPROTOCOLVERSION ) != -1 )
              {
                nextToken = strToken.nextToken();
                response.setHeader(  CIMPROTOCOLVERSION, nextToken );
                continue;
              }
          }

        String requestMethod = request.getMethod();
        if  ( !(requestMethod.equalsIgnoreCase(OPTIONS)) &&
              ( endOfStream == false ) )
          {

            // Copy XML payload to response

            // get Servlet outputstream
	    trace.traceStr("setResponse(): before getting outputstream \n" );
            OutputStream out = response.getOutputStream();
	    trace.traceStr("setResponse(): after getting outputstream \n" );

            // Get the byte Array outputstream
            byte[] outByteArray = getResByteArray();


            out.write(outByteArray);
            out.flush();
            out.close();
          }
        trace.traceStr("setResponse(): EXIT \n" );
      }
        catch ( IOException ioe)
          {
            log.logStr("CIMServlet Exception: " + ioe.getMessage());
            CIMServlet.sendCIMErrorResponse(request, response, GENERIC_IO_ERROR);
	    trace.traceStr("setResponse(): EXIT \n" );
            return;
          }
        catch ( NumberFormatException nfe)
          {
            log.logStr("CIMServlet Exception: NumberFormatException " + nfe.getMessage());
            trace.traceStr("CIMServlet Exception: NumberFormatException " + nfe.getMessage());
	    nfe.printStackTrace( trace.getPrintWriter() );
            CIMServlet.sendCIMErrorResponse(request, response, GENERIC_IO_ERROR);
	    trace.traceStr("setResponse(): EXIT \n" );
            return;
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
	    throws IOException, SocketException, NumberFormatException
    {
	final int SLEEP_TIME = 500;  // sleep time until next read try
	final String EOL = "\r\n";
	final String EOH = "\r\n\r\n";

      trace.traceStr("stream2String: ENTRY \n" );
      if( is != null )
        {
          trace.traceStr("stream2String: is: " + is.toString() + "\n" );
        }
       String line       = null;
       StringBuffer sbuf = new StringBuffer();
       int bytesRead     = 0;
       boolean firstRead = true; // flag the first bytes read
       int contentLength = 0;
       int contentOffset = 0;
       int numBytesToRead   = MAX_READ_LEN;

       byte[] buf = new byte[MAX_READ_LEN];
       while( true )
         {
           try
             {
		 trace.traceStr("stream2String: Reading... \n" );
		 bytesRead = is.read( buf, 0, MAX_READ_LEN );
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
		       if( firstRead == true )
			   {
			       trace.traceStr("First read. \n" );

			       // Look in first read and get the content length
			       int contentIndex =
				   inputString.indexOf( HTTP_CONTENTLENGTH );
			       int endOfLine =
				   inputString.indexOf( EOL, contentIndex );
			       String clString = inputString.substring(
					             contentIndex, endOfLine );
			       StringTokenizer commandTok =
				   new StringTokenizer( clString,
							HTTP_TAGDELIMETER );
			       String nextToken = commandTok.nextToken(); // Tag
			       nextToken = commandTok.nextToken();       // Value
			       String valString = nextToken.trim();
			       trace.traceStr("valString=" +
					      valString + "\n" );
			       contentLength = Integer.parseInt( valString );
			       contentOffset = inputString.indexOf( EOH,
								   endOfLine );
			       numBytesToRead = contentLength + contentOffset -
				                bytesRead;
			       trace.traceStr("contentLength =" +
					      contentLength + "\n" );
			       trace.traceStr("contentOffset =" +
					      contentOffset + "\n" );
			       trace.traceStr("numBytesToRead = " +
					      numBytesToRead + "\n" );
			       firstRead = false;
			   }
		       else
			   {
			       numBytesToRead -= bytesRead;
			   }
		       if( numBytesToRead <= 0 )
			   {
			       throw new EOFException();
			   }
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
         throw soe;
       }
       catch ( EOFException eofe)
       {
	 trace.traceStr("End of file \nstream2String: EXIT \n" );
         return( sbuf.toString() );
       }
       catch ( IOException ioe)
       {
         log.logStr("stream2String: "+ioe.toString());
	 trace.traceStr("stream2String: EXIT \n" );
         throw ioe;
       }
       catch ( NumberFormatException nfe)
       {
         log.logStr("CIMServlet Exception: NumberFormatException " + nfe.getMessage());
         trace.traceStr("CIMServlet Exception: NumberFormatException " + nfe.getMessage());
	 nfe.printStackTrace( trace.getPrintWriter() );
	 trace.traceStr("stream2String: EXIT \n" );
	 throw nfe;
       }
    }
  }

  //*********************************************************************
  /**
   *  Reads the next HTTP header line from the CIMOM response
   *
   * @return
   *   The next line of the HTTP header without the "\r\n"
   */
  //*********************************************************************
  private String getNextLine()
  {
    int lineLen = 0;
    String returnString = null;

    trace.traceStr("getNextLine(): ENTRY \n" );
    trace.traceStr("getNextLine(): Entry sbIndex " + sbIndex + "\n" );
    trace.traceStr("getNextLine(): Entry sbLen " + sbLen + "\n" );
    try
      {
        for ( int i = sbIndex; i < sbLen; i++, lineLen++ )
          {
	      if( (String.valueOf(sb.charAt(i))).equals( "\r" ) )
              {
		  if( (String.valueOf(sb.charAt(i+1))).equals( "\n" ) )
                  {
                    // Return the substring
                    if( lineLen > 0 )
                      {
                        returnString = sb.substring( sbIndex, i );
                        sbIndex = i + 2;
                        break;
                      }
		    else
		      {
			  // End of headers "\r\n\r\n"
			    sbIndex = i + 2;
			    trace.traceStr("getNextLine(): returnString: end of headers" );
			    return( "\r\n" );
		      }
                  }
              }
          }
      }
    catch( StringIndexOutOfBoundsException sioob )
      {
        // TODO: Handle internal error
        log.logStr("Exception: String index out of bounds " +
                                                 sioob.getMessage() + "\n" );
      }
    catch( IndexOutOfBoundsException ioob )
      {
        // TODO: Handle internal error
        log.logStr("Exception: Index out of bounds " +
                                                 ioob.getMessage() + "\n" );
      }
    trace.traceStr("getNextLine(): returnString: \n"  + returnString );
    trace.traceStr("getNextLine(): Exit sbIndex " + sbIndex + "\n" );
    trace.traceStr("getNextLine(): EXIT \n" );
    return returnString;
  }

  //*********************************************************************
  /**
   *  Gets the CIM-XML payload.
    *
    * @return
    *   A byte array of the CIM-XML payload.
    */
    //*********************************************************************
  private byte[] getResByteArray()
  {
    trace.traceStr("getResByteArray(): ENTRY \n" );
    trace.traceStr("getResByteArray(): sbIndex-> " + sbIndex);
    trace.traceStr("getResByteArray(): sbLen-> "   + sbLen);

    int lineLen = 0;
    String payloadString = null;

    if( sbIndex >= sbLen )
      {
        // No payload passed
        return null;
      }
    try
      {
        payloadString = sb.substring( sbIndex, sbLen );
	trace.traceStr("getResByteArray(): payloadString=\n" + payloadString );
      }
    catch( StringIndexOutOfBoundsException sioob )
      {
        // TODO: Handle internal error
        log.logStr("getResByteArray(): String index out of bounds " +
                                                 sioob.getMessage() + "\n" );
      }
    trace.traceStr("getResByteArray(): EXIT \n" );
    return payloadString.getBytes();
  }
}
