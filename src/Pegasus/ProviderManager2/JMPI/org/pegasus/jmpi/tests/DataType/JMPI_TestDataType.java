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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

import org.pegasus.jmpi.*;
import java.math.BigInteger;
import java.util.Vector;

public class JMPI_TestDataType
{
    final String className        = "JMPI_TestDataType";
    final String nameSpaceClass   = "test/JMPI";
    final String nameSpaceInterOp = "test/PG_InterOp";
    final String hostName         = "localhost";

    boolean      fDebug           = false;

    public void setDebug (boolean fDebug)
    {
        this.fDebug = fDebug;
    }

    public void runTests ()
    {
        CIMClient     cimClient        = null;

        try
        {
            int           portNo           = 5988;
            String        username         = "";
            String        password         = "";
            String        urlAttach        = "http://" + hostName + ":" + portNo;
            CIMNameSpace  clientNameSpace  = null;

            clientNameSpace = new CIMNameSpace (urlAttach, nameSpaceInterOp);
            if (clientNameSpace == null)
            {
                System.err.println ("Error: Could not create a CIMNameSpace ('" + urlAttach + "', '" + nameSpaceInterOp + "');");
                System.exit (1);
            }

            try
            {
                cimClient = new CIMClient (clientNameSpace, username, password);
            }
            catch (Exception e)
            {
                System.out.println ("Caught " + e);
                e.printStackTrace ();
            }
            if (cimClient == null)
            {
                System.err.println ("Error: Could not create a CIMClient ('" + clientNameSpace + "', '" + username + "', '" + password + "');");
                System.exit (1);
            }

            if (this.fDebug)
                System.err.println ("cimClient    = " + cimClient);

            runTest (cimClient);
        }
        catch (CIMException e)
        {
            System.out.println ("Error: CIMException: " + e);
            e.printStackTrace ();
        }
        catch (Exception e)
        {
            System.out.println ("Error: Exception: " + e);
            e.printStackTrace ();
        }
        finally
        {
            if (cimClient != null)
            {
                try
                {
                    cimClient.close ();
                }
                catch (CIMException e)
                {
                    System.out.println ("Error: CIMException: " + e);
                    e.printStackTrace ();
                }
            }
        }
    }

