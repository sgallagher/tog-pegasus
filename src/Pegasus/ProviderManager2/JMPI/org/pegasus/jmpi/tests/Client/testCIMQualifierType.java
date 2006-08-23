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
//%/////////////////////////////////////////////////////////////////////////////
package Client;

import org.pegasus.jmpi.CIMClient;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMObjectPath;
import org.pegasus.jmpi.CIMQualifierType;
import java.util.Enumeration;

public class testCIMQualifierType
{
   private boolean DEBUG = false;

   /**
    * This returns the group name.
    *
    * @return String "class" testcase belongs in.
    */
   public String getGroup ()
   {
      return "CIMQualifierType";
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
      }

      if (!fExecuted)
      {
         fRet = testCIMQualifierType (cimClient);
      }

      return fRet;
   }

   private boolean testCIMQualifierType (CIMClient client)
   {
      if (client == null)
      {
         System.out.println ("FAILURE: testCIMQualifierType: client == null");

         return false;
      }

      // -----

      CIMObjectPath cop  = null;
      Enumeration   enum = null;

      cop = new CIMObjectPath ("JMPIExpInstance_TestPropertyTypes",
                               "root/SampleProvider");

      if (DEBUG)
      {
         System.out.println ("testCIMQualifierType: cop = " + cop);
      }

      try
      {
         enum = client.enumQualifierTypes (cop);
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMQualifierType: client.enumerateQualifierTypes: caught " + e);

         return false;
      }

      if (enum == null)
      {
         System.out.println ("FAILURE: testCIMQualifierType: enum == null");

         return false;
      }

      // -----

      while (enum.hasMoreElements ())
      {
         CIMQualifierType cqt = (CIMQualifierType)enum.nextElement ();

         if (cqt == null)
         {
            System.out.println ("FAILURE: testCIMQualifierType: enum.next ()");

            return false;
         }

         String pszName = null;

         pszName = cqt.getName ();

         if (DEBUG)
         {
            System.out.println ("testCIMQualifierType: pszName = " + pszName);
         }

         if (pszName == null)
         {
            System.out.println ("FAILURE: testCIMQualifierType: cqt.getName ()");

            return false;
         }

         cqt.setName (pszName);
      }

      // -----

      System.out.println ("SUCCESS: testCIMQualifierType");

      return true;
   }
}
