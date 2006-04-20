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
// Author:      Mark Hamzy, IBM (hamzy@us.ibm.com)
//
// Modified By: Mark Hamzy, IBM (hamzy@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi.tests.Indication;

import java.util.Vector;
import org.pegasus.jmpi.CIMClass;
import org.pegasus.jmpi.CIMDataType;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMInstance;
import org.pegasus.jmpi.CIMOMHandle;
import org.pegasus.jmpi.CIMObjectPath;
import org.pegasus.jmpi.CIMProperty;
import org.pegasus.jmpi.CIMValue;
import org.pegasus.jmpi.EventProvider2;
import org.pegasus.jmpi.InstanceProvider2;
import org.pegasus.jmpi.MethodProvider2;
import org.pegasus.jmpi.OperationContext;
import org.pegasus.jmpi.SelectExp;
import org.pegasus.jmpi.SelectList;
import org.pegasus.jmpi.NonJoinExp;
import org.pegasus.jmpi.QueryExp;
import org.pegasus.jmpi.UnsignedInt32;
import org.pegasus.jmpi.UnsignedInt64;

public class JMPI_IndicationProvider
             implements InstanceProvider2,
                        MethodProvider2,
                        EventProvider2
{
   private static String CLASSNAME = "TestIndication";
   private static String NAMESPACE = "test/static";
   private CIMOMHandle   ch        = null;
   private Vector        paths     = new Vector ();
   private Vector        instances = new Vector ();

   public void initialize (CIMOMHandle ch)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::initialize: ch = " + ch);

      this.ch = ch;
   }

   public void cleanup ()
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::cleanup");

      ch        = null;
      paths     = null;
      instances = null;
   }

   public CIMObjectPath createInstance (OperationContext oc,
                                        CIMObjectPath    cop,
                                        CIMInstance      cimInstance)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::createInstance: oc          = " + oc);
      System.out.println ("JMPI_IndicationProvider::createInstance: cop         = " + cop);
// @HACK
String nameSpaceCop = cop.getNameSpace ();
cop = new CIMObjectPath (cimInstance.getClassName (),
                         cimInstance.getKeyValuePairs ());
