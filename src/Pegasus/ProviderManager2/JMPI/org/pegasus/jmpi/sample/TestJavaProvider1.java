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

package org.pegasus.sample;

import org.pegasus.jmpi.*;
import java.util.*;


public class TestJavaProvider1 implements InstanceProvider
{
   private Hashtable data=new Hashtable();
   CIMOMHandle ch;

   public TestJavaProvider1() {
      System.err.println("~~~ TestProvider::TestProvider()");
   }
   public void initialize(CIMOMHandle ch)
                throws CIMException {
      System.err.println("~~~ TestProvider::Initialize()");
      this.ch=ch;
   }

   public void cleanup()
                throws CIMException { 
   }



   public CIMObjectPath createInstance(CIMObjectPath op,
                               CIMInstance ci)
                        throws CIMException {
      System.err.println("~~~ TestProvider::createInstance()");
      CIMProperty cpk=ci.getProperty("Identifier");
      Object k=cpk.getValue().getValue();
      CIMProperty cpv=ci.getProperty("data");
      Object d=cpv.getValue().getValue();

      if (data.get(k)!=null)
         throw new CIMException(CIMException.CIM_ERR_ALREADY_EXISTS);

      data.put((String)k,(String)d);
      op.addKey("Identifier",new CIMValue(k));
      op.addKey("Data",new CIMValue(d));
      return op;
   }



   public CIMInstance getInstance(CIMObjectPath op,
                               CIMClass cc,
                               boolean localOnly)
                        throws CIMException {
//      System.err.println("~~~ TestProvider::getInstance()");
      Vector vec=op.getKeys();
      CIMProperty cp=(CIMProperty)vec.elementAt(0);
      CIMValue cv=cp.getValue();
      String k=(String)cv.getValue();
      String d=(String)data.get(k);

     if (d!=null) {
         CIMInstance ci=cc.newInstance();
         ci.setProperty("Identifier",cv);
         ci.setProperty("data",new CIMValue(d));
         return ci;
      }
      return null;
   }



   public void setInstance(CIMObjectPath cop,
                               CIMInstance cimInstance)
                        throws CIMException  {
      System.err.println("~~~ TestProvider::setInstance()");
      CIMProperty cpk=cimInstance.getProperty("Identifier");
      Object k=cpk.getValue().getValue();
      CIMProperty cpv=cimInstance.getProperty("data");
      Object d=cpv.getValue().getValue();

      if (data.get(k)==null)
         throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);

      data.put((String)k,(String)d);
   }



   public void deleteInstance(CIMObjectPath cop)
                        throws CIMException  {
      System.err.println("~~~ TestProvider::deleteInstance()");
      Vector vec=cop.getKeys();
      CIMProperty cp=(CIMProperty)vec.elementAt(0);
      Object k=cp.getValue().getValue();

      if (data.get(k)==null)
         throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);

      data.remove((String)k);
   }



   public Vector enumInstances(CIMObjectPath cop,
                               boolean deep,
                               CIMClass cimClass)
                        throws CIMException {
      Enumeration en=data.keys();
      Vector vec=new Vector();
      while (en.hasMoreElements()) {
         String k=(String)en.nextElement();
         String d=(String)data.get(k);
         CIMObjectPath op=new CIMObjectPath();
         op.setObjectName(cop.getObjectName());
         op.addKey("Identifier",new CIMValue(k));
         vec.addElement(op);
     }
      return vec;
   }



   public Vector enumInstances(CIMObjectPath cop,
                               boolean deep,
                               CIMClass cimClass,
                               boolean localOnly)
                        throws CIMException {
      Enumeration en=data.keys();
      Vector vec=new Vector();
      while (en.hasMoreElements()) {
         String k=(String)en.nextElement();
         String d=(String)data.get(k);
         CIMInstance ci=cimClass.newInstance();
         ci.setProperty("Identifier",new CIMValue(k));
         ci.setProperty("data",new CIMValue(d));
         vec.addElement(ci);
      }
      return vec;
   }



   public Vector execQuery(CIMObjectPath op,
                               String queryStatement,
                               int ql,
                               CIMClass cimClass)
                        throws CIMException {
      return null;
   }

}






