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


package org.pegasus.jmpi;

public class CIMArgument
{
   int cInst;
   private native int     _new();
   private native int     _newS(String name);
   private native int     _newSV(String name,int v);
   private native int     _getType(int v);
   private native int     _setType(int v, int t);
   private native void    _setValue(int p,int v);
   private native int     _getValue(int p);
   private native String  _getName(int v);
   private native void    _setName(int v,String n);
   private native int     _getQualifier(int v,String n);
   private native CIMQualifier[]  _getQualifiers(int v);
   private native void    _setQualifiers(int v,CIMQualifier[] qa);
   private native void    _finalize(int cp);

   protected void finalize() {
      _finalize(cInst);
   }

   CIMArgument(int ci) {
      cInst=ci;
   }

   int cInst() {
      return cInst;
   }

   public CIMArgument() {
      cInst=_new();
   }

   public CIMArgument(String name) {
      cInst=_newS(name);
   }

   public CIMArgument(String name, CIMValue cv) {
      cInst=_newSV(name,cv.cInst);
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

   public CIMDataType getType() {
      return new CIMDataType(_getType(cInst),true);
   }

   public void setType(CIMDataType dt) {
       cInst=_setType(cInst,dt.cInst);
   }

   public String toString() {
      return getType().toString()+" "+getName()+"="+getValue().toString()+";";
   }

   public void setValue(CIMValue v) {
      _setValue(cInst,v.cInst);
   }

   public CIMQualifier getQualifier(String n) {
      return new CIMQualifier(_getQualifier(cInst,n));
   }

   public CIMQualifier[] getQualifiers() {
      return _getQualifiers(cInst);
   }

   public void setQualifiers(CIMQualifier[] qa) {
      _setQualifiers(cInst,qa);
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
}