cop.setNameSpace (nameSpaceCop);
System.out.println ("JMPI_IndicationProvider::createInstance: cop         = " + cop);
      System.out.println ("JMPI_IndicationProvider::createInstance: cimInstance = " + cimInstance);

      // Ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // Ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      // Ensure that all of the properties exists in the instance
      if (  cimInstance.getProperty ("InstanceId") == null
         || cimInstance.getProperty ("PropertyString") == null
         )
         throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);

      // Ensure that all of the key properties exists in the object path
      if (cop.getKeyValue ("InstanceId") == null)
         throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);

      // Ensure the requested object do not exist
      if (findObjectPath (cop) >= 0)
         throw new CIMException (CIMException.CIM_ERR_ALREADY_EXISTS);

      paths.addElement (cop);
      instances.addElement (cimInstance);

      return cop;
   }

   public CIMInstance getInstance (OperationContext oc,
                                   CIMObjectPath    cop,
                                   CIMClass         cimClass,
                                   boolean          includeQualifiers,
                                   boolean          includeClassOrigin,
                                   String           propertyList[])
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::getInstance: oc                 = " + oc);
      System.out.println ("JMPI_IndicationProvider::getInstance: cop                = " + cop);
      System.out.println ("JMPI_IndicationProvider::getInstance: cimClass           = " + cimClass);
      System.out.println ("JMPI_IndicationProvider::getInstance: includeQualifiers  = " + includeQualifiers);
      System.out.println ("JMPI_IndicationProvider::getInstance: includeClassOrigin = " + includeClassOrigin);
      System.out.println ("JMPI_IndicationProvider::getInstance: propertyList       = " + propertyList);

      // ensure the InstanceId key is valid
      Vector keys = cop.getKeys ();
      int    i;

      for (i = 0;
           i < keys.size () && ! ((CIMProperty)keys.elementAt (i)).getName ().equalsIgnoreCase ("InstanceId");
           i++)
      {
      }

      if (i == keys.size ())
         throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the request object exists
      int index = findObjectPath (cop);

      if (index < 0)
         throw new CIMException (CIMException.CIM_ERR_NOT_FOUND);

      return (CIMInstance)instances.elementAt (index);
   }

   public void setInstance (OperationContext oc,
                            CIMObjectPath    cop,
                            CIMInstance      cimInstance)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::setInstance: oc          = " + oc);
      System.out.println ("JMPI_IndicationProvider::setInstance: cop         = " + cop);
      System.out.println ("JMPI_IndicationProvider::setInstance: cimInstance = " + cimInstance);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the request object exists
      int index = findObjectPath (cop);

      if (index < 0)
         throw new CIMException (CIMException.CIM_ERR_NOT_FOUND);
   }

   public void deleteInstance (OperationContext oc,
                               CIMObjectPath    cop)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::deleteInstance: oc  = " + oc);
      System.out.println ("JMPI_IndicationProvider::deleteInstance: cop = " + cop);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      // ensure the request object exists
      int index = findObjectPath (cop);

      if (index < 0)
         throw new CIMException (CIMException.CIM_ERR_NOT_FOUND);
   }

   public Vector enumerateInstanceNames (OperationContext oc,
                                         CIMObjectPath    cop,
                                         CIMClass         cimClass)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::enumerateInstanceNames: oc       = " + oc);
      System.out.println ("JMPI_IndicationProvider::enumerateInstanceNames: cop      = " + cop);
      System.out.println ("JMPI_IndicationProvider::enumerateInstanceNames: cimClass = " + cimClass);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      return paths;
   }

   public Vector enumerateInstances (OperationContext oc,
                                     CIMObjectPath    cop,
                                     CIMClass         cimClass,
                                     boolean          includeQualifiers,
                                     boolean          includeClassOrigin,
                                     String           propertyList[])
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: oc                 = " + oc);
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: cop                = " + cop);
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: cimClass           = " + cimClass);
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: includeQualifiers  = " + includeQualifiers);
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: includeClassOrigin = " + includeClassOrigin);
      System.out.println ("JMPI_IndicationProvider::enumerateInstances: propertyList       = " + propertyList);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

      return instances;
   }

   public Vector execQuery (OperationContext oc,
                            CIMObjectPath    cop,
                            CIMClass         cimClass,
                            String           queryStatement,
                            String           queryLanguage)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::execQuery: oc             = " + oc);
      System.out.println ("JMPI_IndicationProvider::execQuery: cop            = " + cop);
      System.out.println ("JMPI_IndicationProvider::execQuery: cimClass       = " + cimClass);
      System.out.println ("JMPI_IndicationProvider::execQuery: queryStatement = " + queryStatement);
      System.out.println ("JMPI_IndicationProvider::execQuery: queryLanguage  = " + queryLanguage);

      if (!queryLanguage.equals ("WQL"))
      {
         throw new CIMException (CIMException.CIM_ERR_NOT_SUPPORTED);
      }

      SelectExp     q         = new SelectExp (queryStatement);
      SelectList    attrs     = q.getSelectList ();
      NonJoinExp    from      = (NonJoinExp)q.getFromClause ();
      QueryExp      where     = q.getWhereClause ();
      Vector        instances = enumerateInstances (oc,
                                                    cop,
                                                    cimClass,
                                                    true,
                                                    true,
                                                    null);
      Vector        ret       = new Vector ();

      // filter the instances
      for (int i = 0; i < instances.size (); i++)
      {
         if (  where == null
            || where.apply ((CIMInstance)instances.elementAt (i))
            )
         {
            ret.addElement (attrs.apply ((CIMInstance)instances.elementAt (i)));
         }
      }

      return ret;
   }

   public CIMValue invokeMethod (OperationContext oc,
                                 CIMObjectPath    cop,
                                 String           methodName,
                                 Vector           inArgs,
                                 Vector           outArgs)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::invokeMethod: oc         = " + oc);
      System.out.println ("JMPI_IndicationProvider::invokeMethod: cop        = " + cop);
      System.out.println ("JMPI_IndicationProvider::invokeMethod: methodName = " + methodName);
      System.out.println ("JMPI_IndicationProvider::invokeMethod: inArgs     = " + inArgs);
      System.out.println ("JMPI_IndicationProvider::invokeMethod: outArgs    = " + outArgs);

      // ensure the Namespace is valid
      if (!cop.getNameSpace ().equalsIgnoreCase (NAMESPACE))
      {
         throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);
      }

      // ensure the class existing in the specified namespace
      if (!cop.getObjectName ().equalsIgnoreCase (CLASSNAME))
      {
         throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);
      }

      if (methodName.equalsIgnoreCase ("SendTestIndicationNormal"))
      {
         if (inArgs == null)
         {
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);
         }
         else if (inArgs.size () != 1)
         {
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);
         }

         CIMProperty cp = (CIMProperty)inArgs.elementAt (0);

         if (!cp.getName ().equalsIgnoreCase ("indicationSendCount"))
         {
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);
         }
         else if (cp.getType ().getType () != CIMDataType.UINT64)
         {
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);
         }

         CIMValue      cvIndicationSendCount = cp.getValue ();
         UnsignedInt64 uiIndicationSendCount = (UnsignedInt64)cvIndicationSendCount.getValue ();

         System.out.println ("JMPI_IndicationProvider::invokeMethod: uiIndicationSendCount = " + uiIndicationSendCount);

         CIMClass      ccIndication;
         CIMInstance   ciIndication;
         CIMObjectPath copIndication;

         ccIndication = ch.getClass (cop,
                                     true,
                                     true,
                                     true,
                                     null);
         ciIndication = ccIndication.newInstance ();

         ciIndication.setProperty ("InstanceId", new CIMValue (uiIndicationSendCount));
         ciIndication.setProperty ("PropertyString", new CIMValue ("Hello"));

         copIndication = createInstance (oc,
                                         cop,
                                         ciIndication);

         System.out.println ("JMPI_IndicationProvider::invokeMethod: deliverEvent: copIndication = " + copIndication);
         System.out.println ("JMPI_IndicationProvider::invokeMethod: deliverEvent: ciIndication  = " + ciIndication);

         ch.deliverEvent (copIndication.getNameSpace (),
                          ciIndication);

         return new CIMValue (new UnsignedInt32 ("0"));
      }
      else
      {
         throw new CIMException (CIMException.CIM_ERR_METHOD_NOT_AVAILABLE);
      }
   }

   public void authorizeFilter (OperationContext oc,
                                SelectExp        filter,
                                String           eventType,
                                CIMObjectPath    classPath,
                                String           owner)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::authorizeFilter: oc        = " + oc);
      System.out.println ("JMPI_IndicationProvider::authorizeFilter: filter    = " + filter);
      System.out.println ("JMPI_IndicationProvider::authorizeFilter: eventType = " + eventType);
      System.out.println ("JMPI_IndicationProvider::authorizeFilter: classPath = " + classPath);
      System.out.println ("JMPI_IndicationProvider::authorizeFilter: owner     = " + owner);

      if (filter != null)
      {
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectString () = " + filter.getSelectString ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getWhereClause ()  = " + filter.getWhereClause ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectList ()   = " + filter.getSelectList ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getFromClause ()   = " + filter.getFromClause ());
      }
   }

   public boolean mustPoll (OperationContext oc,
                            SelectExp        filter,
                            String           eventType,
                            CIMObjectPath    classPath)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::mustPoll: oc        = " + oc);
      System.out.println ("JMPI_IndicationProvider::mustPoll: filter    = " + filter);
      System.out.println ("JMPI_IndicationProvider::mustPoll: eventType = " + eventType);
      System.out.println ("JMPI_IndicationProvider::mustPoll: classPath = " + classPath);

      if (filter != null)
      {
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectString () = " + filter.getSelectString ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getWhereClause ()  = " + filter.getWhereClause ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectList ()   = " + filter.getSelectList ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getFromClause ()   = " + filter.getFromClause ());
      }

      return false;
   }

   public void activateFilter (OperationContext oc,
                               SelectExp        filter,
                               String           eventType,
                               CIMObjectPath    classPath,
                               boolean          firstActivation)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::activateFilter: oc              = " + oc);
      System.out.println ("JMPI_IndicationProvider::activateFilter: filter          = " + filter);
      System.out.println ("JMPI_IndicationProvider::activateFilter: eventType       = " + eventType);
      System.out.println ("JMPI_IndicationProvider::activateFilter: classPath       = " + classPath);
      System.out.println ("JMPI_IndicationProvider::activateFilter: firstActivation = " + firstActivation);

      if (filter != null)
      {
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectString () = " + filter.getSelectString ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getWhereClause ()  = " + filter.getWhereClause ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectList ()   = " + filter.getSelectList ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getFromClause ()   = " + filter.getFromClause ());
      }
   }

   public void deActivateFilter (OperationContext oc,
                                 SelectExp        filter,
                                 String           eventType,
                                 CIMObjectPath    classPath,
                                 boolean          lastActivation)
      throws CIMException
   {
      System.out.println ("JMPI_IndicationProvider::deActivateFilter: oc             = " + oc);
      System.out.println ("JMPI_IndicationProvider::deActivateFilter: filter         = " + filter);
      System.out.println ("JMPI_IndicationProvider::deActivateFilter: eventType      = " + eventType);
      System.out.println ("JMPI_IndicationProvider::deActivateFilter: classPath      = " + classPath);
      System.out.println ("JMPI_IndicationProvider::deActivateFilter: lastActivation = " + lastActivation);

      if (filter != null)
      {
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectString () = " + filter.getSelectString ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getWhereClause ()  = " + filter.getWhereClause ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getSelectList ()   = " + filter.getSelectList ());
         System.out.println ("JMPI_IndicationProvider::activateFilter: filter.getFromClause ()   = " + filter.getFromClause ());
      }
   }

   private int findObjectPath (CIMObjectPath path)
   {
      String p = path.toString ();

      for (int i = 0; i < paths.size (); i++)
      {
         System.out.println ("testing " + paths.elementAt (i) + " against " + path);

         if (((CIMObjectPath)paths.elementAt (i)).toString ().equalsIgnoreCase (p))
            return i;
      }

      return -1;
   }
}
