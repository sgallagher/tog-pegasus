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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Magda
//              Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

import java.util.*;

public class CIMOMHandle implements ProviderCIMOMHandle
{
   private int    cInst;
   private String providerName;

   private native int  _getClass               (int           inst,
                                                int           cop,
                                                boolean       localOnly,
                                                boolean       includeQualifiers,
                                                boolean       includeClassOrigin,
                                                String[]      propertyList)
      throws CIMException;
   private native int  _deleteClass            (int           cInst,
                                                int           cop)
      throws CIMException;
   private native int  _createClass            (int           inst,
                                                int           cop,
                                                int           cls)
      throws CIMException;
   private native void _setClass               (int           inst,
                                                int           cop,
                                                int           cls)
      throws CIMException;
   private native int  _enumerateClassNames    (int           inst,
                                                int           cop,
                                                boolean       deep)
      throws CIMException;
   private native int  _enumerateClasses       (int           inst,
                                                int           cop,
                                                boolean       deep,
                                                boolean       localOnly,
                                                boolean       includeQualifier,
                                                boolean       includeClassOrigin)
      throws CIMException;

   private native int  _getInstance            (int           cInst,
                                                int           cop,
                                                boolean       localOnly,
                                                boolean       includeQualifiers,
                                                boolean       includeClassOrigin,
                                                String[]      propertyList)
      throws CIMException;
   private native int  _deleteInstance         (int           cInst,
                                                int           cop)
      throws CIMException;
   private native int  _createInstance         (int           cc,
                                                int           path,
                                                int           ci)
      throws CIMException;
   private native void _modifyInstance         (int           cc,
                                                int           path,
                                                int           ci,
                                                boolean       includeQualifiers,
                                                String[]      propertyList)
      throws CIMException;
   private native int  _enumerateInstanceNames (int           inst,
                                                int           cop,
                                                boolean       deep)
      throws CIMException;
   private native int  _enumerateInstances     (int           inst,
                                                int           cop,
                                                boolean       deep,
                                                boolean       localOnly,
                                                boolean       includeQualifiers,
                                                boolean       includeClassOrigin,
                                                String[]      propertyList)
      throws CIMException;
   private native int _execQuery               (int           inst,
                                                int           cop,
                                                String        query,
                                                String        ql)
      throws CIMException;

   private native int _getProperty             (int           inst,
                                                int           cop,
                                                String        n)
      throws CIMException;
   private native void _setProperty            (int           inst,
                                                int           cop,
                                                String        nm,
                                                int           val)
      throws CIMException;

   private native int  _associatorNames        (int           cc,
                                                int           path,
                                                String        assocClass,
                                                String        resultClass,
                                                String        role,
                                                String        resultRole)
      throws CIMException;
   private native int  _associators            (int           cc,
                                                int           path,
                                                String        assocClass,
                                                String        resultClass,
                                                String        role,
                                                String        resultRole,
                                                boolean       includeQualifiers,
                                                boolean       includeClassOrigin,
                                                String[]      propertyList)
      throws CIMException;
   private native int  _referenceNames         (int           cc,
                                                int           path,
                                                String        resultClass,
                                                String        role)
      throws CIMException;
   private native int  _references             (int           cc,
                                                int           path,
                                                String        resultClass,
                                                String        role,
                                                boolean       includeQualifiers,
                                                boolean       includeClassOrigin,
                                                String[]      propertyList)
      throws CIMException;
   private native int  _invokeMethod           (int           cc,
                                                int           path,
                                                String        methodName,
                                                Vector        inParams,
                                                Vector        outParams)
      throws CIMException;
   private native int  _invokeMethod24         (int           cc,
                                                int           path,
                                                String        methodName,
                                                CIMArgument[] inParams,
                                                CIMArgument[] outParams)
       throws CIMException;
   private native void _deliverEvent           (int           cInst,
                                                String        nm,
                                                String        ns,
                                                int           ind)
      throws CIMException;
   private native void _finalize               (int           cInst);

   protected void finalize ()
   {
      _finalize(cInst);
   }

   protected int cInst ()
   {
      return cInst;
   }

   CIMOMHandle (int    ci,
                String name)
   {
      cInst        = ci;
      providerName = name;
   }

    /**
     * @deprecated
     * @see #getClasses(CIMObjectPath name, boolean localOnly, boolean includeQualifiers,
     *         boolean includeClassOrigin, String propertyList[])
     */
   public CIMClass getClass (CIMObjectPath name,
                             boolean       localOnly)
        throws CIMException
   {
      int ciClass = 0;

      if (cInst != 0)
      {
         ciClass = _getClass (cInst,
                              name.cInst (),
                              localOnly,
                              true,
                              true,
                              null);
      }

      if (ciClass != 0)
      {
         return new CIMClass (ciClass);
      }
      else
      {
         return null;
      }
   }

