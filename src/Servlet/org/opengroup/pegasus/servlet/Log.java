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

import java.io.PrintWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;

import java.util.Date;


/**
 *
 *  <h2>Log:</h2>
 *  <p>
 *  Log logs messages to a logfile
 *  </p>
 *  <p>
 *  @see         org.opengroup.pegasus.servlet.CIMServlet
 *  </p>
 */

public class Log
{
    // Logging flag used for logging requests and responses
    private boolean     isLogging = false;
    private PrintWriter logPW     = null;


    // ************************************************************************
    /**
     *   The default constructor.
     **/
    // ************************************************************************

    public Log()
    {
    }

    // ************************************************************************
    /**
     *   This constructor creates a logfile and an associated PrintWriter.
     * @param logDir
     *   Directory path to the trace file
     * @param traceFileName
     *   File name of the trace file
     **/
    // ************************************************************************

    public Log( String logDir,
		String logFileName )
    {
	//
	// CIMservlet will have its own logging file
	//
	try
	    {
		logPW = new PrintWriter( new FileOutputStream(
							      logDir+logFileName,
							      true));
		isLogging = true;
	    }
	catch( IOException ioe )
	    {
		System.err.println( "Unable to open servlet log file." +
				    ioe.getMessage());
	    }
    }

    //*********************************************************************
    /**
    * Log strings to logfile
    *
    * @param  logMsg
    *    Log message string
    */
    //*********************************************************************
    public void logStr( String logMsg )
    {
        if ( isLogging )
          {
            Date date = new Date( System.currentTimeMillis() );
            logPW.println(date.toString()+":");
            logPW.println( logMsg );
            logPW.flush();
          }
    }
}
