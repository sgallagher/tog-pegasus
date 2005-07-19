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
// Modified By:  Adrian Duta
//
//%/////////////////////////////////////////////////////////////////////////////


package org.pegasus.jmpi;

import java.util.*;
import java.math.BigInteger;

public class CIMValue
{
   int cInst;

   private native int _byte(short v, boolean unsigned);
   private native int _short(int v, boolean unsigned);
   private native int makeInt(long v, boolean unsigned);
   private native int _long(long v, boolean unsigned);
   private native int _string(String v);
   private native int _boolean(boolean v);
   private native int _float(float v);
   private native int _double(double v);
   private native int _ref(int v);
   private native int _datetime(int v);

   private native int _byteArray(short v[], boolean unsigned);
   private native int _shortArray(int v[], boolean unsigned);
   private native int _intArray(long v[], boolean unsigned);
   private native int _longArray(long v[], boolean unsigned);
   private native int _stringArray(String v[]);
   private native int _booleanArray(boolean v[]);
   private native int _floatArray(float v[]);
   private native int _doubleArray(double v[]);
   private native int _refArray(int v[]);

   private native Object _getValue(int v) throws CIMException;
   private native int _getType(int v);
   private native boolean _isArray(int v);
   private native String _toString(int v);

   private native void _finalize(int cv);

   protected void finalize() {
     _finalize(cInst);
   }

   // This constructor should be public as well.
   public CIMValue(int v) {
      cInst=v;
   }

   public CIMValue(Object o) {

     cInst=-1;

      if (o instanceof Vector) {
         Vector v=(Vector)o;
         Object o0=v.elementAt(0);
	 int size=v.size();
	 if (o0 instanceof Number) {
            if (o0 instanceof UnsignedInt8) {
               short[] u8=new short[size];
               for (int i=0; i<size; i++)
                  u8[i]=((UnsignedInt8)v.elementAt(i)).shortValue();
               cInst=_byteArray(u8,true);
	    }
            else if (o0 instanceof Byte) {
               short[] s8=new short[size];
               for (int i=0; i<size; i++)
                  s8[i]=((Byte)v.elementAt(i)).shortValue();
               cInst=_byteArray(s8,false);
            }
            else if (o0 instanceof UnsignedInt16) {
               int[] u16=new int[size];
               for (int i=0; i<size; i++)
                  u16[i]=((UnsignedInt16)v.elementAt(i)).intValue();
               cInst=_shortArray(u16,true);
	    }
            else if (o0 instanceof Short) {
               int[] s16=new int[size];
               for (int i=0; i<size; i++)
                  s16[i]=((Short)v.elementAt(i)).intValue();
               cInst=_shortArray(s16,false);
	    }
            else if (o0 instanceof UnsignedInt32) {
               long[] u32=new long[size];
               for (int i=0; i<size; i++)
                  u32[i]=((UnsignedInt32)v.elementAt(i)).longValue();
               cInst=_intArray(u32,true);
	    }
            else if (o0 instanceof Integer) {
               long[] s32=new long[size];
               for (int i=0; i<size; i++)
                  s32[i]=((Integer)v.elementAt(i)).longValue();
               cInst=_intArray(s32,false);
	    }
            else if (o0 instanceof UnsignedInt64) {
               long u64[]=new long[size];
               for (int i=0; i<size; i++)
                  u64[i]=((UnsignedInt64)v.elementAt(i)).longValue();
               cInst=_longArray(u64,true);
	    }
            else if (o0 instanceof Long) {
               long s64[]=new long[size];
               for (int i=0; i<size; i++)
                  s64[i]=((Long)v.elementAt(i)).longValue();
               cInst=_longArray(s64,false);
	    }
            else if (o0 instanceof Float) {
               float f[]=new float[size];
               for (int i=0; i<size; i++)
                  f[i]=((Float)v.elementAt(i)).floatValue();
               cInst=_floatArray(f);
	    }
            else if (o0 instanceof Double) {
               double d[]=new double[size];
               for (int i=0; i<size; i++)
                  d[i]=((Double)v.elementAt(i)).doubleValue();
               cInst=_doubleArray(d);
	    }
	 }

         else if (o0 instanceof Boolean) {
            boolean b[]=new boolean[size];
            for (int i=0; i<size; i++)
            b[i]=((Boolean)v.elementAt(i)).booleanValue();
            cInst=_booleanArray(b);
 	  }

          else if (o0 instanceof String) {
             String s[]=new String[size];
	     v.copyInto(s);
             cInst=_stringArray(s);
	  }

          else if (o0 instanceof CIMObjectPath) {
             int c[]=new int[size];
             for (int i=0; i<size; i++)
                c[i]=((CIMObjectPath)v.elementAt(i)).cInst;
              cInst=_refArray(c);
          }
      }

      else if (o instanceof Number) {
          if (o instanceof UnsignedInt8)
             cInst=_byte(((UnsignedInt8)o).shortValue(),true);
          else if (o instanceof Byte)
             cInst=_byte(((Byte)o).byteValue(),false);
          else if (o instanceof UnsignedInt16)
             cInst=_short(((UnsignedInt16)o).intValue(),true);
          else if (o instanceof Short)
             cInst=_short(((Short)o).shortValue(),false);
          else if (o instanceof UnsignedInt32)
             cInst=makeInt(((UnsignedInt32)o).longValue(),true);
          else if (o instanceof Integer)
             cInst=makeInt(((Integer)o).intValue(),false);
          else if (o instanceof UnsignedInt64)
             cInst=_long(((UnsignedInt64)o).longValue(),true);
          else if (o instanceof Long)
             cInst=_long(((Long)o).longValue(),false);
          else if (o instanceof Float)
             cInst=_float(((Float)o).floatValue());
          else if (o instanceof Double)
             cInst=_double(((Double)o).doubleValue());
      }

      else if (o instanceof Boolean)
         cInst=_boolean(((Boolean)o).booleanValue());

      else if (o instanceof String)
         cInst=_string((String)o);

      else if (o instanceof CIMObjectPath)
         cInst=_ref(((CIMObjectPath)o).cInst);

      else if (o instanceof CIMDateTime)
         cInst=_datetime(((CIMDateTime)o).cInst);

      if (cInst==-1)
         System.err.println("+++ CIMValue(): unsupported type: "+o.getClass());
   }

