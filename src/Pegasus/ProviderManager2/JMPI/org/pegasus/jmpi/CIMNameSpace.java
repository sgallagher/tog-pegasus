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


public class CIMNameSpace {

   static public final int DEFAULT_PORT=5988;
   static public final String DEFAULT_NAMESPACE="root/cimv2";

   int cInst;

   private native int    _new          ();
   private native int    _newHn        (String hn);
   private native int    _newHnNs      (String hn,
                                        String ns);
   private native String _getNameSpace (int    cInst);
   private native String _getHost      (int    cInst);
   private native void   _setNameSpace (int    cInst,
                                        String ns);
   private native void   _setHost      (int    cInst,
                                        String h);
   private native void   _finalize     (int    cInst);

   public CIMNameSpace ()
   {
      cInst=_new();
   }

   protected void finalize ()
   {
      _finalize(cInst);
   }

   public CIMNameSpace (String host)
   {
      cInst=_newHn(host);
   }

   public CIMNameSpace (String host,
                        String ns)
   {
      cInst=_newHnNs(host,ns);
   }

   public String getNameSpace ()
   {
      return _getNameSpace(cInst);
   }

   public String getHost ()
   {
      return _getHost(cInst);
   }

   public void setNameSpace (String ns)
   {
      _setNameSpace(cInst,ns);
   }

   public void setHost (String host)
   {
      _setHost(cInst,host);
   }

   public int getPortNumber ()
   {
      return 0;
   }

   public String getProtocol ()
   {
      return null;
   }

   public String getHostURL ()
   {
      return null;
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
}
