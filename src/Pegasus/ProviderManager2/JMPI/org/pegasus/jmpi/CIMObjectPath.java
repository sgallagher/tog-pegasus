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
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

import java.util.*;

public class CIMObjectPath
{
   private int cInst;

   private native int        _new           ();
   private native int        _newCn         (String cn);
   private native int        _newCnNs       (String cn,
                                             String ns);
   private native int        _newCi         (int    cInst);
   private native int        _newCiNs       (int    cInst,
                                             String ns);
   private native void       _finalize      (int    cInst);
   private native String     _getNameSpace  (int    cInst);
   private native void       _setNameSpace  (int    cInst,
                                             String ns);
   private native String     _getHost       (int    cInst);
   private native void       _setHost       (int    cInst,
                                             String hn);
   private native String     _getObjectName (int    cInst);
   private native void       _setObjectName (int    cInst,
                                             String objectName);
   private native Vector     _getKeys       (int    cInst,
                                             Vector v);
   private native void       _setKeys       (int    cInst,
                                             Vector v);
   private native void       _addKey        (int    cInst,
                                             String key,
                                             int    vInst);
   private native String     _getKeyValue   (int    cInst,
                                             String keyValue);
   private native String     _toString      (int    cInst);
   private native int        _clone         (int    cInst);
   private static native int _set           (String copStr);

   CIMObjectPath (int ci)
   {
      cInst = ci;
   }

   protected int cInst ()
   {
      return cInst;
   }

   protected void finalize ()
   {
      _finalize (cInst);
   }

   public CIMObjectPath ()
   {
      cInst = _new ();
   }

   public CIMObjectPath (String className)
   {
      if (className == null)
         cInst = _new ();
      else
         cInst = _newCn (className);
   }

   public CIMObjectPath (String className, String nameSpace)
   {
      if (nameSpace == null)
         cInst = _newCn (className);
      else
         cInst = _newCnNs (className, nameSpace);
  }

   public CIMObjectPath (String className, Vector keyValuePairs)
   {
      cInst = _newCn (className);
      if (keyValuePairs != null)
         _setKeys (cInst, keyValuePairs);
   }

   public CIMObjectPath (CIMInstance ci)
   {
      cInst = _newCi (ci.cInst ());
   }

   public CIMObjectPath (CIMInstance ci, String ns)
   {
      cInst = _newCiNs (ci.cInst (), ns);
   }

   public Object clone ()
   {
      int ciNew = _clone (cInst);

      if (ciNew != 0)
      {
         return new CIMObjectPath (ciNew);
      }
      else
      {
         return null;
      }
   }

   public String getHost ()
   {
      return _getHost (cInst);
   }

   public void setHost (String hn)
   {
      _setHost (cInst, hn);
   }

   public String getNameSpace ()
   {
      return _getNameSpace (cInst);
   }

   public void setNameSpace (String ns)
   {
      _setNameSpace (cInst, ns);
   }

   public String getObjectName ()
   {
      return _getObjectName (cInst);
   }

   public void setObjectName (String objectName)
   {
      _setObjectName (cInst, objectName);
   }

   public Vector getKeys ()
   {
      return _getKeys (cInst, new Vector ());
   }

   public String getKeyValue (String keyName)
   {
      return _getKeyValue (cInst, keyName);
   }

   public void setKeys (Vector keys)
   {
      _setKeys (cInst, keys);
   }

   public void addKey (String key, CIMValue val)
   {
      _addKey (cInst, key, val.cInst ());
   }

   public String toString ()
   {
      return _toString (cInst);
   }

   public static CIMObjectPath toCop (String copStr)
   {
      return new CIMObjectPath (_set (copStr));
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
}
