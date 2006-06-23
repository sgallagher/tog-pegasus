//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author:      Mark Hamzy, hamzy@us.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
package Client;

import org.pegasus.jmpi.CIMClient;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMInstance;
import org.pegasus.jmpi.CIMObjectPath;
import java.util.Enumeration;
import java.util.Vector;

public class testExecQuery
{
    private final String        className           = "JMPIExpInstance_TestPropertyTypes";
    private final String        nameSpaceClass      = "root/SampleProvider";

    private static boolean      DEBUG               = false;

    private static boolean      fBugExistsExecQuery = false;

    /**
     * This returns the group name.
     *
     * @return String "class" testcase belongs in.
     */
    public String getGroup ()
    {
        return "instances";
    }

    public void setDebug (boolean fDebug)
    {
        DEBUG = fDebug;
    }

    public boolean main (String args[], CIMClient cimClient)
    {
        boolean fExecuted = false;

        for (int i = 0; i < args.length; i++)
        {
            if (args[i].equalsIgnoreCase ("debug"))
            {
                setDebug (true);
            }
            else if (args[i].equalsIgnoreCase ("BugExistsExecQuery"))
            {
               fBugExistsExecQuery = true;
            }
        }

        if (!fExecuted)
            return runTests (cimClient);

        return false;
    }

    public boolean runTests (CIMClient cimClient)
    {
       try
       {
          return runTest (cimClient);
       }
       catch (Exception e)
       {
          System.out.println ("Caught " + e);

          e.printStackTrace ();

          return false;
       }
    }

    private boolean runTest (CIMClient cimClient)
       throws CIMException
    {
        String        queryStatement = null;
        String        queryLanguage  = null;
        CIMObjectPath cimObjectPath  = new CIMObjectPath (className, nameSpaceClass);
        Enumeration   enum           = null;

        queryStatement = "Select * from " + className + " where InstanceId = 1";
        queryLanguage  = "WQL";

        if (DEBUG)
            System.out.println ("execQuery (" + cimObjectPath + ", " + queryStatement + ", " + queryLanguage);

        try
        {
           enum = cimClient.execQuery (cimObjectPath,
                                       queryStatement,
                                       queryLanguage);
        }
        catch (Exception e)
        {
           if (DEBUG)
           {
              System.err.println ("Caught " + e);
              e.printStackTrace ();
           }
        }

        if (enum == null)
        {
           if (fBugExistsExecQuery)
           {
              System.out.println ("IGNORE: testExecQuery: enum == null");

              return true;
           }
           else
           {
              System.out.println ("FAIL: testExecQuery: enum == null");

              return false;
           }
        }

        if (DEBUG)
            System.out.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

        while (enum.hasMoreElements())
        {
           CIMInstance cimInstanceEQ = (CIMInstance)(enum.nextElement());

           System.out.println (cimInstanceEQ);
        }

        queryStatement = "Select * from " + className + " where InstanceId = 2";
        queryLanguage  = "WQL";

        if (DEBUG)
            System.out.println ("execQuery (" + cimObjectPath + ", " + queryStatement + ", " + queryLanguage);

        try
        {
           enum = cimClient.execQuery (cimObjectPath,
                                       queryStatement,
                                       queryLanguage);
        }
        catch (Exception e)
        {
           if (DEBUG)
           {
              System.err.println ("Caught " + e);
              e.printStackTrace ();
           }
        }

        if (enum == null)
        {
           System.out.println ("FAIL: testExecQuery: enum == null");

           return false;
        }
        if (DEBUG)
            System.out.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

        while (enum.hasMoreElements())
        {
           CIMInstance cimInstanceEQ = (CIMInstance)(enum.nextElement());

           System.out.println (cimInstanceEQ);
        }

        System.out.println ("SUCCESS: testExecQuery");

        return true;
    }
}
