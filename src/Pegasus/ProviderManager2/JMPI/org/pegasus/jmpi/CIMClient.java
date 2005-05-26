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
// Modified By: Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi;

import java.util.*;
import org.pegasus.jmpi.*;

public class CIMClient {

    public static final String  WQL  = "WQL";
    public static final String  WQL1 = "WQL1";
    public static final String  WQL2 = "WQL2";
    public static final int     HTTP = 2;
    public static final int     XML  = 2;
    public static final boolean DEEP = true;

    CIMNameSpace ns;
    int          cInst;
    int          cNsInst;

    private native int    _newNaUnPw              (int           name,
                                                   String        userName,
                                                   String        passWord);

    private native void   _disconnect             (int           cc);
    private native int    _getClass               (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       localOnly,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin,
                                                   String        propertyList[]);
    private native int    _deleteClass            (int           cc,
                                                   int           ns,
                                                   int           path);
    private native void   _createClass            (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   int           ci);
    private native void   _setClass               (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   int           ci);

    private native int    _getInstance            (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       localOnly,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin,
                                                   String        propertyList[]);
    private native void   _deleteInstance         (int           cc,
                                                   int           ns,
                                                   int           path);
    private native int    _createInstance         (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   int           ci);
    private native void   _modifyInstance         (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   int           ci,
                                                   boolean       includeQualifiers,
                                                   String        propertyList[]);

    private native int    _enumerateClasses       (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       deep,
                                                   boolean       localOnly,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin);
    private native int    _enumerateClassNames    (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       deep);
    private native int    _enumerateInstanceNames (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       deep);
    private native int    _enumerateInstances     (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   boolean       deep,
                                                   boolean       localOnly,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin,
                                                   String        propertyList[]);
    private native int    _enumerateQualifiers    (int           cc,
                                                   int           ns,
                                                   int           path);

    private native int    _getQualifier           (int           cc,
                                                   int           ns,
                                                   int           path);
    private native void   _setQualifier           (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   int           type);
    private native void   _deleteQualifier        (int           cc,
                                                   int           ns,
                                                   int           path);

    private native int    _getProperty            (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        propertyName);
    private native void   _setProperty            (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        propertyName,
                                                   int           newValue);

    private native int    _execQuery              (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        query,
                                                   String        ql);

    private native int    _invokeMethod           (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        methodName,
                                                   Vector        inParams,
                                                   Vector        outParams)
          throws CIMException;
    private native int    _invokeMethod24         (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        methodName,
                                                   CIMArgument[] inParams,
                                                   CIMArgument[] outParams)
          throws CIMException;

    private native int    _associatorNames        (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        assocClass,
                                                   String        resultClass,
                                                   String        role,
                                                   String        resultRole);
    private native int    _associators            (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        assocClass,
                                                   String        resultClass,
                                                   String        role,
                                                   String        resultRole,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin,
                                                   String        propertyList[]);
    private native int    _referenceNames         (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        resultClass,
                                                   String        role);
    private native int    _references             (int           cc,
                                                   int           ns,
                                                   int           path,
                                                   String        resultClass,
                                                   String        role,
                                                   boolean       includeQualifiers,
                                                   boolean       includeClassOrigin,
                                                   String        propertyList[]);

    private native void   _createNameSpace        (int           cc,
                                                   String        ns);
    private native Vector _enumerateNameSpaces    (int           cc,
                                                   int           path,
                                                   boolean       deep,
                                                   Vector        v);
    private native void   _deleteNameSpace        (int           cc,
                                                   String        ns);

    private native void   _finalize               (int           cInst);

    protected void finalize ()
    {
       _finalize(cInst);
    }

    public CIMNameSpace getNameSpace ()
    {
        return ns;
    }

    public CIMClient(CIMNameSpace nameSpace,
                     String       userName,
                     String       pword)
         throws CIMException
    {
       ns=nameSpace;
       cNsInst=nameSpace.cInst;
       cInst=_newNaUnPw(cNsInst,userName,pword);
    }

