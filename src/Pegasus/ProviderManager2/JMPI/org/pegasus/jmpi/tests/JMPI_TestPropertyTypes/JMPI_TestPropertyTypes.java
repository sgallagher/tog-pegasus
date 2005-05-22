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
// Author: Adrian Schuur schuur@deibm.com
//
// Modified By:
//
//
// This code is based on TestPropertyTypes.cpp
//
// Original Author: Yi Zhou (yi_zhou@hp.com)
//
// Original Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//                       Sushma Fernandes, Hewlett-Packard Company
//                         (sushma_fernandes@hp.com)
//                       Carol Ann Krug Graves, Hewlett-Packard Company
//                        (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi.tests.JMPI_TestPropertyTypes;

import org.pegasus.jmpi.*;
import java.util.*;


public class JMPI_TestPropertyTypes implements InstanceProvider, MethodProvider, EventProvider
{
   CIMOMHandle ch;

   Vector pathes=new Vector();
   Vector instances=new Vector();

   public JMPI_TestPropertyTypes() {
   }

   public void cleanup()
                throws CIMException {
   }

   public void initialize(CIMOMHandle ch)
                throws CIMException {
      this.ch=ch;

      CIMInstance instance1=new CIMInstance("JMPI_TestPropertyTypes");
      CIMObjectPath cop1=new CIMObjectPath("JMPI_TestPropertyTypes","test/static");

      instance1.setProperty("CreationClassName",
         new CIMValue(new String("JMPI_TestPropertyTypes")));
      cop1.addKey("CreationClassName",
         new CIMValue(new String("JMPI_TestPropertyTypes")));
      instance1.setProperty("InstanceId",
          new CIMValue(new UnsignedInt64("1")));
      cop1.addKey("InstanceId",
          new CIMValue(new UnsignedInt64("1")));
      instance1.setProperty("PropertyString",
         new CIMValue(new String("JMPI_TestPropertyTypes_Instance1")));
      instance1.setProperty("PropertyUint8",
          new CIMValue(new UnsignedInt8((short)120)));
      instance1.setProperty("PropertyUint16",
          new CIMValue(new UnsignedInt16((int)1600)));
      instance1.setProperty("PropertyUint32",
          new CIMValue(new UnsignedInt32((long)3200)));
      instance1.setProperty("PropertyUint64",
          new CIMValue(new UnsignedInt64("6400")));
      instance1.setProperty("PropertySint8",
          new CIMValue(new Byte((byte)-120)));
      instance1.setProperty("PropertySint16",
          new CIMValue(new Short((short)-1600)));
      instance1.setProperty("PropertySint32",
          new CIMValue(new Integer(-3200)));
      instance1.setProperty("PropertySint64",
          new CIMValue(new Long(-6400)));
      instance1.setProperty("PropertyBoolean",
          new CIMValue(new Boolean(true)));
      instance1.setProperty("PropertyReal32",
          new CIMValue(new Float(1.12345670123)));
      instance1.setProperty("PropertyReal64",
          new CIMValue(new Double(1.12345678906543210123)));
      instance1.setProperty("PropertyDatetime",
         new CIMValue(new CIMDateTime("20010515104354.000000:000")));

      instances.addElement(instance1);
      pathes.addElement(cop1);

      CIMInstance instance2=new CIMInstance("JMPI_TestPropertyTypes");
      CIMObjectPath cop2=new CIMObjectPath("JMPI_TestPropertyTypes","test/static");

      instance2.setProperty("CreationClassName",
         new CIMValue(new String("JMPI_TestPropertyTypes")));
      cop2.addKey("CreationClassName",
         new CIMValue(new String("JMPI_TestPropertyTypes")));
      instance2.setProperty("InstanceId",
          new CIMValue(new UnsignedInt64("2")));
      cop2.addKey("InstanceId",
          new CIMValue(new UnsignedInt64("2")));
      instance2.setProperty("PropertyString",
         new CIMValue(new String("JMPI_TestPropertyTypes_Instance2")));
      instance2.setProperty("PropertyUint8",
          new CIMValue(new UnsignedInt8((short)122)));
      instance2.setProperty("PropertyUint16",
          new CIMValue(new UnsignedInt16((int)1602)));
      instance2.setProperty("PropertyUint32",
          new CIMValue(new UnsignedInt32((long)3202)));
      instance2.setProperty("PropertyUint64",
          new CIMValue(new UnsignedInt64("6402")));
      instance2.setProperty("PropertySint8",
          new CIMValue(new Byte((byte)-122)));
      instance2.setProperty("PropertySint16",
          new CIMValue(new Short((short)-1602)));
      instance2.setProperty("PropertySint32",
          new CIMValue(new Integer(-3202)));
      instance2.setProperty("PropertySint64",
          new CIMValue(new Long(-6402)));
      instance2.setProperty("PropertyBoolean",
          new CIMValue(new Boolean(false)));
      instance2.setProperty("PropertyReal32",
          new CIMValue(new Float(2.12345670123)));
      instance2.setProperty("PropertyReal64",
          new CIMValue(new Double(2.12345678906543210123)));
      instance2.setProperty("PropertyDatetime",
         new CIMValue(new CIMDateTime("20010515104354.000000:000")));

      instances.addElement(instance2);
      pathes.addElement(cop2);
   }