   public CIMClass getClass (CIMObjectPath name,
                             boolean       localOnly,
                             boolean       includeQualifiers,
                             boolean       includeClassOrigin,
                             String[]      propertyList)
        throws CIMException
   {
      int ciClass = 0;

      if (cInst != 0)
      {
         ciClass = _getClass (cInst,
                              name.cInst (),
                              localOnly,
                              includeQualifiers,
                              includeClassOrigin,
                              propertyList);
      }

      if (ciClass != 0)
      {
         return new CIMClass (ciClass);
      }
      else
      {
         return null;
      }
   }

   public void createClass (CIMObjectPath path,
                            CIMClass      cc)
        throws CIMException
   {
      if (cInst != 0)
      {
         _createClass (cInst,
                       path.cInst (),
                       cc.cInst ());
      }
   }

   public void setClass (CIMObjectPath path,
                         CIMClass      cc)
        throws CIMException
   {
      if (cInst != 0)
      {
         _setClass (cInst,
                    path.cInst (),
                    cc.cInst ());
      }
   }

   public void modifyClass (CIMObjectPath path,
                            CIMClass      modifiedClass)
        throws CIMException
   {
      setClass(path,
               modifiedClass);
   }

   public void deleteClass (CIMObjectPath cop)
        throws CIMException
   {
      if (cInst != 0)
      {
         _deleteClass (cInst,
                       cop.cInst ());
      }
   }

