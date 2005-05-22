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
// Author:      Mark Hamzy, hamzy@us.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

import org.pegasus.jmpi.*;
import java.util.*;
import java.math.BigInteger;

public class JMPI_RT_TestExecQuery1
{
    final String className      = "JMPI_RT_ExecQuery1";
    final String nameSpaceClass = "test/static";
    final String hostName       = "localhost";

    boolean      fDebug         = true;

    public void setDebug (boolean fDebug)
    {
        this.fDebug = fDebug;
    }

    public void testExecQuery ()
    {
        CIMClient cimClient = null;

        try
        {
            String        nameSpaceInterOp = "test/static";
            int           portNo           = 5988;
            String        username         = "";
            String        password         = "";
            String        urlAttach        = "http://" + hostName + ":" + portNo;
            CIMNameSpace  clientNameSpace  = null;
            CIMObjectPath cop              = null;
            CIMInstance   cimInstance1     = null;
            CIMInstance   cimInstance2     = null;
            CIMObjectPath retCop           = null;

            clientNameSpace = new CIMNameSpace (urlAttach, nameSpaceInterOp);
            if (clientNameSpace == null)
            {
                System.err.println ("Error: Could not create a CIMNameSpace ('" + urlAttach + "', '" + nameSpaceInterOp + "');");
                System.exit (1);
            }

            try
            {
                cimClient = new CIMClient (clientNameSpace, username, password);
            }
            catch (Exception e)
            {
                System.err.println ("Caught " + e);
                e.printStackTrace ();
            }
            if (cimClient == null)
            {
                System.err.println ("Error: Could not create a CIMClient ('" + clientNameSpace + "', '" + username + "', '" + password + "');");
                System.exit (1);
            }

            if (this.fDebug)
                System.out.println ("cimClient    = " + cimClient);

            cop = new CIMObjectPath (className, nameSpaceInterOp);
            if (cop == null)
            {
                System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceInterOp + "');");
                System.exit (1);
            }

            if (this.fDebug)
                System.out.println ("cop          = " + cop);

            testExecQuery (cimClient);
        }
        catch (CIMException e)
        {
            System.err.println ("Error: Exception: " + e);
            e.printStackTrace ();
        }
        finally
        {
            if (cimClient != null)
            {
                try
                {
                    cimClient.close ();
                }
                catch (CIMException e)
                {
                  System.err.println ("Error: Exception: " + e);
                  e.printStackTrace ();
                }
            }
        }
    }

    private void testExecQuery (CIMClient cimClient) throws CIMException
    {
        String        queryStatement = null;
        String        queryLanguage  = null;
        CIMObjectPath cimObjectPath  = new CIMObjectPath (className, nameSpaceClass);
        Enumeration   enum           = null;

        queryStatement = "Select * from " + className + " where InstanceId = 1";
        queryLanguage  = "WQL";

        if (this.fDebug)
            System.out.println ("execQuery (" + cimObjectPath + ", " + queryStatement + ", " + queryLanguage);

        enum = cimClient.execQuery (cimObjectPath, queryStatement, queryLanguage);

        if (this.fDebug)
            System.out.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

        while (enum.hasMoreElements())
        {
           CIMInstance cimInstanceEQ = (CIMInstance)(enum.nextElement());

           System.out.println (cimInstanceEQ);
        }

        queryStatement = "Select * from " + className + " where InstanceId = 2";
        queryLanguage  = "WQL";

        if (this.fDebug)
            System.out.println ("execQuery (" + cimObjectPath + ", " + queryStatement + ", " + queryLanguage);

        enum = cimClient.execQuery (cimObjectPath, queryStatement, queryLanguage);

        if (this.fDebug)
            System.out.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

        while (enum.hasMoreElements())
        {
           CIMInstance cimInstanceEQ = (CIMInstance)(enum.nextElement());

           System.out.println (cimInstanceEQ);
        }
    }

    public static void main (String args[])
    {
        JMPI_RT_TestExecQuery1 instance  = new JMPI_RT_TestExecQuery1 ();
        boolean                fExecuted = false;

        for (int i = 0; i < args.length; i++)
        {
            if (args[i].equalsIgnoreCase ("debug"))
            {
                instance.setDebug (true);
            }
        }

        if (!fExecuted)
            instance.testExecQuery ();
    }
}
