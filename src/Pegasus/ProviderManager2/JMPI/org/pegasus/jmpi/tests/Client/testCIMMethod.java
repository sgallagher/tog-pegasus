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

import org.pegasus.jmpi.CIMClass;
import org.pegasus.jmpi.CIMClient;
import org.pegasus.jmpi.CIMDataType;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMMethod;
import org.pegasus.jmpi.CIMObjectPath;

public class testCIMMethod
{
   private boolean DEBUG = false;

   /**
    * This returns the group name.
    *
    * @return String "class" testcase belongs in.
    */
   public String getGroup ()
   {
      return "CIMMethod";
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
         fRet = testCIMMethod (cimClient);
      }

      return fRet;
   }

   private boolean testCIMMethod (CIMClient client)
   {
      if (client == null)
      {
         System.out.println ("FAILURE: testCIMMethod: client == null");

         return false;
      }

      // -----

      CIMObjectPath cop = null;
      CIMClass      cc  = null;

      cop = new CIMObjectPath ("JMPIExpInstance_TestPropertyTypes",
                               "root/SampleProvider");

      if (DEBUG)
      {
         System.out.println ("testCIMMethod: cop = " + cop);
      }

      try
      {
         cc = client.getClass (cop,
                               true,  // propagated
                               true,  // includeQualifiers
                               true,  // includeClassOrigin
                               null); // propertyList
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMMethod: client.getClass: caught " + e);

         return false;
      }

      if (cc == null)
      {
         System.out.println ("FAILURE: testCIMMethod: cc == null");

         return false;
      }

      if (DEBUG)
      {
         System.out.println ("testCIMMethod: cc = " + cc);
      }

      // -----

      CIMMethod cm = null;

      cm = cc.getMethod ("enableModifications");

      if (DEBUG)
      {
         System.out.println ("testCIMMethod: cm = " + cm);
      }

      if (cm == null)
      {
         System.out.println ("FAILURE: testCIMMethod: cc.getMethod ()");

         return false;
      }

      // -----

      int iType = 0;

      iType = cm.getType ();

      if (DEBUG)
      {
         System.out.println ("testCIMMethod: cm.getType () = " + iType);
      }

      if (iType != CIMDataType.BOOLEAN)
      {
         System.out.println ("FAILURE: testCIMMethod: cm.getType ()");

         return false;
      }

      // -----

      System.out.println ("SUCCESS: testCIMMethod");

      return true;
   }
}
