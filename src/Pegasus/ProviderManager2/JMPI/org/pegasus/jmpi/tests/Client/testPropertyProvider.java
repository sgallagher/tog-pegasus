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

import java.util.Enumeration;
import java.util.Vector;
import org.pegasus.jmpi.CIMClient;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMInstance;
import org.pegasus.jmpi.CIMObjectPath;
import org.pegasus.jmpi.CIMProperty;
import org.pegasus.jmpi.CIMValue;

public class testPropertyProvider
{
   final String                className      = "JMPIProperty_TestPropertyTypes";
   final String                nameSpaceClass = "root/SampleProvider";
   final String                hostName       = "localhost";

   private static boolean      DEBUG          = false;

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
      boolean fRet      = true;

      for (int i = 0; i < args.length; i++)
      {
         if (args[i].equalsIgnoreCase ("debug"))
         {
            setDebug (true);
         }
         else if (args[i].equalsIgnoreCase ("get"))
         {
            if (fRet)
            {
               fRet = testGetProperty (cimClient);
            }

            fExecuted = true;
         }
         else if (args[i].equalsIgnoreCase ("set"))
         {
            if (fRet)
            {
               fRet = testSetProperty (cimClient);
            }

            fExecuted = true;
         }
      }

      if (!fExecuted)
      {
         if (fRet)
         {
            fRet = testGetProperty (cimClient);
         }

         if (fRet)
         {
            fRet = testSetProperty (cimClient);
         }
      }

      return fRet;
   }

   private boolean testSetProperty (CIMClient cimClient)
   {
      try
      {
         CIMObjectPath cop1             = null;
         CIMInstance   cimInstance1     = null;
         CIMInstance   cimInstance1a    = null;

         if (DEBUG)
            System.err.println ("cimClient     = " + cimClient);

         if (DEBUG)
            System.err.println ("enumInstances (" + cop1 + ", true)");

         cop1 = new CIMObjectPath (className, nameSpaceClass);
         if (cop1 == null)
         {
            if (DEBUG)
               System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceClass + "');");

            System.out.println ("FAIL: testSetProperty");

            return false;
         }

         cop1.addKey ("InstanceId", new CIMValue ("1"));

         if (DEBUG)
            System.err.println ("cop1          = " + cop1);

         Enumeration enum = cimClient.enumInstances (cop1, true);

         if (DEBUG)
            System.err.println ("enum = " + enum + ", hasMoreElements = " + enum.hasMoreElements ());

         while (enum.hasMoreElements ())
         {
            CIMObjectPath copRet = (CIMObjectPath)enum.nextElement ();
            Vector        keys   = copRet.getKeys ();

            for (int i = 0; i < keys.size (); i++)
            {
               CIMProperty cp         = (CIMProperty)keys.elementAt (i);
               CIMValue    cv         = cp.getValue ();
               Object      oRealValue = cv.getValue ();

               if (DEBUG)
               {
                  System.err.println ("cp         = " + cp);
                  System.err.println ("cv         = " + cv);
                  System.err.println ("oRealValue = " + oRealValue);
               }
            }
         }

         cimInstance1 = cimClient.getInstance (cop1, false);

         if (DEBUG)
            System.err.println ("cimInstance1  = " + cimInstance1);

         cimClient.setProperty (cop1, "PropertyString", new CIMValue (new String ("The second property string.")));

         cimInstance1a = cimClient.getInstance (cop1, false);

         if (DEBUG)
            System.err.println ("cimInstance1a = " + cimInstance1a);

         System.out.println ("SUCCESS: testSetProperty");

         return true;
      }
      catch (CIMException e)
      {
          System.err.println ("Error: Exception: " + e);
          e.printStackTrace ();
      }

      return false;
   }

   private boolean testGetProperty (CIMClient cimClient)
   {
      try
      {
         CIMObjectPath cop1             = null;
         CIMInstance   cimInstance1     = null;
         CIMInstance   cimInstance1a    = null;

         if (DEBUG)
            System.err.println ("cimClient     = " + cimClient);

         cop1 = new CIMObjectPath (className, nameSpaceClass);
         if (cop1 == null)
         {
            if (DEBUG)
               System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceClass + "');");

            System.out.println ("FAIL: testGetProperty");

            return false;
         }

         cop1.addKey ("InstanceId", new CIMValue ("1"));

         if (DEBUG)
            System.err.println ("cop1          = " + cop1);

         cimInstance1 = cimClient.getInstance (cop1, false);

         if (DEBUG)
            System.err.println ("cimInstance1  = " + cimInstance1);

         CIMValue cv = cimClient.getProperty (cop1, "PropertyString");

         if (DEBUG)
            System.err.println ("cv            = " + cv);

         System.out.println ("SUCCESS: testGetProperty");

         return true;
      }
      catch (CIMException e)
      {
         System.err.println ("Error: Exception: " + e);
         e.printStackTrace ();
      }

      return false;
   }
}
