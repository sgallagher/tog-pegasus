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
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi;

import java.net.URL;
import java.net.URLClassLoader;
import java.net.JarURLConnection;
import java.io.IOException;

public class JarClassLoader
       extends URLClassLoader
{
    private static boolean DEBUG = false;
    private        URL     url   = null;

    public JarClassLoader (URL url)
    {
        super (new URL[] { url });

        this.url = url;

        if (DEBUG)
        {
           System.err.println ("--- JarClassLoader::JarClassLoader: url: " + url);
        }
    }

    void connect ()
       throws IOException
    {
        URL              u  = null;
        JarURLConnection uc = null;

        u = new URL ("jar",       // protocol
                     "",          // host
                     url + "!/"); // file

        if (u == null)
        {
           throw new IOException ("cannot create a new URL " + url + "!/");
        }

        uc = (JarURLConnection)u.openConnection ();

        if (DEBUG)
        {
           System.err.println ("--- JarClassLoader::connect: u: " + u);
        }

        if (uc == null)
        {
           throw new IOException ("cannot open a connection on URL " + url + "!/");
        }
    }

    static public Class load (String jar, String cls)
       throws ClassNotFoundException, IOException
    {
        JarClassLoader cl = null;
        Class          c  = null;

        cl = new JarClassLoader (new URL ("File:" + jar));

        if (cl == null)
        {
           throw new IOException ("cannot create a new JarClassLoader " + jar);
        }

        cl.connect ();

        if (DEBUG)
        {
           System.err.println ("--- JarClassLoader::load: Loading " + cls);
        }

        c = cl.loadClass (cls);

        if (DEBUG)
        {
           System.err.println ("--- JarClassLoader::load: Loading done: " + c);
        }

        return c;
    }
}
