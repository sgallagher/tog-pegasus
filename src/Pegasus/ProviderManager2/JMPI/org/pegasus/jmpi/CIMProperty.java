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
// Modified By: Adrian Duta
//
//%/////////////////////////////////////////////////////////////////////////////


package org.pegasus.jmpi;

public class CIMProperty
{
   int cInst;
   private native int     _getValue(int p);
   private native String  _getName(int v);
   private native void    _setName(int v,String n);
   private native int     _property(String name,int v);
   private native int     _new();
   private native boolean _isReference(int v);
   private native String  _getRefClassName(int v);
   private native int     _getType(int v);
   private native int     _setType(int v, int t);
   private native void    _setValue(int p,int v);
   private native boolean _isArray(int p);
   private native String  _getIdentifier(int p);
   private native void    _addValue(int p,int v);
   private native void    _addQualifier(int p,int v);
   private native void    _finalize(int cp);

   protected void finalize() {
      _finalize(cInst);
   }

   CIMProperty(int ci) {
      cInst=ci;
   }

   int cInst() {
      return cInst;
   }

   public CIMProperty() {
      cInst=_new();
   }

   public CIMProperty(String name, CIMValue cv) {
      cInst=_property(name,cv.cInst);
   }

   public CIMValue getValue() {
      return new CIMValue(_getValue(cInst));
   }

   public String getName() {
      return _getName(cInst);
   }

   public void setName(String n) {
       _setName(cInst,n);
   }

   public boolean isReference() {
      return _isReference(cInst);
   }

   public CIMDataType getType() {
      return new CIMDataType(_getType(cInst),true);
   }

   public void setType(CIMDataType dt) {
       cInst=_setType(cInst,dt.cInst);
   }

   public String getRefClassName() {
      return _getRefClassName(cInst);
   }

   public String toString() {
      return getType().toString()+" "+getName()+"="+getValue().toString()+";";
   }

  public void setValue(CIMValue v) {
     _setValue(cInst,v.cInst);
  }
  
  public void addValue(CIMValue v) {
     if (!_isArray(cInst))
        return;
     _addValue(cInst,v.cInst);
  }

  public void addQualifier(CIMQualifier q) {
     _addQualifier(cInst,q.cInst);
  }

  public boolean isArray() {
     return _isArray(cInst);
  }
  
  public String getIdentifier() {//to be implemented
     return _getIdentifier(cInst);
  }
  
   static {
      System.loadLibrary("JMPIProviderManager");
   }
}




