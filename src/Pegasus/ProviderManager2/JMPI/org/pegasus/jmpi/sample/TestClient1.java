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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi.sample;

import org.pegasus.jmpi.*;
import java.util.*;

public class TestClient1 {

    public static void main(String args[]) throws CIMException {

       System.out.println("--- TestClient1 started");

       CIMClient cc=null;

       try {

//        int proto=CIMClient.XML;
          String hostName="localhost";
          String nameSpace="root/cimv2";
          int portNo=5988;
          int listenerPortNo=8189;
          String classText="";
          String username="a3";
          String password="a3";
          String attachName=hostName;

          attachName="http://"+hostName+":"+portNo;


          for (int i=0; i<args.length; i++) {
             if (args[i].equalsIgnoreCase("-u")) {
                i++;
                if (i>=args.length) {
                    System.err.println("A user name is required");
                } else {
                    username=args[i];
                }
             }
             else if (args[i].equalsIgnoreCase("-h")) {
                i++;
                if (i>=args.length) {
                    System.err.println("A hostname is required");
                } else {
                    hostName=args[i];
                }
             }
             else if (args[i].equalsIgnoreCase("-p")) {
                i++;
                if (i>=args.length) {
                    System.err.println("A port number is required");
                } else {
                    Integer intv=new Integer(args[i]);
                    portNo=intv.intValue();
                }
             }
          }

          attachName="http://"+hostName+":"+portNo;

          CIMNameSpace clientNameSpace=new CIMNameSpace(attachName,nameSpace);

          cc=new CIMClient(clientNameSpace,
             username,password);

          CIMObjectPath cop=new CIMObjectPath("Cim_ManagedElement","root/cimv2");

          CIMClass cls=cc.getClass(cop,false);

          System.out.println("--- 5 "+attachName);

          Enumeration enum=cc.enumInstances(cop,false,true);
          while (enum.hasMoreElements()) {
             CIMInstance obj=(CIMInstance)(enum.nextElement());
             System.err.println(obj.toString());
          }

          Enumeration enum2=cc.enumInstances(cop,false);
          while (enum2.hasMoreElements()) {
             CIMObjectPath obj=(CIMObjectPath)(enum2.nextElement());
//           System.err.println(obj.toString());
          }

/*
          String query=
                  "TRACE select id "+
                  "from Test_VirtualMachine "+
                  "where id!=\"VM4\"";
          Enumeration enum=cc.execQuery(new CIMObjectPath("Test_VirtualMachine"),query,1);

          while (enum.hasMoreElements()) {
             CIMInstance obj=(CIMInstance)(enum.nextElement());
             System.err.println("--- obj: "+obj.toString());
          }
*/
          cc.close();
       }
       catch (CIMException e) {
         System.err.println("--- Exception: "+e);
       }
    }
}
