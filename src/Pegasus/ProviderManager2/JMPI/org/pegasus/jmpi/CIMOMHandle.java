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
// Modified By: Magda
//
//%/////////////////////////////////////////////////////////////////////////////


package org.pegasus.jmpi;
import java.util.*;

public class CIMOMHandle
{
   int cInst;
   String name;
   private native int _getClass(int inst, int cop, boolean localOnly)
      throws CIMException;
   private native void _enumClass(int inst, int cop, boolean localOnly, Vector vec)
      throws CIMException;
   private native int _getProperty(int inst, int cop, String n)
      throws CIMException;
   private native int _deleteInstance(int cInst, int cop)
      throws CIMException;
   private native void _enumInstances(int cInst, int cop, boolean deep, boolean localOnly, Vector vec)
      throws CIMException;
   private native int _getInstance(int cInst, int cop, boolean localOnly)
      throws CIMException;
   private native void _deliverEvent(int cInst, String nm, String ns, int ind)
      throws CIMException;
   private native void _finalize(int ch);

   protected void finalize() {
      //   _finalize(cInst);
   }

   CIMOMHandle(int ci, String nm) {
      cInst=ci;
      name=nm;
   }

   public CIMClass getClass(CIMObjectPath name, boolean localOnly)
         throws CIMException {
      return new CIMClass(_getClass(cInst,name.cInst,localOnly));
   }

   public Enumeration enumClass(CIMObjectPath name, boolean localOnly)
         throws CIMException {
      Vector vec=new Vector();
      _enumClass(cInst,name.cInst,localOnly,vec);
      return vec.elements();
   }

   public CIMValue getProperty(CIMObjectPath cop,String n)
         throws CIMException {
      int val = _getProperty(cInst, cop.cInst(), n);
      if (val != -1)
         return new CIMValue(val);
      return null;
   }


   public void deleteInstance(CIMObjectPath cop)
          throws CIMException {
      _deleteInstance(cInst, cop.cInst());
   }


   public Enumeration enumInstances(CIMObjectPath cop,boolean deep,boolean localOnly)
         throws CIMException {
      Vector vec = new Vector();
      _enumInstances(cInst, cop.cInst(), deep, localOnly, vec);
      return vec.elements();
   }


   public CIMInstance getInstance(CIMObjectPath cop,boolean localOnly)
         throws CIMException { //to be implemented
      int inst=_getInstance(cInst, cop.cInst, localOnly);
      if (inst !=-1)
         return new CIMInstance(inst);
      return null;
   }

   public void deliverEvent(String ns, CIMInstance ind)
        throws CIMException {
      _deliverEvent(cInst,name,ns,ind.cInst);
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
};



