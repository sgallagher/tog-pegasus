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
// Author:      Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

public class CIMObject
{
    int         cInst;
    boolean     fIsClass;
    CIMClass    cimClass;
    CIMInstance cimInstance;

    private native int  _newClass               (int cInst);
    private native int  _newInstance            (int cInst);
    private native void _finalize               (int cInst);

    protected void finalize ()
    {
       _finalize (cInst);
    }

    public CIMObject (CIMClass cc)
    {
        cInst       = _newClass (cc.cInst);
        fIsClass    = true;
        cimClass    = cc;
        cimInstance = null;
    }

    public CIMObject (CIMInstance ci)
    {
        cInst       = _newInstance (ci.cInst);
        fIsClass    = false;
        cimClass    = null;
        cimInstance = ci;
    }

    public CIMObject (int inst, boolean isClass)
    {
        if (isClass)
        {
            cInst       = _newClass (inst);
            fIsClass    = true;
            cimClass    = new CIMClass (inst);
            cimInstance = null;
        }
        else
        {
            cInst       = _newInstance (inst);
            fIsClass    = true;
            cimClass    = null;
            cimInstance = new CIMInstance (inst);
        }
    }

    public CIMClass getCIMClass ()
    {
        return cimClass;
    }

    public CIMInstance getCIMInstance ()
    {
        return cimInstance;
    }

    public boolean isClass ()
    {
        return fIsClass;
    }

    public boolean isInstance ()
    {
        return !fIsClass;
    }

    static {
       System.loadLibrary ("JMPIProviderManager");
    }
}
