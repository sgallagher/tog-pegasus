
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






