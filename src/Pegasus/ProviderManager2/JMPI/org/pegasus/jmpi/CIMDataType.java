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

/**
    Encapsulates the CIM data types (as defined in the CIM Specification).
    This class includes methods that perform operations on CIM data types,
    such as returning the CIM data type of a CIM object, returning an
    array of CIM data types for an array of CIM objects, and creating an
    object or array of objects of the specified CIM data type.  All CIM
    properties must have a valid CIM data type.

 */
public class CIMDataType
{
   public static final int  SIZE_SINGLE    = 0;
   public static final int  SIZE_UNLIMITED = 1;

   private final static int ARRAY          = 0x10;
   private final static int OTHER          = 0x20;

   public static final int  INVALID        = 0;
   public static final int  UINT8          = 1;
   public static final int  SINT8          = 2;
   public static final int  UINT16         = 3;
   public static final int  SINT16         = 4;
   public static final int  UINT32         = 5;
   public static final int  SINT32         = 6;
   public static final int  UINT64         = 7;
   public static final int  SINT64         = 8;
   public static final int  STRING         = 9;
   public static final int  BOOLEAN        = 10;
   public static final int  REAL32         = 11;
   public static final int  REAL64         = 12;
   public static final int  DATETIME       = 13;
   public static final int  CHAR16         = 14;

   public static final int  UINT8_ARRAY    = ARRAY + UINT8;
   public static final int  SINT8_ARRAY    = ARRAY + SINT8;
   public static final int  UINT16_ARRAY   = ARRAY + UINT16;
   public static final int  SINT16_ARRAY   = ARRAY + SINT16;
   public static final int  UINT32_ARRAY   = ARRAY + UINT32;
   public static final int  SINT32_ARRAY   = ARRAY + SINT32;
   public static final int  UINT64_ARRAY   = ARRAY + UINT64;
   public static final int  SINT64_ARRAY   = ARRAY + SINT64;
   public static final int  STRING_ARRAY   = ARRAY + STRING;
   public static final int  BOOLEAN_ARRAY  = ARRAY + BOOLEAN;
   public static final int  REAL32_ARRAY   = ARRAY + REAL32;
   public static final int  REAL64_ARRAY   = ARRAY + REAL64;
   public static final int  DATETIME_ARRAY = ARRAY + DATETIME;
   public static final int  CHAR16_ARRAY   = ARRAY + CHAR16;

   public static final int  REFERENCE      = OTHER + 1;

   int cInst;

   private native int     _new             (int    type);
   private native int     _newAr           (int    type,
                                            int    size);
   private native int     _newRef          (int    type,
                                            String ref);
   private native boolean _isReference     (int    ci);
   private native boolean _isArray         (int    ci);
   private native int     _getType         (int    ci);
   private native int     _getSize         (int    ci);
   private native String  _getRefClassName (int    ci);
   private native String  _toString        (int    ci);
   private native void    _finalize        (int    cInst);

   protected void finalize ()
   {
      _finalize(cInst);
   }

   CIMDataType (int     ci,
                boolean dmy)
   {
      cInst=ci;
   }

   public CIMDataType (int type)
   {
      if (type>INVALID && type<=CHAR16)
         cInst=_new(type);
      else if (type>=UINT8_ARRAY && type<=CHAR16_ARRAY)
         cInst=_newAr(type,0);
      else if (type==REFERENCE)
         cInst=_newRef(REFERENCE,"");
      else
         cInst=_new(INVALID);
   }

   public CIMDataType (int type,
                       int size)
   {
      if (type>=UINT8_ARRAY && type<=CHAR16_ARRAY)
         cInst=_newAr(type,size);
      else
         cInst=_new(INVALID);
   }

   public CIMDataType (String ref)
   {
      cInst=_newRef(REFERENCE,ref);
   }

   public boolean isArrayType ()
   {
      return _isArray(cInst);
   }
   public boolean isReferenceType ()
   {
      return _isReference(cInst);
   }
   public int getType ()
   {
      return _getType(cInst);
   }
   public int getSize ()
   {
      return _getSize(cInst);
   }
   public String getRefClassName ()
   {
      return _getRefClassName(cInst);
   }
   public String toString ()
   {
      return _toString(cInst);
   }

   public static CIMDataType getPredefinedType (int type)
   {
      return new CIMDataType(type);
   }

   static {
      System.loadLibrary("JMPIProviderManager");
   }
};
