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
/
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
 *  Trace logs trace messages to a trace file
 *  </p>
 *  <p>
 *  @see         org.opengroup.pegasus.servlet.CIMServlet
 *  </p>
 */

public class Trace
{
    // Trace flag used for logging trace messages
    private boolean     isTracing = false;
    private PrintWriter tracePW   = null;


    // ************************************************************************
    /**
     *   The default constructor.
     **/
    // ************************************************************************

    public Trace()
    {
    }

    // ************************************************************************
    /**
     *   The default constructor. This constructor creates a trace file and an
     * associated PrintWriter.
     * @param logDir
     *   Directory path to the trace file
     * @param traceFileName
     *   File name of the trace file
     **/
    // ************************************************************************

    public Trace( String logDir,
		String traceFileName )
    {
	try
	    {
		tracePW = new PrintWriter( new FileOutputStream(
							      logDir+traceFileName,
							      true));
		isTracing = true;
	    }
	catch( IOException ioe )
	    {
		System.err.println( "Unable to open servlet trace file." +
				    ioe.getMessage());
	    }
    }


    //*********************************************************************
    /**
    * Get trace file PrintWriter
    *
    * @return
    *    Trace PrintWriter
    */
    //*********************************************************************
    public PrintWriter getPrintWriter()
    {
	return tracePW;
    }


    //*********************************************************************
    /**
    * Log strings to trace file
    *
    * @param  traceMsg
    *    Trace message string
    */
    //*********************************************************************
    public void traceStr( String traceMsg )
    {
        if ( isTracing )
          {
            Date date = new Date( System.currentTimeMillis() );
            tracePW.println(date.toString()+":");
            tracePW.println( traceMsg );
            tracePW.flush();
          }
    }

}
