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
// Author:      Adrian Duta
//
// Modified By: Adrian Schuur, schuur@de.ibm.com 
//
//%/////////////////////////////////////////////////////////////////////////////

package org.pegasus.jmpi;

public class CIMQualifier 
{
   int cInst;
   private native int _new(String n);
   private native String _getName(int cq);
   private native Object _getValue(int cq);
   private native void   _setValue(int cq, int v);
   private native void   _finalize(int cq);

   protected void finalize() {
      _finalize(cInst);
   }
 

   CIMQualifier(int ci) {
      cInst=ci;
   }
   
   public CIMQualifier(String iname) {
      cInst=_new(iname);
   }

   public CIMValue getValue() {
      return new CIMValue(_getValue(cInst));
   }

   public void setValue(CIMValue value) {
      _setValue(cInst,value.cInst);
   }

   public String getName() {
      return _getName(cInst);
   }
}
