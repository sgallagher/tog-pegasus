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
// Author:      Adrian Schuur, schuur@de.ibm.com 
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi;

import java.util.*;
import org.pegasus.jmpi.*;

public class CIMClient {
    int cInst;
    int cNsInst;
    
    private native int  _newNaUnPw(int name, String userName, String passWord);
    private native void _disconnect(int cc);
    private native int  _getClass(int cc, int path, boolean localOnly);
    private native int  _getInstance(int cc, int path, boolean localOnly);
    private native int  _enumerateInstanceNames(int cc, int path, boolean deep);
    private native int  _enumerateInstances(int cc, int path, boolean deep, boolean localOnly);
    private native int  _createInstance(int cc, int path, int ci);      
    private native void _setInstance(int cc, int path, int ci);
    private native int  _getProperty(int cc, int path, String propertyName);      
    private native void _setProperty(int cc, int path, String propertyName, int newValue);
    private native void _deleteInstance(int cc, int path);
    
    private native int  _execQuery(int cc, int path, String query, String ql);
    
    private native int  _associatorNames(int cc, int path,
               String assocClass, String resultClass, String role, String resultRole); 
    private native int  _associators(int cc, int path,
               String assocClass, String resultClass, String role, String resultRole,
               boolean includeQualifiers, boolean includeClassOrigin, String propertyList[]);
    private native int  _referenceNames(int cc, int path,
               String resultClass, String role);
    private native int  _references(int cc, int path,
               String resultClass, String role,
               boolean includeQualifiers, boolean includeClassOrigin, String propertyList[]); 
	    
    /*
    public CIMClient() 
                throws CIMException {
    }
    public CIMClient(CIMNameSpace name) 
		throws CIMException {
    } */
    public CIMClient(CIMNameSpace name,  
                     String userName,
		     String pword) 
		throws CIMException {
       cNsInst=name.cInst;
       cInst=_newNaUnPw(cNsInst,userName,pword);
    } /*
    public CIMClient(CIMNameSpace name, 
                     String userName,
		     String pword,int type)
	        throws CIMException{
    } 
    public CIMClient(CIMNameSpace name,
                     String userName,
		     String pword,
		     String roleName,
		     String rolePwd) 
		throws CIMException {
    } 
    public CIMClient(CIMNameSpace name,
                     String userName,
		     String pword,
		     String roleName,
		     String rolePwd,
		     int type) 
		throws CIMException {
    } */
    /*
    public void createNameSpace(CIMNameSpace ins) throws CIMException{
    }    static {
       System.loadLibrary("JMPIProviderManager");
    }
*/
    public void close() throws CIMException {
       _disconnect(cInst);
    }
/*    
    public void deleteNameSpace(CIMNameSpace cns) throws CIMException{
    }
    public void deleteClass(CIMObjectPath path) throws CIMException{
    }
*/    
    public void deleteInstance(CIMObjectPath path) throws CIMException {
        _deleteInstance(cInst,path.cInst);
   }
/*    
    public void deleteQualifierType(CIMObjectPath path) throws CIMException{
    }
    public Enumeration enumNameSpace(CIMObjectPath path,boolean deep) 
		throws CIMException {
    }
    
    public Enumeration enumQualifierTypes(CIMObjectPath path) 
		throws CIMException{
    }
    public Enumeration enumClass(CIMObjectPath path,
		   boolean deep,
		   boolean local) throws CIMException {
    }  
    public Enumeration enumClass(CIMObjectPath path,
		   boolean deep) throws CIMException {
    }
*/
    public Enumeration enumInstances(CIMObjectPath path,
		       boolean deep) throws CIMException {
       return new PathEnumeration(_enumerateInstanceNames(cInst,path.cInst,deep));
    }
    public Enumeration enumInstances(CIMObjectPath path,
		       boolean deep,
		       boolean localOnly) throws CIMException {
       return new InstEnumeration(_enumerateInstances(cInst,path.cInst,deep,localOnly));
    }
    public CIMClass getClass(CIMObjectPath path,
          boolean localOnly) throws CIMException {
       System.out.println("-#- "+cInst);   
       return new CIMClass(_getClass(cInst,path.cInst,localOnly));      
    }
    public CIMInstance getInstance(CIMObjectPath path,
          boolean localOnly) throws CIMException {
       return new CIMInstance(_getInstance(cInst,path.cInst,localOnly));      
    }
/*    
    public CIMValue invokeMethod(CIMObjectPath path,
		   String methodName,
		   Vector inParams,
		   Vector outParams) throws CIMException{
    }
    public CIMQualifierType getQualifierType(CIMObjectPath path) 
	    throws CIMException{
    }
    public void createQualifierType(CIMObjectPath path,
		      CIMQualifierType qt) throws CIMException{
    }
    public void setQualifierType(CIMObjectPath path,
		   CIMQualifierType qt) throws CIMException{
    }
    public void createClass(CIMObjectPath path,
	      CIMClass cc) throws CIMException {
    }
    public void setClass(CIMObjectPath path,
	   CIMClass cc) throws CIMException {
    }
*/   
    public CIMObjectPath createInstance(CIMObjectPath path,
          CIMInstance ci) throws CIMException {
       return new CIMObjectPath(_createInstance(cInst,path.cInst,ci.cInst));      
    }
    public void setInstance(CIMObjectPath path,
          CIMInstance ci) throws CIMException {
       _setInstance(cInst,path.cInst,ci.cInst);
    }
    public CIMValue getProperty(CIMObjectPath path,
          String propertyName) throws CIMException {
       return new CIMValue(_getProperty(cInst,path.cInst,propertyName));      
    }
    public void setProperty(CIMObjectPath path,
	       String propertyName,
	       CIMValue newValue) throws CIMException {
       _setProperty(cInst,path.cInst,propertyName,newValue.cInst);
    }
   
    public Enumeration execQuery(CIMObjectPath path,
				 String query,String ql) throws CIMException {
       return new InstEnumeration(_execQuery(cInst,path.cInst,query,ql));
    }
    
    public Enumeration associatorNames(CIMObjectPath path,
               String assocClass,
					String resultClass,
					String role,
               String resultRole) 
	       throws CIMException{
       return new InstEnumeration(_associatorNames(cInst,path.cInst,
            assocClass,resultClass,role,resultRole));
    }
    public Enumeration associators(CIMObjectPath path,
					String assocClass,
					String resultClass,
					String role,
					String resultRole,
					boolean includeQualifiers,
					boolean includeClassOrigin,
					String propertyList[]) 
          throws CIMException {
       return new InstEnumeration(_associators(cInst,path.cInst,
            assocClass,resultClass,role,resultRole,
            includeQualifiers,includeClassOrigin,propertyList));
    }
    public Enumeration referenceNames(CIMObjectPath path,
					String resultClass,
					String role) 
	        throws CIMException{
        return new InstEnumeration(_referenceNames(cInst,path.cInst,
            resultClass,role));
    }
    public Enumeration references(CIMObjectPath path,
					String resultClass,
					String role,
					boolean includeQualifiers,
					boolean includeClassOrigin,
					String propertyList[]) 
		     throws CIMException {
        return new InstEnumeration(_references(cInst,path.cInst,
            resultClass,role,
            includeQualifiers,includeClassOrigin,propertyList));
    }
/*    
    public CIMNameSpace getNameSpace() {
    }
    public CIMFeatures getServerFeatures() throws CIMException
    {
    }
    public void authenticate(String userName,String password) throws CIMException {
    }
    */
    static {
       System.loadLibrary("JMPIProviderManager");
    }
}
