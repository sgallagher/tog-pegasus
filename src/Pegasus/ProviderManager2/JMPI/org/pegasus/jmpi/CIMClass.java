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
// Modified By: Adrian Duta
//              Magda
//              Andy
//
//%/////////////////////////////////////////////////////////////////////////////


package org.pegasus.jmpi;

import java.util.Vector;

/**
    Creates and instantiates a CIM class.  In CIM, a class object may
    be a class or an associator.  A CIM class must contain a name and
    may contain methods, properties, and qualifiers.  It is a template
    for creating a CIM instance.  A CIM class represents a collection
    of CIM instances, all of which support a common type (for example,
    a set of properties, methods, and associations).
 */

public class CIMClass
{
   int cInst;
   private native int     _newInstance(int cInst);
   private native String  _getName(int cInst);
   private native int     _getQualifier(int cInst, String n);
   private native boolean _hasQualifier(int cInst, String n);
   private native int     _getProperty(int cInst, String n);
   private native Vector  _getProperties(int cInst,Vector vec);
   private native int     _new(String n);
   private native String  _getSuperClass(int cInst);
   private native Vector  _getKeys(int cInst, Vector vec);
   private native int     _getMethod(int cInst, String n);
   private native boolean _equals(int cInst, int cInst1);
   private native void    _finalize(int cc);

   protected void finalize() {
      _finalize(cInst);
   }

   CIMClass(int ci) {
      cInst=ci;
   }
   
   CIMClass(String n) {
      cInst = _new(n);
   }

   int cInst() {
      return cInst;
   }
   
    /**
       Returns a new instance appropriately initialized
     */
   public CIMInstance newInstance() {
      return new CIMInstance(_newInstance(cInst));
   }
   
    /**
      getName - returns the name of this class

      @return String with class name.
    */
   public String getName() {
      return _getName(cInst);
   }
   
    /**
       getQualifier - get the specified CIM Qualifier in this class

       @param String name - The string name of the CIM qualifier.

       @return CIMQualifier Returns the CIM Qualifier in this class else null
     */
   public CIMQualifier getQualifier(String n) {
      int qInst=_getQualifier(cInst,n);
      if (qInst!=-1)
         return new CIMQualifier(qInst);
      return null;
   }

    /**
       Checks whether this class has the specified qualifier

       Returns true if qualifier defined
     */
   public boolean hasQualifier(String n) {
      return _hasQualifier(cInst,n);
   }

    /**
       Gets the properties for this CIM class
     */
   public Vector getProperties() {
      return _getProperties(cInst,new Vector());
   }
   
    /**
       getProperty - get the CIMProperty for the specified class

       @param String name - name of the property to get.

       @return CIMProperty return the CIM property else if exist else Null
			 if the property does not exist
    */
   public CIMProperty getProperty(String n) {
   int qInst=_getProperty(cInst,n);
   if (qInst!=-1)
      return new CIMProperty(qInst);
         return null;
   }

    /**
      getSuperClass - returns the name of the parent for this class

      @return String with parent class name.
    */
   public String getSuperClass() {//to be implemented
      return _getSuperClass(cInst); 
   }

    /**
         getKeys - Returns the properties that are keys for this class

         @return  Vector containing the list of key properties
     */
   public Vector getKeys() {//to be implemented
      return _getKeys(cInst, new Vector());
   }

    /**
       getMethod  - // Returns the specified method
     */
   public CIMMethod getMethod(String n) {//to be implemented
      return new CIMMethod(_getMethod(cInst, n));
   }

    /**
       Returns a String representation of the CIMClass.

       @return String empty or cimclass string
    */
   public String toString() {
      return "@ CIMClass.toString() not implemented yet!";
   }

   public boolean equals(Object o) {
      if (!(o instanceof CIMClass))
         return(false);
      CIMClass clsToBeCompared=(CIMClass)o;
      if (cInst < 1 || clsToBeCompared.cInst < 1) {
         System.out.println("wrong cInst found!");
         return(false);
      }
      boolean rv = _equals(cInst, clsToBeCompared.cInst);
      return rv;
   }

   static {
       System.loadLibrary("JMPIProviderManager");
   }
}
