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

   CIMValue(int v) {
      cInst=v;
   }
   public CIMValue(Object o) {

      cInst=-1;
      if (o instanceof Number) {
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

      else if (o instanceof Byte[]) {
         Byte S8[]=(Byte[])o;
         short s8[]=new short[S8.length];

         for (int i=0;i<S8.length;i++)
            s8[i]=S8[i].byteValue();
         cInst=_byteArray(s8,false);
      }

      else if (o instanceof UnsignedInt8[]) {
         UnsignedInt8[] U8=(UnsignedInt8[])o;
         short u8[]=new short[U8.length];

         for (int i=0;i<U8.length;i++)
            u8[i]=U8[i].shortValue();
         cInst=_byteArray(u8,true);
      } 

      else if (o instanceof Short[]) {   
         Short S16[]=(Short[])o;
         int s16[]=new int[S16.length];

         for (int i=0;i<S16.length;i++)
            s16[i]=S16[i].shortValue();
         cInst=_shortArray(s16,false);
      }

      else if (o instanceof UnsignedInt16[]) {
         UnsignedInt16 U16[]=(UnsignedInt16[])o;
         int u16[]=new int[U16.length];

         for (int i=0;i<U16.length;i++)
            u16[i]=U16[i].intValue();
         cInst=_shortArray(u16,true);
      }

      else if (o instanceof Integer[]) {   
         Integer S32[]=(Integer[])o;
         long s32[]=new long[S32.length];

         for (int i=0;i<S32.length;i++)
            s32[i]=S32[i].intValue();
         cInst=_intArray(s32,false);
      }

      else if (o instanceof UnsignedInt32[]) {
         UnsignedInt32 U32[]=(UnsignedInt32[])o;
         long u32[]=new long[U32.length];
         for (int i=0;i<U32.length;i++)
            u32[i]=U32[i].longValue();
         cInst=_intArray(u32,true);
      }

      else if (o instanceof Long[]) {
         Long S64[]=(Long[])o;
         long s64[]=new long[S64.length];

         for (int i=0;i<S64.length;i++)
            s64[i]=S64[i].longValue();
         cInst=_longArray(s64,false);
      }

      else if (o instanceof UnsignedInt64[]) {
         UnsignedInt64 U64[]=(UnsignedInt64[])o;
         long u64[]=new long[U64.length];

         for (int i=0;i<U64.length;i++)
            u64[i]=U64[i].longValue();
         cInst=_longArray(u64,true);
      }

      else if (o instanceof Float[]) {
         Float F[]=(Float[])o;
         float f[]=new float[F.length];

         for (int i=0;i<F.length;i++)
            f[i]=F[i].floatValue();
         cInst=_floatArray(f);
      }

      else if (o instanceof Double[]) {
         Double D[]=(Double[])o;
         double d[]=new double[D.length];

         for (int i=0;i<D.length;i++)
            d[i]=D[i].doubleValue();
         cInst=_doubleArray(d);
      }

      else if (o instanceof Boolean)
         cInst=_boolean(((Boolean)o).booleanValue());                 

      else if (o instanceof Boolean[]) {
         Boolean B[]=(Boolean[])o;
         boolean b[]=new boolean[B.length];

         for (int i=0;i<B.length;i++)
            b[i]=B[i].booleanValue();
         cInst=_booleanArray(b);
      }

      else if (o instanceof String)
         cInst=_string((String)o);

      else if (o instanceof String[])
         cInst=_stringArray((String[])o);

      else if (o instanceof CIMObjectPath)
         cInst=_ref(((CIMObjectPath)o).cInst); 

      else if (o instanceof CIMObjectPath[]) {
         CIMObjectPath C[]=(CIMObjectPath[])o;
         int c[]=new int[C.length];

         for (int i=0;i<C.length;i++)
            c[i]=C[i].cInst;
         cInst=_refArray(c);
      }
      else if (o instanceof CIMDateTime)
         cInst=_datetime(((CIMDateTime)o).cInst); 

      if (cInst==-1)
         System.err.println("+++ CIMValue(): unsupported type: "+o.getClass());
   }

   public CIMValue(String str, int type, boolean isArray) throws Exception { 

      //System.out.println("value: "+str+" , type: "+CIMDataType.toStr[type]+" , isArray: "+isArray);

      cInst=-1;
      if (isArray) {    
         StringTokenizer st = new StringTokenizer(str, ",");
         Vector v = new Vector(st.countTokens());
         while (st.hasMoreTokens())
            v.addElement(st.nextToken());
         int len=v.size();
         switch (type) {
         case CIMDataType.UINT8:
            short u8[]=new short[len];
            for (int i=0;i<len;i++)
               u8[i]=new UnsignedInt8((String)v.elementAt(i)).shortValue();
            cInst=_byteArray(u8,true);
            break;
         case CIMDataType.SINT8:
            short s8[]=new short[len];
            for (int i=0;i<len;i++)
               s8[i]=new Byte((String)v.elementAt(i)).byteValue();
            cInst=_byteArray(s8,false);
            break;
         case CIMDataType.UINT16:
            int u16[]=new int[len];
            for (int i=0;i<len;i++)
               u16[i]=new UnsignedInt16((String)v.elementAt(i)).intValue();
            cInst=_shortArray(u16,true);
            break;
         case CIMDataType.SINT16:
            int s16[]=new int[len];
            for (int i=0;i<len;i++)
               s16[i]=new Short((String)v.elementAt(i)).shortValue();
            cInst=_shortArray(s16,false);
            break;
         case CIMDataType.UINT32:
            long u32[]=new long[len];
            for (int i=0;i<len;i++)
               u32[i]=new UnsignedInt32((String)v.elementAt(i)).longValue();
            cInst=_intArray(u32,true);
            break;
         case CIMDataType.SINT32:
            long s32[]=new long[len];
            for (int i=0;i<len;i++)
               s32[i]=new Integer((String)v.elementAt(i)).intValue();
            cInst=_intArray(s32,false);
            break;
         case CIMDataType.UINT64:
            long u64[]=new long[len];
            for (int i=0;i<len;i++)
               u64[i]=new UnsignedInt64((String)v.elementAt(i)).longValue();
            cInst=_longArray(u64,true);
            break;
         case CIMDataType.SINT64:
            long s64[]=new long[len];
            for (int i=0;i<len;i++)
               s64[i]=new Long((String)v.elementAt(i)).longValue();
            cInst=_longArray(s64,false);
            break;
         case CIMDataType.STRING:
            String s[]=new String[len];
            for (int i=0;i<len;i++)
               s[i]=(String)v.elementAt(i);
            cInst=_stringArray(s);         
            break;
         case CIMDataType.BOOLEAN:
            boolean b[]=new boolean[len];
            for (int i=0;i<len;i++)
               b[i]=new Boolean((String)v.elementAt(i)).booleanValue();
            cInst=_booleanArray(b);
            break;
         case CIMDataType.REAL32:
            float f[]=new float[len];
            for (int i=0;i<len;i++)
               f[i]=new Float((String)v.elementAt(i)).floatValue();
            cInst=_floatArray(f);
            break;
         case CIMDataType.REAL64:
            double d[]=new double[len];
            for (int i=0;i<len;i++)
               d[i]=new Double((String)v.elementAt(i)).doubleValue();
            cInst=_doubleArray(d);
            break;
         case CIMDataType.REFERENCE:
            int r[]=new int[len];
            for (int i=0;i<len;i++)
               r[i]=new CIMObjectPath((String)v.elementAt(i)).cInst;
            cInst=_refArray(r);
         }
      }
      
      else {
         switch (type) {
         case CIMDataType.UINT8:
            cInst=_byte(new UnsignedInt8(str).shortValue(),true);
            break;
         case CIMDataType.SINT8:
            cInst=_byte(new Byte(str).byteValue(),false);
            break;
         case CIMDataType.UINT16:
            cInst=_short(new UnsignedInt16(str).intValue(),true);
            break;
         case CIMDataType.SINT16:
            cInst=_short(new Short(str).shortValue(),false);
            break;
         case CIMDataType.UINT32:
            cInst=makeInt(new UnsignedInt32(str).longValue(),true);
            break;
         case CIMDataType.SINT32:
            cInst=makeInt(new Integer(str).intValue(),false);
            break;
         case CIMDataType.UINT64:
            cInst=_long(new UnsignedInt64(str).longValue(),true);
            break;
         case CIMDataType.SINT64:
            cInst=_long(new Long(str).longValue(),false);
            break;
         case CIMDataType.STRING:
            cInst=_string(str);
            break;
         case CIMDataType.BOOLEAN:
            cInst=_boolean(new Boolean(str).booleanValue());
            break;
         case CIMDataType.REAL32:
            cInst=_float(new Float(str).floatValue());
            break;
         case CIMDataType.REAL64:
            cInst=_double(new Double(str).doubleValue());
            break;
         case CIMDataType.REFERENCE:
            cInst=_ref(new CIMObjectPath(str).cInst);
         }        
      }
      
      if (cInst==-1)
         throw new Exception("+++ unsupported type: "+type+" , isArray: "+isArray);            
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





