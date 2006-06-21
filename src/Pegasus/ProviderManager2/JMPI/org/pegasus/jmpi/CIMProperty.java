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
// Modified By: Adrian Duta
//              Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

public class CIMProperty
{
    public static final int PEG_NOT_FOUND = -1;

    private int cInst;

    private native int     _getValue        (int    ci);
    private native String  _getName         (int    ci);
    private native void    _setName         (int    ci,   String n);
    private native int     _property        (String name, int    v);
    private native int     _new             ();
    private native boolean _isReference     (int    ci);
    private native String  _getRefClassName (int    ci);
    private native int     _getType         (int    ci);
    private native int     _setType         (int    ci,   int    t);
    private native void    _setValue        (int    ci,   int    v);
    private native boolean _isArray         (int    ci);
    private native String  _getIdentifier   (int    ci);
    private native void    _addValue        (int    ci,   int    v);
    private native void    _addQualifier    (int    ci,   int    v);
    private native void    _finalize        (int    ci);
    private native int     _findQualifier   (int    ci,   String qualifier);
    private native int     _getQualifier    (int    ci,   int    index);

    protected void finalize ()
    {
       _finalize (cInst);
    }

    CIMProperty (int ci)
    {
       cInst = ci;
    }

    protected int cInst ()
    {
       return cInst;
    }

    public CIMProperty ()
    {
       cInst = _new ();
    }

    public CIMProperty (String name, CIMValue cv)
    {
        cInst = 0;

        if (cv.cInst () == 0)
       	    return;

        cInst = _property (name, cv.cInst ());
    }

    public CIMValue getValue ()
    {
        if (cInst == 0)
            return null;

        return new CIMValue (_getValue (cInst));
    }

    public String getName ()
    {
        if (cInst == 0)
            return null;

        return _getName (cInst);
    }

    public void setName (String n)
    {
        if (cInst == 0)
            return;

        _setName (cInst, n);
    }

    public boolean isReference ()
    {
        if (cInst == 0)
            return false;

        return _isReference (cInst);
    }

    public CIMDataType getType ()
    {
        if (cInst == 0)
            return null;

        return new CIMDataType (_getType (cInst), true);
    }

    public void setType (CIMDataType dt)
    {
        if (cInst == 0 || dt.cInst () == 0)
            return;

        cInst = _setType (cInst, dt.cInst ());
    }

    public String getRefClassName ()
    {
        if (cInst == 0)
            return null;

        return _getRefClassName (cInst);
    }

    public String toString ()
    {
        if (cInst == 0)
            return null;

        return getType().toString() + " " + getName () + "=" + getValue ().toString () + ";";
    }

    public void setValue (CIMValue v)
    {
        if (cInst == 0 || v.cInst () == 0)
            return;

        _setValue (cInst, v.cInst ());
    }

    public void addValue (CIMValue v)
    {
        if (cInst == 0 || v.cInst () == 0)
            return;

        if (!_isArray (cInst))
           return;

        _addValue (cInst, v.cInst ());
    }

    public void addQualifier (CIMQualifier q)
    {
        if (cInst == 0 || q.cInst () == 0)
            return;

        _addQualifier (cInst, q.cInst ());
    }

    public boolean isArray ()
    {
        if (cInst == 0)
            return false;

        return _isArray (cInst);
    }

    public String getIdentifier ()
    {
        if (cInst == 0)
            return null;

        return _getIdentifier (cInst);
    }

    public int findQualifier (String qualifier)
    {
       return _findQualifier (cInst, qualifier);
    }

    public CIMQualifier getQualifier (int index)
    {
       int ciQualifier = _getQualifier (cInst, index);

       if (ciQualifier != 0)
       {
          return new CIMQualifier (ciQualifier);
       }

       return null;
    }

    static {
        System.loadLibrary("JMPIProviderManager");
    }
}