   public Enumeration enumClass (CIMObjectPath path,
                                 boolean       deep,
                                 boolean       localOnly)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _enumerateClasses (cInst,
                                            path.cInst (),
                                            deep,
                                            localOnly,
                                            true,
                                            true);
      }

      if (ciEnumeration != 0)
      {
         return new ClassEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration enumerateClasses (CIMObjectPath name,
                                        boolean       deep,
                                        boolean       localOnly,
                                        boolean       includeQualifier,
                                        boolean       includeClassOrigin)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _enumerateClasses (cInst,
                                            name.cInst (),
                                            deep,
                                            localOnly,
                                            includeQualifier,
                                            includeClassOrigin);
      }

      if (ciEnumeration != 0)
      {
         return new ClassEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration enumClass (CIMObjectPath path,
                                 boolean       deep)
        throws CIMException
   {
      return enumerateClassNames(path,
                                 deep);
   }

   public Enumeration enumerateClassNames (CIMObjectPath path,
                                           boolean       deepInheritance)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _enumerateClassNames (cInst,
                                               path.cInst (),
                                               deepInheritance);
      }

      if (ciEnumeration != 0)
      {
         return new PathEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public CIMInstance getInstance (CIMObjectPath path,
                                   boolean       localOnly)
        throws CIMException
   {
      return getInstance(path,
                         localOnly,
                         true,
                         false,
                         (String[])null);
   }

   public CIMInstance getInstance (CIMObjectPath path,
                                   boolean       localOnly,
                                   boolean       includeQualifiers,
                                   boolean       includeClassOrigin,
                                   String[]      propertyList)
        throws CIMException
   {
      int ciInstance = 0;

      if (cInst != 0)
      {
         ciInstance = _getInstance (cInst,
                                    path.cInst (),
                                    localOnly,
                                    includeQualifiers,
                                    includeClassOrigin,
                                    propertyList);
      }

      if (ciInstance != 0)
      {
         return new CIMInstance (ciInstance);
      }
      else
      {
         return null;
      }
   }

   public void deleteInstance (CIMObjectPath cop)
        throws CIMException
   {
      if (cInst != 0)
      {
         _deleteInstance (cInst,
                          cop.cInst());
      }
   }

   public CIMObjectPath createInstance (CIMObjectPath path,
                                        CIMInstance   ci)
        throws CIMException
   {
      int ciObjectPath = 0;

      if (cInst != 0)
      {
         ciObjectPath = _createInstance (cInst,
                                         path.cInst (),
                                         ci.cInst ());
      }

      if (ciObjectPath != 0)
      {
         return new CIMObjectPath (ciObjectPath);
      }
      else
      {
         return null;
      }
   }

   public void setInstance (CIMObjectPath path,
                            CIMInstance   ci)
        throws CIMException
   {
      modifyInstance(path,
                     ci,
                     true,
                     (String[])null);
   }

   public void modifyInstance (CIMObjectPath path,
                               CIMInstance   modifiedInstance,
                               boolean       includeQualifiers,
                               String[]      propertyList)
        throws CIMException
   {
      if (cInst != 0)
      {
         _modifyInstance (cInst,
                          path.cInst (),
                          modifiedInstance.cInst (),
                          includeQualifiers,
                          propertyList);
      }
   }

   public Enumeration enumInstances (CIMObjectPath path,
                                     boolean       deep)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _enumerateInstanceNames (cInst,
                                                  path.cInst (),
                                                  deep);
      }

      if (ciEnumeration != 0)
      {
         return new PathEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration enumerateInstanceNames (CIMObjectPath path)
        throws CIMException
   {
       return enumInstances(path,
                            true);
   }

   public Enumeration enumInstances (CIMObjectPath cop,
                                     boolean       deep,
                                     boolean       localOnly)
         throws CIMException
   {
      return enumerateInstances(cop,
                                deep,
                                localOnly,
                                true,
                                true,
                                (String[])null);
   }

   public Enumeration enumerateInstances (CIMObjectPath path,
                                          boolean       deepInheritance,
                                          boolean       localOnly,
                                          boolean       includeQualifiers,
                                          boolean       includeClassOrigin,
                                          String[]      propertyList)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _enumerateInstances (cInst,
                                              path.cInst (),
                                              deepInheritance,
                                              localOnly,
                                              includeQualifiers,
                                              includeClassOrigin,
                                              propertyList);
      }

      if (ciEnumeration != 0)
      {
         return new InstEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration execQuery (CIMObjectPath path,
                                 String        query,
                                 String        ql)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _execQuery (cInst,
                                     path.cInst (),
                                     query,
                                     ql);
      }

      if (ciEnumeration != 0)
      {
         return new InstEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public CIMValue getProperty (CIMObjectPath path,
                                String        propertyName)
        throws CIMException
   {
      int ciValue = 0;

      if (cInst != 0)
      {
         ciValue = _getProperty (cInst,
                                 path.cInst (),
                                 propertyName);
      }

      if (ciValue != 0)
      {
         return new CIMValue (ciValue);
      }
      else
      {
         return null;
      }
   }

   public void setProperty (CIMObjectPath path,
                            String        propertyName,
                            CIMValue      newValue)
        throws CIMException
   {
      if (cInst != 0)
      {
         _setProperty (cInst,
                       path.cInst (),
                       propertyName,
                       newValue.cInst ());
      }
   }

   public Enumeration associatorNames (CIMObjectPath path,
                                       String        assocClass,
                                       String        resultClass,
                                       String        role,
                                       String        resultRole)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _associatorNames (cInst,
                                           path.cInst (),
                                           assocClass,
                                           resultClass,
                                           role,
                                           resultRole);
      }

      if (ciEnumeration != 0)
      {
         return new PathEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration associators(CIMObjectPath path,
                                  String        assocClass,
                                  String        resultClass,
                                  String        role,
                                  String        resultRole,
                                  boolean       includeQualifiers,
                                  boolean       includeClassOrigin,
                                  String[]      propertyList)
          throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _associators (cInst,
                                       path.cInst (),
                                       assocClass,
                                       resultClass,
                                       role,
                                       resultRole,
                                       includeQualifiers,
                                       includeClassOrigin,
                                       propertyList);
      }

      if (ciEnumeration != 0)
      {
         return new InstEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration referenceNames (CIMObjectPath path,
                                      String        resultClass,
                                      String        role)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _referenceNames (cInst,
                                          path.cInst (),
                                          resultClass,
                                          role);
      }

      if (ciEnumeration != 0)
      {
         return new PathEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public Enumeration references (CIMObjectPath path,
                                  String        resultClass,
                                  String        role,
                                  boolean       includeQualifiers,
                                  boolean       includeClassOrigin,
                                  String[]      propertyList)
        throws CIMException
   {
      int ciEnumeration = 0;

      if (cInst != 0)
      {
         ciEnumeration = _references (cInst,
                                      path.cInst (),
                                      resultClass,
                                      role,
                                      includeQualifiers,
                                      includeClassOrigin,
                                      propertyList);
      }

      if (ciEnumeration != 0)
      {
         return new InstEnumeration (ciEnumeration);
      }
      else
      {
         return null;
      }
   }

   public CIMValue invokeMethod (CIMObjectPath path,
                                 String        methodName,
                                 Vector        inParams,
                                 Vector        outParams)
        throws CIMException
   {
      int ciValue = 0;

      if (cInst != 0)
      {
         ciValue = _invokeMethod (cInst,
                                  path.cInst (),
                                  methodName,
                                  inParams,
                                  outParams);
      }

      if (ciValue != 0)
      {
         return new CIMValue (ciValue);
      }
      else
      {
         return null;
      }
   }

   public CIMValue invokeMethod (CIMObjectPath path,
                                 String        methodName,
                                 CIMArgument[] inParams,
                                 CIMArgument[] outParams)
        throws CIMException
   {
      throw new CIMException(CIMException.CIM_ERR_NOT_SUPPORTED);
      // return new CIMValue(_invokeMethod24(cInst,cNsInst,path.cInst (),methodName,inParams,outParams));
   }

   public void deliverEvent (String      ns,
                             CIMInstance ind)
        throws CIMException
   {
      if (cInst != 0)
      {
         _deliverEvent (cInst,
                        providerName,
                        ns,
                        ind.cInst ());
      }
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
}