    public CIMClient (CIMNameSpace nameSpace,
                      String       userName,
                      String       pword,
                      int          type)
         throws CIMException
    {
       if (type!=HTTP)
          throw new CIMException("Specified protocol type not supported.");

       ns=nameSpace;
       cNsInst=nameSpace.cInst;
       cInst=_newNaUnPw(cNsInst,userName,pword);
    }
/*
    public CIMClient(CIMNameSpace name,
                     String       userName,
                     String       pword,
                     String       roleName,
                     String       rolePwd)
         throws CIMException
    {
    }
    public CIMClient(CIMNameSpace name,
                     String       userName,
                     String       pword,
                     String       roleName,
                     String       rolePwd,
                     int          type)
         throws CIMException
    {
    }
*/

    public void close()
         throws CIMException
    {
       _disconnect (cInst);
    }

    public void createNameSpace (CIMNameSpace ins)
         throws CIMException
    {
       _createNameSpace(cInst,ins.getNameSpace());
    }

    public void deleteNameSpace (CIMNameSpace cns)
         throws CIMException
    {
       _deleteNameSpace(cInst,cns.getNameSpace());
    }
    public void deleteClass (CIMObjectPath path)
         throws CIMException
    {
        _deleteClass(cInst,cNsInst,path.cInst);
    }

    public void deleteInstance (CIMObjectPath path)
         throws CIMException
    {
        _deleteInstance(cInst,cNsInst,path.cInst);
    }


    public Enumeration enumNameSpace (CIMObjectPath path,
                                      boolean       deep)
         throws CIMException
    {
        return _enumerateNameSpaces(cInst,path.cInst,deep, new Vector()).elements();
    }

    public Enumeration enumerateNameSpaces (CIMObjectPath path,
                                            boolean       deepInheritance)
         throws CIMException
    {
        return enumNameSpace(path, deepInheritance);
    }

    public Enumeration enumClass (CIMObjectPath path,
                                  boolean       deep,
                                  boolean       local)
         throws CIMException
    {
        return new ClassEnumeration(_enumerateClasses(cInst,
                                                      cNsInst,
                                                      path.cInst,
                                                      deep,
                                                      local,
                                                      true,
                                                      false));
    }

    public Enumeration enumerateClasses (CIMObjectPath path,
                                         boolean       deepInheritance,
                                         boolean       localOnly,
                                         boolean       includeQualifiers,
                                         boolean       includeClassOrigin)
         throws CIMException
    {
        return new ClassEnumeration(_enumerateClasses(cInst,
                                                      cNsInst,
                                                      path.cInst,
                                                      deepInheritance,
                                                      localOnly,
                                                      includeQualifiers,
                                                      includeClassOrigin));
    }

    public Enumeration enumClass (CIMObjectPath path,
                                  boolean       deep)
         throws CIMException
    {
       return new PathEnumeration(_enumerateClassNames(cInst,
                                                       cNsInst,
                                                       path.cInst,
                                                       deep));
    }

    public Enumeration enumerateClassNames (CIMObjectPath path,
                                            boolean       deep)
         throws CIMException
    {
       return enumClass(path,deep);
    }

    public Enumeration enumInstances (CIMObjectPath path,
                                      boolean       deep)
         throws CIMException
    {
       return new PathEnumeration(_enumerateInstanceNames(cInst,
                                                          cNsInst,
                                                          path.cInst,
                                                          deep));
    }

    public Enumeration enumerateInstanceNames (CIMObjectPath path)
         throws CIMException
    {
       return enumInstances(path,false);
    }

    public Enumeration enumInstances (CIMObjectPath path,
                                      boolean       deep,
                                      boolean       localOnly)
         throws CIMException
    {
       return enumerateInstances(path,
                                 deep,
                                 localOnly,
                                 true,
                                 false,
                                 (String[])null);
    }

    public Enumeration enumerateInstances (CIMObjectPath path,
                                           boolean       deepInheritance,
                                           boolean       localOnly,
                                           boolean       includeQualifiers,
                                           boolean       includeClassOrigin,
                                           String        propertyList[])
         throws CIMException
    {
       return new InstEnumeration(_enumerateInstances(cInst,
                                                      cNsInst,
                                                      path.cInst,
                                                      deepInheritance,
                                                      localOnly,
                                                      includeQualifiers,
                                                      includeClassOrigin,
                                                      propertyList));
    }