   public CIMValue(Object val, CIMDataType type) throws Exception {
      CIMValue nv=new CIMValue(val);
      cInst=nv.cInst;
      nv.cInst=0;
   }

   public Object getValue() throws CIMException {
      return getValue(true);
   }

   public Object getValue(boolean toVector) throws CIMException {
      Object resp=null;
      Object o=null;

      try {
         o=_getValue(cInst);
         if (o==null)
            return null;

         int type=_getType(cInst);
         boolean isArray=_isArray(cInst);
         if (isArray) switch (type) {
            case CIMDataType.BOOLEAN:
               Boolean b[]=(Boolean[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<b.length;i++)
                     ((Vector)resp).addElement(b[i]);
               }
               else resp=b;
               break;
            case CIMDataType.SINT8:
               Byte s8[]=(Byte[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<s8.length;i++)
                     ((Vector)resp).addElement(s8[i]);
               }
               else resp=s8;
               break;
            case CIMDataType.UINT8:
               UnsignedInt8 u8[]=(UnsignedInt8[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<u8.length;i++)
                     ((Vector)resp).addElement(u8[i]);
               }
               else resp=u8;
               break;
            case CIMDataType.SINT16:
               Short s16[]=(Short[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<s16.length;i++)
                     ((Vector)resp).addElement(s16[i]);
               }
               else resp=s16;
               break;
            case CIMDataType.UINT16:
               UnsignedInt16 u16[]=(UnsignedInt16[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<u16.length;i++)
                     ((Vector)resp).addElement(u16[i]);
               }
               else resp=u16;
               break;
            case CIMDataType.SINT32:
               Integer s32[]=(Integer[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<s32.length;i++)
                     ((Vector)resp).addElement(s32[i]);
               }
               else resp=s32;
               break;
            case CIMDataType.UINT32:
               UnsignedInt32 u32[]=(UnsignedInt32[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<u32.length;i++)
                     ((Vector)resp).addElement(u32[i]);
               }
               else resp=u32;
               break;
            case CIMDataType.SINT64:
               Long s64[]=(Long[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<s64.length;i++)
                     ((Vector)resp).addElement(s64[i]);
               }
               else resp=s64;
               break;
            case CIMDataType.UINT64:
               UnsignedInt64 u64[]=(UnsignedInt64[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<u64.length;i++)
                     ((Vector)resp).addElement(u64[i]);
               }
               else resp=u64;
               break;
            case CIMDataType.STRING:
               String s[]=(String[])o;
               if (toVector) {
                  resp=new Vector();
		  for (int i=0;i<s.length;i++)
                     ((Vector)resp).addElement(s[i]);
               }
               else resp=s;
               break;
            case CIMDataType.REAL32:
               Float f[]=(Float[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<f.length;i++)
                     ((Vector)resp).addElement(f[i]);
               }
               else resp=f;
               break;
            case CIMDataType.REAL64:
               Double d[]=(Double[])o;
               if (toVector) {
                  resp=new Vector();
                  for (int i=0;i<d.length;i++)
                     ((Vector)resp).addElement(d[i]);
               }
               else resp=d;
               break;
            case CIMDataType.REFERENCE:
         }
         else resp=o;
      }
      catch (CIMException ce) {
         throw ce;
      }
      catch (Exception e) {
         //e.printStackTrace();
         throw new CIMException(1,e.getMessage());
      }
      return resp;
   }


   public boolean isArray() {
      return _isArray(cInst);
   }

   public String toString() {
      return _toString(cInst);
   }

   public int cInst(){
      return cInst;
   }
   
   static {
      System.loadLibrary("JMPIProviderManager");
   }

};





