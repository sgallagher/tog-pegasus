//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Based on: RT_IndicationProvider.cpp by Carol Ann Krug Graves, Hewlett-Packard Company
//                                        (carolann_graves@hp.com)

// Author: Adrian Schuur, IBM (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi.tests.ProcessIndication;

import org.pegasus.jmpi.*;
import java.util.*;



public class JMPI_RT_SampleProvider implements MethodProvider, EventProvider
{
   CIMOMHandle ch;
   int nextUID = 0;
   int enabled=0;

   void generateIndication(String methodname)
                        throws CIMException {

      CIMInstance inst;
      CIMObjectPath cop;
      CIMDateTime dat;
      CIMClass cls;

      if (enabled==0) return;

      System.err.println("+++ generateIndication()");

      cop=new CIMObjectPath("JMPI_RT_SampleIndication","test/static");
      cls=ch.getClass(cop,false);
      inst=cls.newInstance();

      String buffer=""+(nextUID++);
      inst.setProperty("IndicationIdentifier",new CIMValue(buffer));

      dat=new CIMDateTime();
      inst.setProperty("IndicationTime",new CIMValue(dat));

      inst.setProperty("MethodName",new CIMValue(methodname));

      Vector ar=new Vector();
      ar.addElement(new String("test"));
      inst.setProperty("CorrelatedIndications",new CIMValue(ar));

//      CMAddContextEntry(ctx,"SnmpTrapOidContainer","1.3.6.1.4.1.900.2.3.9002.9600",CMPI_chars);

      ch.deliverEvent("test/static",inst);

      System.err.println("+++ generateIndication() done");
   }



   public JMPI_RT_SampleProvider() {
   }

   public void cleanup()
                throws CIMException {
   }

   public void initialize(CIMOMHandle ch)
                        throws CIMException {
     this.ch=ch;
   }




   public CIMValue invokeMethod(CIMObjectPath op,
                               String method,
                               Vector in,
                               Vector out)
                        throws CIMException {

      System.err.println("+++ testProvInvokeMethod()");
      if (enabled==0)
         System.err.println("+++ PROVIDER NOT ENABLED");
      else generateIndication(method);
      return new CIMValue("hello");
   }





   public void authorizeFilter(SelectExp filter,
                                String eventType,
                                CIMObjectPath classPath,
                                String owner)
                        throws CIMException {
   }

   public boolean mustPoll(SelectExp filter,
                            String eventType,
                            CIMObjectPath classPath)
                        throws CIMException {
       return false;
   }


   public void activateFilter(SelectExp filter,
                               String eventType,
                               CIMObjectPath classPath,
                               boolean firstActivation)
                        throws CIMException {
      System.err.println("+++ testProvActivateFilter()");
      enabled++;
      System.err.println("--- enabled: "+enabled);
   }


   public void deActivateFilter(SelectExp filter,
                                 String eventType,
                                 CIMObjectPath classPath,
                                 boolean lastActivation)
                        throws CIMException {
      System.err.println("+++ testProvDeActivateFilter()");
      enabled=0;
      System.err.println("--- enabled: "+enabled);
   }

}