   int findObjectPath(CIMObjectPath path) {
      String p=path.toString();
      for (int i=0; i<pathes.size(); i++) {
        if (((CIMObjectPath)pathes.elementAt(i)).toString().equalsIgnoreCase(p))
            return i;
      }
      return -1;
   }


   void testPropertyTypesValue(CIMInstance instanceObject)
                       throws CIMException {

      Vector properties=instanceObject.getProperties();

      int PropertyCount=properties.size();

      for (int j=0; j<PropertyCount; j++) {
         CIMProperty property=(CIMProperty)properties.elementAt(j);
         String propertyName=property.getName();
         CIMValue propertyValue=property.getValue();
         Object value=propertyValue.getValue();

         int type=property.getType().getType();

         switch (type) {
         case CIMDataType.UINT8:
            if (!(value instanceof UnsignedInt8))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((UnsignedInt8)value).intValue() >= 255)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.UINT16:
            if (!(value instanceof UnsignedInt16))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((UnsignedInt16)value).intValue() >= 10000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.UINT32:
            if (!(value instanceof UnsignedInt32))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((UnsignedInt32)value).intValue() >= 10000000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.UINT64:
            if (!(value instanceof UnsignedInt64))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((UnsignedInt64)value).longValue() >= 1000000000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.SINT8:
            if (!(value instanceof Byte))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Byte)value).intValue() <= -120)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.SINT16:
           if (!(value instanceof Short))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Short)value).intValue() < -10000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.SINT32:
            if (!(value instanceof Integer))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Integer)value).intValue() <= -10000000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.SINT64:
            if (!(value instanceof Long))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Long)value).intValue() <= -1000000000)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.REAL32:
            if (!(value instanceof Float))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Float)value).floatValue() >= 10000000.32)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         case CIMDataType.REAL64:
           if (!(value instanceof Double))
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            if (((Double)value).doubleValue() >= 1000000000.64)
               throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
            break;
         default: ;
         }
      }
   }

   public CIMObjectPath createInstance(CIMObjectPath op,
                               CIMInstance ci)
                        throws CIMException {

      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the InstanceId key is valid
      CIMProperty prop=ci.getProperty("InstanceId");
      if (prop==null)
         throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);

      CIMObjectPath rop=new CIMObjectPath("JMPI_TestPropertyTypes","test/static");
      rop.addKey("InstanceId",prop.getValue());
      rop.addKey("CreationClassName", new CIMValue("JMPI_TestPropertyTypes"));

      // ensure the property values are valid
      testPropertyTypesValue(ci);

      // Determine if a property exists in the class
      if (ci.getProperty("PropertyUint8")==null)
         throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);

      // ensure the requested object do not exist
      if (findObjectPath(op) >= 0)
         throw new CIMException(CIMException.CIM_ERR_ALREADY_EXISTS);

      return rop;
   }



   public CIMInstance getInstance(CIMObjectPath op,
                               CIMClass cc,
                               boolean localOnly)
                        throws CIMException {
      // ensure the InstanceId key is valid
      Vector keys=op.getKeys();
      int i;
      for (i=0; i<keys.size() && !((CIMProperty)keys.elementAt(i)).getName().
             equalsIgnoreCase("InstanceId");
           i++);

      if (i==keys.size())
         throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);

      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the request object exists
      int index=findObjectPath(op);
      if (index<0)
         throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);

      return (CIMInstance)instances.elementAt(index);
   }



   public void setInstance(CIMObjectPath op,
                               CIMInstance cimInstance)
                        throws CIMException  {

      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the property values are valid
      testPropertyTypesValue(cimInstance);

      // ensure the request object exists
      int index=findObjectPath(op);
      if (index<0)
         throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);
   }



   public void deleteInstance(CIMObjectPath op)
                        throws CIMException  {
      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the request object exists
      int index=findObjectPath(op);
      if (index<0)
         throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);
   }



   public Vector enumInstances(CIMObjectPath op,
                               boolean deep,
                               CIMClass cimClass)
                        throws CIMException {
      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      return pathes;
   }



   public Vector enumInstances(CIMObjectPath op,
                               boolean deep,
                               CIMClass cimClass,
                               boolean localOnly)
                        throws CIMException {
      // ensure the Namespace is valid
      if (!op.getNameSpace().equalsIgnoreCase("test/static"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!op.getObjectName().equalsIgnoreCase("JMPI_TestPropertyTypes"))
         throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

      return instances;
   }



   public CIMInstance[] execQuery (CIMObjectPath cop,
                                   String        queryStatement,
                                   String        queryLanguage,
                                   CIMClass      cimClass)
                        throws CIMException {
      return null;
   }




   public CIMValue invokeMethod(CIMObjectPath op,
                               String method,
                               Vector in,
                               Vector out)
                        throws CIMException {

      if (method.equalsIgnoreCase("SayHello"))
         return new CIMValue(new String("hello"));

      throw new CIMException(CIMException.CIM_ERR_METHOD_NOT_AVAILABLE);
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
    }


    public void deActivateFilter(SelectExp filter,
                                 String eventType,
                                 CIMObjectPath classPath,
                                 boolean lastActivation)
                        throws CIMException {
    }


}