    public CIMClass getClass (CIMObjectPath path,
                              boolean       localOnly)
         throws CIMException
    {
       return getClass(path,localOnly,true,false,(String[])null);
    }

    public CIMClass getClass (CIMObjectPath path,
                              boolean       localOnly,
                              boolean       includeQualifiers,
                              boolean       includeClassOrigin,
                              String        propertyList[])
         throws CIMException
    {
       return new CIMClass(_getClass(cInst,
                                     cNsInst,
                                     path.cInst,
                                     localOnly,
                                     includeQualifiers,
                                     includeClassOrigin,
                                     propertyList));
    }

    public CIMInstance getInstance (CIMObjectPath path,
                                    boolean       localOnly)
         throws CIMException
    {
       return getInstance(path,localOnly,true,false,(String[])null);
    }

    public CIMInstance getInstance (CIMObjectPath path,
                                    boolean       localOnly,
                                    boolean       includeQualifiers,
                                    boolean       includeClassOrigin,
                                    String        propertyList[])
         throws CIMException
    {
       return new CIMInstance(_getInstance(cInst,
                                           cNsInst,
                                           path.cInst,
                                           localOnly,
                                           includeQualifiers,
                                           includeClassOrigin,
                                           propertyList));
    }

    public CIMValue invokeMethod (CIMObjectPath path,
                                  String        methodName,
                                  Vector        inParams,
                                  Vector        outParams)
         throws CIMException
    {
       return new CIMValue(_invokeMethod(cInst,
                                         cNsInst,
                                         path.cInst,
                                         methodName,
                                         inParams,
                                         outParams));
    }

    public CIMValue invokeMethod (CIMObjectPath path,
                                  String        methodName,
                                  CIMArgument[] inParams,
                                  CIMArgument[] outParams)
         throws CIMException
    {
       throw new CIMException(CIMException.CIM_ERR_NOT_SUPPORTED);
///////return new CIMValue(_invokeMethod24(cInst,cNsInst,path.cInst,methodName,inParams,outParams));
    }


    public Enumeration enumQualifierTypes (CIMObjectPath path)

         throws CIMException
    {
        return new QualEnumeration(_enumerateQualifiers(cInst,cNsInst,path.cInst));
    }

    public Enumeration enumerateQualifiers (CIMObjectPath path)

         throws CIMException
    {
        return new QualEnumeration(_enumerateQualifiers(cInst,cNsInst,path.cInst));
    }

    public void deleteQualifierType (CIMObjectPath path)
         throws CIMException
    {
        _deleteQualifier(cInst,cNsInst,path.cInst);
    }

    public void deleteQualifier (CIMObjectPath path)
         throws CIMException
    {
        _deleteQualifier(cInst,cNsInst,path.cInst);
    }

    public CIMQualifierType getQualifierType (CIMObjectPath path)

         throws CIMException
    {
         return new CIMQualifierType(_getQualifier(cInst,cNsInst,path.cInst));
    }

    public CIMQualifierType getQualifier (CIMObjectPath path)

         throws CIMException
    {
         return new CIMQualifierType(_getQualifier(cInst,cNsInst,path.cInst));
    }

    public void createQualifierType (CIMObjectPath    path,
                                     CIMQualifierType qt)
         throws CIMException
    {
         _setQualifier(cInst,cNsInst,path.cInst,qt.cInst);
    }

    public void setQualifierType (CIMObjectPath    path,
                                  CIMQualifierType qt)
         throws CIMException
    {
         _setQualifier(cInst,cNsInst,path.cInst,qt.cInst);
    }

    public void setQualifier (CIMObjectPath    path,
                              CIMQualifierType qualifierType)
         throws CIMException
    {
       setQualifierType(path,qualifierType);
    }

