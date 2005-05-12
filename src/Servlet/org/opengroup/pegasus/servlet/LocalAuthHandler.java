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
//%/////////////////////////////////////////////////////////////////////////////

package org.opengroup.pegasus.servlet;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
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

import org.opengroup.pegasus.servlet.AuthenticationFailedException;

public class LocalAuthHandler
{

    private static final String ROOTUSER = "root";

    private static final int MAX_READ_LEN  = 8192;
    private static final int MAX_WRITE_LEN = 8192;


    public LocalAuthHandler() { }
    //*********************************************************************
    /**
    * Write line to HTTP socket
    *
    * @param line
    *    String to write
    */
    //*********************************************************************
    private int writeToSocket(OutputStream socketOut, String line )
    {
         int ret = 0;

         System.out.println("writeToSocket(): ENTRY" );
         try
         {
             // Log HTTP output
             System.out.println( "IN: " + line + " And Len=" + line.length() );

             if( line != null )
             {
                  //socketOut.write( (line + "\r\n").getBytes() );
                  socketOut.write( line.getBytes() );
                  socketOut.flush();
             }
             else
             {
                  // End of headers
                  socketOut.write( ("\r\n").getBytes() );
                  socketOut.flush();
             }
             System.out.println("writeToSocket(): EXIT " );
          }
          catch ( IOException ioe)
          {
             System.out.println("Exception: " + ioe.getMessage());
             System.out.println("writeToSocket(): EXIT " );
             return(-1);
          }
        return(ret);
    }
    //*********************************************************************
    /**
    * Read a line to HTTP socket
    *
    * @param inStream
    *
    */
    //*********************************************************************
    private String readFromSocket(InputStream socketIn)
    {
         String inputString = null;
         int bytesRead     = 0;

         System.out.println("readFromSocket: Reading... " );

         byte[] buf = new byte[MAX_READ_LEN];

         try
         {
             bytesRead = socketIn.read( buf, 0,MAX_READ_LEN );
             if( bytesRead < 0 )
             {
                   throw new EOFException();
             }
             if( bytesRead > 0 )
             {
                 inputString = new String( buf, 0, bytesRead );
                 System.out.println( "Stream bytesRead: " + bytesRead);
                 System.out.println( "Stream IN:" + inputString);
             }
         }
         catch ( IOException ioe)
         {
             System.out.println("Exception: " + ioe.getMessage());
             System.out.println("readFromSocket(): EXIT " );
             return(null);
         }
         System.out.println("readFromSocket(): EXIT " );
         return(inputString);
    }
    //*********************************************************************
    /**
    * Read the file and send data back to caller
    *
    * @param   fileName  Name of file to read
    *
    * @return  fileData  data read from file
    */
    //*********************************************************************
    private String getFileData(String fileName)
    {
       StringBuffer sbuf = null;

      System.out.println("getFileData: Entered... " );
      System.out.println("filename : " + fileName );
       if ( fileName == null )
        return(null);

       try
       {
           //InputStream fileIn = new InputStream(new FileInputStream(fileName));
           //InputStream fileIn = (InputStream) new FileInputStream(fileName);
           BufferedReader fileIn = new BufferedReader(new FileReader(fileName));

           sbuf = new StringBuffer();
           String oneLine;

           while ( ( oneLine = fileIn.readLine()) != null)
                sbuf.append(oneLine);
       }
       catch ( IOException ioe)
       {
             System.out.println("Exception: " + ioe.getMessage());
             System.out.println("getFileData(): EXIT " );
             return(null);
       }
       System.out.println("got: " + sbuf.toString());
       System.out.println("getFileData: EXIT... " );
       return(sbuf.toString());
    }

    //*********************************************************************
    /**
    * handle authentication on servlet(client) side
    *
    * @param   xmlCIMServerInStream   Socket Input stream
    *
    * @param   xmlCIMServerInStream   Socket output stream
    */
    //*********************************************************************
    public void processAuthentication(
                     InputStream socketIn,
                     OutputStream socketOut)
                throws AuthenticationFailedException
    {
         int ret = -1;

         //
         // How to get process owner name
         // Is this right method
         //

         // Write user Name
         ret = writeToSocket(socketOut, ROOTUSER);
         if ( ret == -1)
         {
            throw new AuthenticationFailedException(
                      "socket write failed for root");
         }

         // Read Socket data : name of the File
         String filePath = readFromSocket(socketIn);
         if ( filePath == null )
         {

            // Server not responded
            throw new AuthenticationFailedException(
                      "unable to read data from socket");
         }

         // Get content from File
         String fileData = getFileData(filePath);
         if ( fileData == null )
         {
            // unable to read data
            throw new AuthenticationFailedException(
                      "failed to read from file");
         }


         // Write file Content back to Server
         ret = writeToSocket(socketOut, fileData);
         if ( ret == -1)
         {
            throw new AuthenticationFailedException(
                      "failed to write to socket");
         }

         try {
            // make sure we flush all before new
            // data
            socketOut.flush();
         }
         catch ( IOException ioe)
         {
             System.out.println("Exception: " + ioe.getMessage());
             System.out.println("getFileData(): EXIT " );
         }
    }

}
