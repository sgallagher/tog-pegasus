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

public class JMPI_TestPropertyProvider
{
    final String className      = "JMPI_PropertyProvider";
    final String nameSpaceClass = "test/static";
    final String hostName       = "localhost";

    boolean      fDebug         = true;

    public void setDebug (boolean fDebug)
    {
        this.fDebug = fDebug;
    }

    private void testSetProperty ()
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
            CIMObjectPath cop1             = null;
            CIMInstance   cimInstance1     = null;
            CIMInstance   cimInstance1a    = null;

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
                System.out.println ("cimClient     = " + cimClient);
/*
            if (this.fDebug)
                System.out.println ("enumInstances (" + cop + ", true)");

            Enumeration enum = cimClient.enumInstances (cop, true);

            if (this.fDebug)
                System.out.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

            while (enum.hasMoreElements ())
            {
                CIMObjectPath copRet = (CIMObjectPath)enum.nextElement ();
                Vector        keys   = copRet.getKeys ();

                for (int i = 0; i < keys.size (); i++)
                {
                    CIMProperty cp         = (CIMProperty)keys.elementAt (i);
                    CIMValue    cv         = cp.getValue ();
                    Object      oRealValue = cv.getValue ();

                    if (this.fDebug)
                    {
                        System.out.println ("cp         = " + cp);
                        System.out.println ("cv         = " + cv);
                        System.out.println ("oRealValue = " + oRealValue);
                    }
                }
            }
*/
            cop1 = new CIMObjectPath (className, nameSpaceInterOp);
            if (cop1 == null)
            {
                System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceInterOp + "');");
                System.exit (1);
            }

            cop1.addKey ("InstanceId", new CIMValue ("1"));

            if (this.fDebug)
                System.out.println ("cop1          = " + cop1);

            cimInstance1 = cimClient.getInstance (cop1, false);

            if (this.fDebug)
                System.out.println ("cimInstance1  = " + cimInstance1);

            cimClient.setProperty (cop1, "PropertyString", new CIMValue (new String ("someone dies.")));

            cimInstance1a = cimClient.getInstance (cop1, false);

            if (this.fDebug)
                System.out.println ("cimInstance1a = " + cimInstance1a);
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

    private void testGetProperty ()
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
            CIMObjectPath cop1             = null;
            CIMInstance   cimInstance1     = null;
            CIMInstance   cimInstance1a    = null;

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
                System.out.println ("cimClient     = " + cimClient);

            cop1 = new CIMObjectPath (className, nameSpaceInterOp);
            if (cop1 == null)
            {
                System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceInterOp + "');");
                System.exit (1);
            }

            cop1.addKey ("InstanceId", new CIMValue ("1"));

            if (this.fDebug)
                System.out.println ("cop1          = " + cop1);

            cimInstance1 = cimClient.getInstance (cop1, false);

            if (this.fDebug)
                System.out.println ("cimInstance1  = " + cimInstance1);

            CIMValue cv = cimClient.getProperty (cop1, "PropertyString");

            if (this.fDebug)
                System.out.println ("cv            = " + cv);
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

    public static void main (String args[])
    {
        JMPI_TestPropertyProvider instance  = new JMPI_TestPropertyProvider ();
        boolean                fExecuted = false;

        for (int i = 0; i < args.length; i++)
        {
            if (args[i].equalsIgnoreCase ("debug"))
            {
                instance.setDebug (true);
            }
            else if (args[i].equalsIgnoreCase ("get"))
            {
                instance.testGetProperty ();
                fExecuted = true;
            }
            else if (args[i].equalsIgnoreCase ("set"))
            {
                instance.testSetProperty ();
                fExecuted = true;
            }
        }

        if (!fExecuted)
            instance.testSetProperty ();
    }
}