   public void createClass (CIMObjectPath path,
                            CIMClass      cc)
         throws CIMException
    {
       _createClass(cInst,cNsInst,path.cInst,cc.cInst);
    }

    public void setClass (CIMObjectPath path,
                          CIMClass      cc)
         throws CIMException
    {
       _setClass(cInst,cNsInst,path.cInst,cc.cInst);
    }

    public void modifyClass (CIMObjectPath path,
                             CIMClass      modifiedClass)
         throws CIMException
    {
       setClass(path,modifiedClass);
    }

    public CIMObjectPath createInstance (CIMObjectPath path,
                                         CIMInstance   ci)
         throws CIMException
    {
       return new CIMObjectPath(_createInstance(cInst,cNsInst,path.cInst,ci.cInst));
    }

    public void setInstance (CIMObjectPath path,
                             CIMInstance   ci)
         throws CIMException
    {
       modifyInstance(path,ci,true,(String[])null);
    }

    public void modifyInstance (CIMObjectPath path,
                                CIMInstance   modifiedInstance,
                                boolean       includeQualifiers,
                                String        propertyList[])
         throws CIMException
    {
       _modifyInstance(cInst,
                       cNsInst,
                       path.cInst,
                       modifiedInstance.cInst,
                       includeQualifiers,
                       propertyList);
    }

    public CIMValue getProperty (CIMObjectPath path,
                                 String        propertyName)
         throws CIMException
    {
       return new CIMValue(_getProperty(cInst,
                                        cNsInst,
                                        path.cInst,
                                        propertyName));
    }

    public void setProperty (CIMObjectPath path,
                             String        propertyName,
                             CIMValue      newValue)
         throws CIMException
    {
       _setProperty(cInst,cNsInst,path.cInst,propertyName,newValue.cInst);
    }

    public Enumeration execQuery(CIMObjectPath path,
                                 String        query,
                                 String        ql)
         throws CIMException
    {
       return new InstEnumeration(_execQuery(cInst,cNsInst,path.cInst,query,ql));
    }

    public Enumeration associatorNames (CIMObjectPath path,
                                        String        assocClass,
                                        String        resultClass,
                                        String        role,
                                        String        resultRole)

         throws CIMException
    {
       return new PathEnumeration(_associatorNames(cInst,
                                                   cNsInst,
                                                   path.cInst,
                                                   assocClass,
                                                   resultClass,
                                                   role,
                                                   resultRole));
    }

    public Enumeration associators (CIMObjectPath path,
                                    String        assocClass,
                                    String        resultClass,
                                    String        role,
                                    String        resultRole,
                                    boolean       includeQualifiers,
                                    boolean       includeClassOrigin,
                                    String        propertyList[])

         throws CIMException
    {
        return new InstEnumeration(_associators(cInst,
                                                cNsInst,
                                                path.cInst,
                                                assocClass,
                                                resultClass,
                                                role,
                                                resultRole,
                                                includeQualifiers,
                                                includeClassOrigin,
                                                propertyList));
    }

    public Enumeration referenceNames (CIMObjectPath path,
                                       String        resultClass,
                                       String        role)

         throws CIMException
    {
       return new PathEnumeration(_referenceNames(cInst,
                                                  cNsInst,
                                                  path.cInst,
                                                  resultClass,
                                                  role));
    }

    public Enumeration references (CIMObjectPath path,
                                   String        resultClass,
                                   String        role,
                                   boolean       includeQualifiers,
                                   boolean       includeClassOrigin,
                                   String        propertyList[])

         throws CIMException
    {
       return new InstEnumeration(_references(cInst,
                                              cNsInst,
                                              path.cInst,
                                              resultClass,
                                              role,
                                              includeQualifiers,
                                              includeClassOrigin,
                                              propertyList));
    }
/*
    public CIMNameSpace getNameSpace ()
    {
    }

    public CIMFeatures getServerFeatures ()
         throws CIMException
    {
    }

    public void authenticate (String userName,
                              String password)
         throws CIMException
    {
    }
*/
    static {
       System.loadLibrary("JMPIProviderManager");
    }
}
