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

public class UnsignedNumber  extends Number {

    private long number;
    private long mask;

    public UnsignedNumber(long val, long mask, String msg)
                throws java.lang.NumberFormatException {
        number=val;
        if ((number&~mask)!=0)
            throw new java.lang.NumberFormatException(msg);
	this.mask=mask;
    }

    public UnsignedNumber(java.lang.String str, long mask, String msg)
                throws java.lang.NumberFormatException {
        Long l=new Long(str);
        number=l.longValue();
        if ((number&~mask)!=0)
            throw new java.lang.NumberFormatException(msg);
	this.mask=mask;
    }

    public String toString() {
        return ""+(number & mask);
    }

    public int hashCode(){
        return (int)(number & mask);
    }

    public boolean equals(java.lang.Object o){
        if (o instanceof UnsignedNumber)
        return (((UnsignedNumber)o).number & ((UnsignedNumber)o).mask)==(number & mask);
        return false;
    }

    public int intValue() {
        return (int)(number & mask);
    }

    public long longValue() {
        return (long)(number & mask);
    }

    public float floatValue() {
        return (float)(number & mask);
    }

    public double doubleValue() {
        return (double)(number & mask);
    }

    public byte byteValue() {
        return (byte)(number & mask);
    }

    public short shortValue() {
        return (short) (number & mask);
    }
}