    private void runTest (CIMClient cimClient) throws CIMException, Exception
    {
        String        nameSpaceTest = "test/static";
        String        classNameTest = "JMPI_TestPropertyTypes";
        CIMObjectPath copInterOp    = null;
        CIMObjectPath copTest       = null;
        CIMInstance   cimInstance   = null;

        copInterOp = new CIMObjectPath (className, nameSpaceInterOp);
        if (copInterOp == null)
        {
            System.err.println ("Error: Could not create a CIMObjectPath ('" + className + "', '" + nameSpaceInterOp + "');");
            System.exit (1);
        }

        if (this.fDebug)
            System.err.println ("copInterOp   = " + copInterOp);

        copTest = new CIMObjectPath (classNameTest, nameSpaceTest);
        if (copTest == null)
        {
            System.err.println ("Error: Could not create a CIMObjectPath ('" + classNameTest + "', '" + nameSpaceTest + "');");
            System.exit (1);
        }

        copTest.addKey ("CreationClassName", new CIMValue (new String (classNameTest)));
        copTest.addKey ("InstanceId", new CIMValue (new UnsignedInt64 ("1")));

        if (this.fDebug)
            System.err.println ("copTest      = " + copTest);

        cimInstance = cimClient.getInstance (copTest, false);

        if (this.fDebug)
            System.err.println ("cimInstance = " + cimInstance);

        if (runTestUINT8 (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT8.");
        else
            System.out.println ("Error: UINT8 failed!");
        if (runTestSINT8 (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT8.");
        else
            System.out.println ("Error: SINT8 failed!");
        if (runTestUINT16 (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT16.");
        else
            System.out.println ("Error: UINT16 failed!");
        if (runTestSINT16 (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT16.");
        else
            System.out.println ("Error: SINT16 failed!");
        if (runTestUINT32 (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT32.");
        else
            System.out.println ("Error: UINT32 failed!");
        if (runTestSINT32 (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT32.");
        else
            System.out.println ("Error: SINT32 failed!");
        if (runTestUINT64 (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT64.");
        else
            System.out.println ("Error: UINT64 failed!");
        if (runTestSINT64 (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT64.");
        else
            System.out.println ("Error: SINT64 failed!");
        if (runTestSTRING (cimClient, cimInstance, copTest))
            System.out.println ("Success: STRING.");
        else
            System.out.println ("Error: STRING failed!");
        if (runTestBOOLEAN (cimClient, cimInstance, copTest))
            System.out.println ("Success: BOOLEAN.");
        else
            System.out.println ("Error: BOOLEAN failed!");
        if (runTestREAL32 (cimClient, cimInstance, copTest))
            System.out.println ("Success: REAL32.");
        else
            System.out.println ("Error: REAL32 failed!");
        if (runTestREAL64 (cimClient, cimInstance, copTest))
            System.out.println ("Success: REAL64.");
        else
            System.out.println ("Error: REAL64 failed!");
        if (runTestDATETIME (cimClient, cimInstance, copTest))
            System.out.println ("Success: DATETIME.");
        else
            System.out.println ("Error: DATETIME failed!");
        if (runTestCHAR16 (cimClient, cimInstance, copTest))
            System.out.println ("Success: CHAR16.");
        else
            System.out.println ("Error: CHAR16 failed!");
        if (runTestINSTANCE (cimClient, cimInstance, copTest))
            System.out.println ("Success: INSTANCE.");
        else
            System.out.println ("Error: INSTANCE failed!");
        if (runTestCLASS (cimClient, cimInstance, copTest))
            System.out.println ("Success: CLASS.");
        else
            System.out.println ("Error: CLASS failed!");
        if (runTestUINT8_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT8_ARRAY.");
        else
            System.out.println ("Error: UINT8_ARRAY failed!");
        if (runTestSINT8_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT8_ARRAY.");
        else
            System.out.println ("Error: SINT8_ARRAY failed!");
        if (runTestUINT16_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT16_ARRAY.");
        else
            System.out.println ("Error: UINT16_ARRAY failed!");
        if (runTestSINT16_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT16_ARRAY.");
        else
            System.out.println ("Error: SINT16_ARRAY failed!");
        if (runTestUINT32_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT32_ARRAY.");
        else
            System.out.println ("Error: UINT32_ARRAY failed!");
        if (runTestSINT32_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT32_ARRAY.");
        else
            System.out.println ("Error: SINT32_ARRAY failed!");
        if (runTestUINT64_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: UINT64_ARRAY.");
        else
            System.out.println ("Error: UINT64_ARRAY failed!");
        if (runTestSINT64_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: SINT64_ARRAY.");
        else
            System.out.println ("Error: SINT64_ARRAY failed!");
        if (runTestSTRING_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: STRING_ARRAY.");
        else
            System.out.println ("Error: STRING_ARRAY failed!");
        if (runTestBOOLEAN_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: BOOLEAN_ARRAY.");
        else
            System.out.println ("Error: BOOLEAN_ARRAY failed!");
        if (runTestREAL32_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: REAL32_ARRAY.");
        else
            System.out.println ("Error: REAL32_ARRAY failed!");
        if (runTestREAL64_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: REAL64_ARRAY.");
        else
            System.out.println ("Error: REAL64_ARRAY failed!");
        if (runTestDATETIME_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: DATETIME_ARRAY.");
        else
            System.out.println ("Error: DATETIME_ARRAY failed!");
        if (runTestCHAR16_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: CHAR16_ARRAY.");
        else
            System.out.println ("Error: CHAR16_ARRAY failed!");
        if (runTestINSTANCE_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: INSTANCE_ARRAY.");
        else
            System.out.println ("Error: INSTANCE_ARRAY failed!");
        if (runTestCLASS_ARRAY (cimClient, cimInstance, copTest))
            System.out.println ("Success: CLASS_ARRAY.");
        else
            System.out.println ("Error: CLASS_ARRAY failed!");
    }

    private boolean runTestUINT8 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT8
            CIMValue cv = null;

            cv = new CIMValue (new UnsignedInt8 ((byte)16), new CIMDataType (CIMDataType.UINT8));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a uint8");
            }

            cimInstance.setProperty ("PropertyUint8", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get UINT8
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyUint8");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt8 ui8         = (UnsignedInt8)o;

            if (this.fDebug)
                System.err.println ("ui8         = " + ui8);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT8 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT8
            CIMValue cv = null;

            cv = new CIMValue (new Byte ((byte)-42), new CIMDataType (CIMDataType.SINT8));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a sint8");
            }

            cimInstance.setProperty ("PropertySint8", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get SINT8
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertySint8");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Byte b                   = (Byte)o;

            if (this.fDebug)
                System.err.println ("b           = " + b);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT16 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT16
            CIMValue cv = null;

            cv = new CIMValue (new UnsignedInt16 ((int)9831), new CIMDataType (CIMDataType.UINT16));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Uint16");
            }

            cimInstance.setProperty ("PropertyUint16", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get UINT16
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyUint16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt16 ui16       = (UnsignedInt16)o;

            if (this.fDebug)
                System.err.println ("ui16        = " + ui16);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT16 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT16
            CIMValue cv = null;

            cv = new CIMValue (new Short ((short)-1578), new CIMDataType (CIMDataType.SINT16));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a sint16");
            }

            cimInstance.setProperty ("PropertySint16", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get SINT16
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertySint16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Short s                  = (Short)o;

            if (this.fDebug)
                System.err.println ("s           = " + s);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT32 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT32
            CIMValue cv = null;

            cv = new CIMValue (new UnsignedInt32 (33000L), new CIMDataType (CIMDataType.UINT32));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Uint32");
            }

            cimInstance.setProperty ("PropertyUint32", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get UINT32
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyUint32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt32 ui32       = (UnsignedInt32)o;

            if (this.fDebug)
                System.err.println ("ui32        = " + ui32);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT32 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT32
            CIMValue cv = null;

            cv = new CIMValue (new Integer (-45000), new CIMDataType (CIMDataType.SINT32));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Sint32");
            }

            cimInstance.setProperty ("PropertySint32", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get SINT32
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertySint32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Integer i                = (Integer)o;

            if (this.fDebug)
                System.err.println ("i           = " + i);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT64 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT64
            CIMValue cv = null;

            cv = new CIMValue (new UnsignedInt64 (new BigInteger ("77000000")), new CIMDataType (CIMDataType.UINT64));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Uint64");
            }

            cimInstance.setProperty ("PropertyUint64", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get UINT64
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyUint64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt64 ui64       = (UnsignedInt64)o;

            if (this.fDebug)
                System.err.println ("ui64        = " + ui64);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT64 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT64
            CIMValue cv = null;

            cv = new CIMValue (new Long ((long)-99000000), new CIMDataType (CIMDataType.SINT64));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Sint64");
            }

            cimInstance.setProperty ("PropertySint64", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get SINT64
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertySint64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Long l                   = (Long)o;

            if (this.fDebug)
                System.err.println ("l           = " + l);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSTRING (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set STRING
            CIMValue cv = null;

            cv = new CIMValue ("Bob lives", new CIMDataType (CIMDataType.STRING));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a String");
            }

            cimInstance.setProperty ("PropertyString", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get STRING
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyString");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            String s                 = (String)o;

            if (this.fDebug)
                System.err.println ("s           = " + s);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestBOOLEAN (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set BOOLEAN
            CIMValue cv = null;

            cv = new CIMValue (new Boolean (false), new CIMDataType (CIMDataType.BOOLEAN));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Boolean");
            }

            cimInstance.setProperty ("PropertyBoolean", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get BOOLEAN
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyBoolean");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Boolean b                = (Boolean)o;

            if (this.fDebug)
                System.err.println ("b           = " + b);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestREAL32 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set REAL32
            CIMValue cv = null;

            cv = new CIMValue (new Float (3.14159), new CIMDataType (CIMDataType.REAL32));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Real32");
            }

            cimInstance.setProperty ("PropertyReal32", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get REAL32
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyReal32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Float f                  = (Float)o;

            if (this.fDebug)
                System.err.println ("f           = " + f);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestREAL64 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set REAL64
            CIMValue cv = null;

            cv = new CIMValue (new Double (2.718281828), new CIMDataType (CIMDataType.REAL64));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Real64");
            }

            cimInstance.setProperty ("PropertyReal64", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get REAL64
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyReal64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Double d                 = (Double)o;

            if (this.fDebug)
                System.err.println ("d           = " + d);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestDATETIME (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set DATETIME
            CIMValue cv = null;

            cv = new CIMValue (new CIMDateTime ("20051216161710.000000:000"), new CIMDataType (CIMDataType.DATETIME));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a DateTime");
            }

            cimInstance.setProperty ("PropertyDateTime", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get DATETIME
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyDateTime");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMDateTime cdt          = (CIMDateTime)o;

            if (this.fDebug)
                System.err.println ("char        = " + cdt);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestCHAR16 (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set CHAR16
            CIMValue cv = null;

            cv = new CIMValue (new Character ('!'), new CIMDataType (CIMDataType.CHAR16));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a Char16");
            }

            cimInstance.setProperty ("PropertyChar16", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get CHAR16
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyChar16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Character   ch           = (Character)o;

            if (this.fDebug)
                System.err.println ("char        = " + ch);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestINSTANCE (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set INSTANCE
            CIMValue cv = null;

            cv = new CIMValue (new CIMObject (cimInstance), new CIMDataType (CIMDataType.OBJECT));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an object");
            }

            cimInstance.setProperty ("PropertyObject", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get INSTANCE
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyObject");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMObject   cimObject    = (CIMObject)o;
            CIMInstance cimInstance2 = cimObject.getCIMInstance ();

            if (this.fDebug)
                System.err.println ("cimInstance2 = " + cimInstance2);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestCLASS (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set CLASS
            CIMValue cv       = null;
            CIMClass cimClass = null;

            cimClass = cimClient.getClass (cop, false);

            if (this.fDebug)
                System.err.println ("cimClass     = " + cimClass);

            cv = new CIMValue (new CIMObject (cimClass), new CIMDataType (CIMDataType.OBJECT));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting a class");
            }

            cimInstance.setProperty ("PropertyObject", cv);

            if (this.fDebug)
                System.err.println ("cimInstance = " + cimInstance);

            // get CLASS
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyObject");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMObject   cimObject = (CIMObject)o;
            CIMClass    cimClass2 = cimObject.getCIMClass ();

            if (this.fDebug)
                System.err.println ("cimClass2    = " + cimClass2);

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT8_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT8_ARRAY
            CIMValue cv          = null;
            Vector   vectorUINT8 = new Vector ();

            vectorUINT8.addElement (new UnsignedInt8 ((byte)64));
            vectorUINT8.addElement (new UnsignedInt8 ((byte)127));
            vectorUINT8.addElement (new UnsignedInt8 ((byte)10));

            cv = new CIMValue (vectorUINT8, new CIMDataType (CIMDataType.UINT8_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of uint8s");
            }

            cimInstance.setProperty ("PropertyArrayUint8", cv);

            // get UINT8_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayUint8");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt8 ui8[]       = (UnsignedInt8[])o;

            if (this.fDebug)
            {
                System.err.print ("ui8         = [");
                for (int i = 0; i < ui8.length; i++)
                {
                    System.err.print (ui8[i]);
                    if (i < ui8.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT8_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT8_ARRAY
            CIMValue cv          = null;
            Vector   vectorSINT8 = new Vector ();

            vectorSINT8.addElement (new Byte ((byte)0));
            vectorSINT8.addElement (new Byte ((byte)-128));
            vectorSINT8.addElement (new Byte ((byte)-77));

            cv = new CIMValue (vectorSINT8, new CIMDataType (CIMDataType.SINT8_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of sint8s");
            }

            cimInstance.setProperty ("PropertyArraySint8", cv);

            // get SINT8_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArraySint8");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Byte si8[] = (Byte[])o;

            if (this.fDebug)
            {
                System.err.print ("si8         = [");
                for (int i = 0; i < si8.length; i++)
                {
                    System.err.print (si8[i]);
                    if (i < si8.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT16_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT16_ARRAY
            CIMValue cv           = null;
            Vector   vectorUINT16 = new Vector ();   

            vectorUINT16.addElement (new UnsignedInt16 ((int)65535));
            vectorUINT16.addElement (new UnsignedInt16 ((int)33333));
            vectorUINT16.addElement (new UnsignedInt16 ((int)11111));

            cv = new CIMValue (vectorUINT16, new CIMDataType (CIMDataType.UINT16_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of uint16s");
            }

            cimInstance.setProperty ("PropertyArrayUint16", cv);

            // get UINT16_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayUint16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt16 ui16[] = (UnsignedInt16[])o;

            if (this.fDebug)
            {
                System.err.print ("ui16        = [");
                for (int i = 0; i < ui16.length; i++)
                {
                    System.err.print (ui16[i]);
                    if (i < ui16.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT16_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT16_ARRAY
            CIMValue cv           = null;
            Vector   vectorSINT16 = new Vector ();

            vectorSINT16.addElement (new Short ((byte)0));
            vectorSINT16.addElement (new Short ((byte)-6173));
            vectorSINT16.addElement (new Short ((byte)-32768));

            cv = new CIMValue (vectorSINT16, new CIMDataType (CIMDataType.SINT16_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of sint16s");
            }

            cimInstance.setProperty ("PropertyArraySint16", cv);

            // get SINT16_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArraySint16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Short si16[] = (Short[])o;

            if (this.fDebug)
            {
                System.err.print ("si16        = [");
                for (int i = 0; i < si16.length; i++)
                {
                    System.err.print (si16[i]);
                    if (i < si16.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT32_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT32_ARRAY
            CIMValue cv           = null;
            Vector   vectorUINT32 = new Vector ();   

            vectorUINT32.addElement (new UnsignedInt32 (4294967295L));
            vectorUINT32.addElement (new UnsignedInt32 (2222222222L));
            vectorUINT32.addElement (new UnsignedInt32 (108L));

            cv = new CIMValue (vectorUINT32, new CIMDataType (CIMDataType.UINT32_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of uint32s");
            }

            cimInstance.setProperty ("PropertyArrayUint32", cv);

            // get UINT32_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayUint32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt32 ui32[] = (UnsignedInt32[])o;

            if (this.fDebug)
            {
                System.err.print ("ui32        = [");
                for (int i = 0; i < ui32.length; i++)
                {
                    System.err.print (ui32[i]);
                    if (i < ui32.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT32_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT32_ARRAY
            CIMValue cv           = null;
            Vector   vectorSINT32 = new Vector ();

            vectorSINT32.addElement (new Integer (-12345));
            vectorSINT32.addElement (new Integer (2147483647));
            vectorSINT32.addElement (new Integer (-2147483648));

            cv = new CIMValue (vectorSINT32, new CIMDataType (CIMDataType.SINT32_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of sint32s");
            }

            cimInstance.setProperty ("PropertyArraySint32", cv);

            // get SINT32_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArraySint32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Integer si32[] = (Integer[])o;

            if (this.fDebug)
            {
                System.err.print ("si32        = [");
                for (int i = 0; i < si32.length; i++)
                {
                    System.err.print (si32[i]);
                    if (i < si32.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestUINT64_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set UINT64_ARRAY
            CIMValue cv           = null;
            Vector   vectorUINT64 = new Vector ();   

            vectorUINT64.addElement (new UnsignedInt64 (new BigInteger ("7482743276264381934")));
            vectorUINT64.addElement (new UnsignedInt64 (new BigInteger ("11122233344455566677")));
            vectorUINT64.addElement (new UnsignedInt64 (new BigInteger ("18446744073709551615")));

            cv = new CIMValue (vectorUINT64, new CIMDataType (CIMDataType.UINT64_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of uint64s");
            }

            cimInstance.setProperty ("PropertyArrayUint64", cv);

            // get UINT64_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayUint64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            UnsignedInt64 ui64[] = (UnsignedInt64[])o;

            if (this.fDebug)
            {
                System.err.print ("ui64        = [");
                for (int i = 0; i < ui64.length; i++)
                {
                    System.err.print (ui64[i]);
                    if (i < ui64.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSINT64_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set SINT64_ARRAY
            CIMValue cv = null;

            Vector vectorSINT64 = new Vector ();

            vectorSINT64.addElement (new Long (1736513846286344121L));
            vectorSINT64.addElement (new Long (-9223372036854775808L));
            vectorSINT64.addElement (new Long (9223372036854775807L));

            cv = new CIMValue (vectorSINT64, new CIMDataType (CIMDataType.SINT64_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of sint64s");
            }

            cimInstance.setProperty ("PropertyArraySint64", cv);

            // get SINT64_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArraySint64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Long si64[] = (Long[])o;

            if (this.fDebug)
            {
                System.err.print ("si64        = [");
                for (int i = 0; i < si64.length; i++)
                {
                    System.err.print (si64[i]);
                    if (i < si64.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestSTRING_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set STRING_ARRAY
            CIMValue cv           = null;
            Vector   vectorSTRING = new Vector ();   

            vectorSTRING.addElement (new String ("Hello World."));
            vectorSTRING.addElement (new String ("This is a test."));
            vectorSTRING.addElement (new String ("Bob lives!"));

            cv = new CIMValue (vectorSTRING, new CIMDataType (CIMDataType.STRING_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of strings");
            }

            cimInstance.setProperty ("PropertyArrayString", cv);

            // get STRING_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayString");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            String string[] = (String[])o;

            if (this.fDebug)
            {
                System.err.print ("string      = [");
                for (int i = 0; i < string.length; i++)
                {
                    System.err.print (string[i]);
                    if (i < string.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestBOOLEAN_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set BOOLEAN_ARRAY
            CIMValue cv            = null;
            Vector   vectorBOOLEAN = new Vector ();   

            vectorBOOLEAN.addElement (new Boolean (true));
            vectorBOOLEAN.addElement (new Boolean (false));
            vectorBOOLEAN.addElement (new Boolean (true));

            cv = new CIMValue (vectorBOOLEAN, new CIMDataType (CIMDataType.BOOLEAN_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of booleans");
            }

            cimInstance.setProperty ("PropertyArrayBoolean", cv);

            // get BOOLEAN_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayBoolean");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Boolean b[] = (Boolean[])o;

            if (this.fDebug)
            {
                System.err.print ("b           = [");
                for (int i = 0; i < b.length; i++)
                {
                    System.err.print (b[i]);
                    if (i < b.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestREAL32_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set REAL32_ARRAY
            CIMValue cv           = null;
            Vector   vectorREAL32 = new Vector ();   

            vectorREAL32.addElement (new Float (1.23456789012345678e-01));
            vectorREAL32.addElement (new Float (1.40129846432481707e-45));
            vectorREAL32.addElement (new Float (3.40282346638528860e+38));

            cv = new CIMValue (vectorREAL32, new CIMDataType (CIMDataType.REAL32_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of REAL32s");
            }

            cimInstance.setProperty ("PropertyArrayREAL32", cv);

            // get REAL32_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayREAL32");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Float f[] = (Float[])o;

            if (this.fDebug)
            {
                System.err.print ("f           = [");
                for (int i = 0; i < f.length; i++)
                {
                    System.err.print (f[i]);
                    if (i < f.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestREAL64_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set REAL64_ARRAY
            CIMValue cv           = null;
            Vector   vectorREAL64 = new Vector ();

            vectorREAL64.addElement (new Double (9.876));
            vectorREAL64.addElement (new Double (4.94065645841246544e-324d));
            vectorREAL64.addElement (new Double (1.79769313486231570e+308d));

            cv = new CIMValue (vectorREAL64, new CIMDataType (CIMDataType.REAL64_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of REAL64s");
            }

            cimInstance.setProperty ("PropertyArrayREAL64", cv);

            // get REAL64_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayREAL64");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Double d[] = (Double[])o;

            if (this.fDebug)
            {
                System.err.print ("d           = [");
                for (int i = 0; i < d.length; i++)
                {
                    System.err.print (d[i]);
                    if (i < d.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestDATETIME_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set DATETIME_ARRAY
            CIMValue cv             = null;
            Vector   vectorDATETIME = new Vector ();

            vectorDATETIME.addElement (new CIMDateTime ("20051216161710.000000:000"));
            vectorDATETIME.addElement (new CIMDateTime ("20041115151609.100000:000"));
            vectorDATETIME.addElement (new CIMDateTime ("20031014141508.200000:000"));

            cv = new CIMValue (vectorDATETIME, new CIMDataType (CIMDataType.DATETIME_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of datetime");
            }

            cimInstance.setProperty ("PropertyArrayDatetime", cv);

            // get DATETIME_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayDatetime");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMDateTime cdt[] = (CIMDateTime[])o;

            if (this.fDebug)
            {
                System.err.print ("cdt         = [");
                for (int i = 0; i < cdt.length; i++)
                {
                    System.err.print (cdt[i]);
                    if (i < cdt.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestCHAR16_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set CHAR16_ARRAY
            CIMValue cv           = null;
            Vector   vectorCHAR16 = new Vector ();

            vectorCHAR16.addElement (new Character ('B'));
            vectorCHAR16.addElement (new Character ('o'));
            vectorCHAR16.addElement (new Character ('b'));

            cv = new CIMValue (vectorCHAR16, new CIMDataType (CIMDataType.CHAR16_ARRAY));

            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of char16");
            }

            cimInstance.setProperty ("PropertyArrayChar16", cv);

            // get CHAR16_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayChar16");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            Character c[] = (Character[])o;

            if (this.fDebug)
            {
                System.err.print ("c           = [");
                for (int i = 0; i < c.length; i++)
                {
                    System.err.print (c[i]);
                    if (i < c.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }

            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestINSTANCE_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set INSTANCE_ARRAY
            CIMValue cv             = null;
            Vector   vectorINSTANCE = new Vector ();
    
            vectorINSTANCE.addElement (new CIMObject (cimInstance));
            vectorINSTANCE.addElement (new CIMObject (cimInstance));
            vectorINSTANCE.addElement (new CIMObject (cimInstance));
    
            cv = new CIMValue (vectorINSTANCE, new CIMDataType (CIMDataType.OBJECT_ARRAY));
    
            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of objects");
            }

            cimInstance.setProperty ("PropertyArrayObject", cv);

            // get INSTANCE_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayObject");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMObject co[] = (CIMObject[])o;

            for (int i = 0; i < co.length; i++)
            {
                CIMInstance cimInstance2 = co[i].getCIMInstance ();

                if (cimInstance2 == null)
                    return false;
            }

            if (this.fDebug)
            {
                System.err.print ("co          = [");
                for (int i = 0; i < co.length; i++)
                {
                    System.err.print (co[i]);
                    if (i < co.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }
    
            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    private boolean runTestCLASS_ARRAY (CIMClient cimClient, CIMInstance cimInstance, CIMObjectPath cop)
    {
        try
        {
            // set CLASS_ARRAY
            CIMValue cv          = null;
            CIMClass cimClass    = null;
            Vector   vectorCLASS = new Vector ();
    
            cimClass = cimClient.getClass (cop, false);
    
            if (this.fDebug)
                System.err.println ("cimClass     = " + cimClass);
    
            vectorCLASS.addElement (new CIMObject (cimClass));
            vectorCLASS.addElement (new CIMObject (cimClass));
            vectorCLASS.addElement (new CIMObject (cimClass));
    
            cv = new CIMValue (vectorCLASS, new CIMDataType (CIMDataType.OBJECT_ARRAY));
    
            if (this.fDebug)
            {
                System.err.println ("cv           = " + cv);
                System.err.println ("Setting an array of objects");
            }

            cimInstance.setProperty ("PropertyArrayObject", cv);

            // get CLASS_ARRAY
            CIMProperty cp = null;

            cp = cimInstance.getProperty ("PropertyArrayObject");

            if (this.fDebug)
                System.err.println ("cp          = " + cp);

            cv = cp.getValue ();

            if (this.fDebug)
                System.err.println ("cv          = " + cv);

            Object o = cv.getValue (false);

            if (this.fDebug)
                System.err.println ("o           = " + o);

            CIMObject co[] = (CIMObject[])o;

            for (int i = 0; i < co.length; i++)
            {
                CIMClass cimClass2 = co[i].getCIMClass ();

                if (cimClass2 == null)
                    return false;
            }

            if (this.fDebug)
            {
                System.err.print ("co          = [");
                for (int i = 0; i < co.length; i++)
                {
                    System.err.print (co[i]);
                    if (i < co.length - 1)
                        System.err.print (",");
                }
                System.err.println ("]");
            }
    
            return true;
        }
        catch (Exception e)
        {
            System.out.println ("Caught " + e);
            e.printStackTrace ();
            return false;
        }
    }

    public static void main (String args[])
    {
        JMPI_TestDataType inst      = new JMPI_TestDataType ();
        boolean           fExecuted = false;

        for (int i = 0; i < args.length; i++)
        {
            if (args[i].equalsIgnoreCase ("debug"))
            {
                inst.setDebug (true);
            }
        }

        if (!fExecuted)
            inst.runTests ();
    }
}
