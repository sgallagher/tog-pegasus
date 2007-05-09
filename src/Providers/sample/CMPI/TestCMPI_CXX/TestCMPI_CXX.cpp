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
// Please be aware that the CMPI C++ API is NOT a standard currently.
//
//%/////////////////////////////////////////////////////////////////////////////

#include "TestCMPI_CXX.h"

#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_STD;

/* -----------------------------------------------------------------------*/
/*      Provider Factory - IMPORTANT for entry point generation           */
/* -----------------------------------------------------------------------*/

CMProviderBase (TestCMPI_CXX_Provider);

CMInstanceMIFactory (TestCMPI_CXX, TestCMPI_CXX_Provider);

/* -----------------------------------------------------------------------*/
/*                          Base Provider Interface                       */
/* -----------------------------------------------------------------------*/

TestCMPI_CXX::TestCMPI_CXX (const CmpiBroker&  mbp, 
                            const CmpiContext& ctx)
    : CmpiBaseMI (mbp, ctx),
      CmpiInstanceMI (mbp, ctx),
      cppBroker (mbp) 
{
#ifdef PEGASUS_DEBUG
    cout << "TestCMPI_CXX::TestCMPI_CXX ()" << endl;
#endif
}

TestCMPI_CXX::~TestCMPI_CXX ()
{
#ifdef PEGASUS_DEBUG
    cout << "TestCMPI_CXX::~TestCMPI_CXX ()" << endl;
#endif
}

int
TestCMPI_CXX::isUnloadable() const
{
    return 1;  // may be unloaded
}

const char *
convertTime (CmpiDateTime& dtTime)
{
   static char timeBuffer[27];
   CMPIUint64  ui64Time;
   time_t      tTime;

   ui64Time = dtTime.getDateTime ();
   tTime    = ui64Time / 1000000;

   return ctime_r (&tTime, timeBuffer);
}

void
printData (const CmpiData& d)
{
#ifdef PEGASUS_DEBUG
   CMPIType type = d.getType ();

   if (type & CMPI_ARRAY)
   {
      CmpiArray v = d.getArray ();
      cout << "[" << v.size () << ":";
      for (CMPICount i = 0, n = v.size (); i < n; i++)
      {
         CmpiArrayIdx idx = v[i];

         printData (idx.getData ());

         if (i + 1 < n)
         {
            cout << ",";
         }
      }
      cout << "]" << endl;
      return;
   }

   switch (type)
   {
   case CMPI_boolean:
   {
      CMPIBoolean v = d.getBoolean ();
      cout << v;
      break;
   }
   case CMPI_char16:
   {
      CMPIChar16 v = d.getChar16 ();
      cout << v;
      break;
   }
   case CMPI_real32:
   {
      CMPIReal32 v = d.getReal32 ();
      cout << v;
      break;
   }
   case CMPI_real64:
   {
      CMPIReal64 v = d.getReal64 ();
      cout << v;
      break;
   }
   case CMPI_uint8:
   {
      CMPIUint8 v = d.getUint8 ();
      cout << v;
      break;
   }
   case CMPI_uint16:
   {
      CMPIUint16 v = d.getUint16 ();
      cout << v;
      break;
   }
   case CMPI_uint32:
   {
      CMPIUint32 v = d.getUint32 ();
      cout << v;
      break;
   }
   case CMPI_uint64:
   {
      CMPIUint64 v = d.getUint64 ();
      cout << v;
      break;
   }
   case CMPI_sint8:
   {
      CMPISint8 v = d.getSint8 ();
      cout << v;
      break;
   }
   case CMPI_sint16:
   {
      CMPISint16 v = d.getSint16 ();
      cout << v;
      break;
   }
   case CMPI_sint32:
   {
      CMPISint32 v = d.getSint32 ();
      cout << v;
      break;
   }
   case CMPI_sint64:
   {
      CMPISint64 v = d.getSint64 ();
      cout << v;
      break;
   }
   case CMPI_instance:
   {
      CmpiInstance v = d.getInstance ();
      cout << "Instance";
      break;
   }
   case CMPI_ref:
   {
      cout << "CMPI_ref @TBD";
      break;
   }
   case CMPI_args:
   {
      cout << "CMPI_args @TBD";
      break;
   }
   case CMPI_class:
   {
      cout << "CMPI_class @TBD";
      break;
   }
   case CMPI_filter:
   {
      cout << "CMPI_filter @TBD";
      break;
   }
   case CMPI_enumeration:
   {
      cout << "CMPI_enumeration @TBD";
      break;
   }
   case CMPI_string:
   {
      CmpiString v = d.getString ();
      if (v.charPtr ())
      {
         cout << "\"" << v.charPtr () << "\"";
      }
      else
      {
         cout << "\"\"";
      }
      break;
   }
   case CMPI_chars:
   {
      const char *v = d.getCString ();
      if (v)
      {
         cout << "\"" << v << "\"";
      }
      else
      {
         cout << "\"\"";
      }
      break;
   }
   case CMPI_dateTime:
   {
      CmpiDateTime v = d.getDateTime ();
      cout << "DateTime ";
      try
      {
         CMPIUint64 dt = v.getDateTime();
         cout << dt;
      }
      catch (const CmpiStatus& e)
      {
         cout << "(null)";
      }
      break;
   }
   case CMPI_ptr:
   {
      cout << "CMPI_ptr @TBD";
      break;
   }
   case CMPI_charsptr:
   {
      cout << "CMPI_charsptr @TBD";
      break;
   }
   default:
   {
      cerr << "Error: printData: Unknown type " << type;
      throw CmpiStatus (CMPI_RC_ERR_FAILED);
   }
   }
   cout << endl;
#endif
}

void
printInstance (const CmpiInstance& inst)
{
#ifdef PEGASUS_DEBUG
   cout << "inst.getPropertyCount: " << inst.getPropertyCount () << endl;

   for (int i = 0, n = inst.getPropertyCount (); i < n; i++)
   {
      CmpiString name;
      CmpiData   d;

      d = inst.getProperty (i, &name);

      cout << i << ":";
      if (name.charPtr ())
      {
         cout << name.charPtr () << ":";
      }
      printData (d);
   }
#endif
}

void
printObjectPath (const CmpiObjectPath& cop)
{
#ifdef PEGASUS_DEBUG
   cout << "Hostname = ";
   CmpiString hostname = cop.getHostname ();
   if (hostname.charPtr ())
   {
      cout << hostname.charPtr ();
   }
   cout << endl;

   cout << "classname = ";
   CmpiString classname = cop.getClassName ();
   if (classname.charPtr ())
   {
      cout << classname.charPtr ();
   }
   cout << endl;
   cout << "keycount = " << cop.getKeyCount () << endl;

   for (unsigned int i = 0, n = cop.getKeyCount (); i < n; i++)
   {
      CmpiString name;
      CmpiData   value;

      value = cop.getKey (i, &name);

      cout << i << ":";
      if (name.charPtr ())
      {
         cout << name.charPtr () << ":";
      }
      printData (value);
   }
#endif
}

CmpiStatus
TestCMPI_CXX::initialize (const CmpiContext& ctx)
{
#ifdef PEGASUS_DEBUG
    cout << "TestCMPI_CXX::initialize () enter" << endl;
#endif

    try
    {
        CMPIBoolean    bData1     = true;
        CMPIChar16     chData1    = '1';
        CMPIUint8      ui8Data1   = 2;
        CMPIUint16     ui16Data1  = 3;
        CMPIUint32     ui32Data1  = 4;
        CMPIUint64     ui64Data1  = 5;
        CMPISint8      si8Data1   = 6;
        CMPISint16     si16Data1  = 7;
        CMPISint32     si32Data1  = 8;
        CMPISint64     si64Data1  = 9;
        CMPIReal32     r32Data1   = 10;
        CMPIReal64     r64Data1   = 11;
        CmpiString     strData1   = "12";
        const char     achData1[] = { '1', '3', '\0' };
        CmpiDateTime   dtData1;
        CmpiObjectPath copData1 ("root/SampleProvider", "CWS_PlainFile_CXX");
        CmpiInstance   ciData1 (copData1);
        CmpiData       d;

        // Test CmpiArray constructors
        CmpiArray      abData1 (1, CMPI_boolean);
        CmpiArray      achData2 (1, CMPI_char16);
        CmpiArray      ar32Data3 (1, CMPI_real32);
        CmpiArray      ar64Data4 (1, CMPI_real64);
        CmpiArray      aui8Data5 (1, CMPI_uint8);
        CmpiArray      aui16Data6 (1, CMPI_uint16);
        CmpiArray      aui32Data7 (1, CMPI_uint32);
        CmpiArray      aui64Data8 (1, CMPI_uint64);
        CmpiArray      asi8Data9 (1, CMPI_sint8);
        CmpiArray      asi16Data10 (1, CMPI_sint16);
        CmpiArray      asi32Data11 (1, CMPI_sint32);
        CmpiArray      asi64Data12 (1, CMPI_sint64);
        CmpiArray      ainstData13 (1, CMPI_instance);
        CmpiArray      arefData14 (1, CMPI_ref);
        CmpiArray      aargData15 (1, CMPI_args);
        CmpiArray      aclaData16 (1, CMPI_class);
        CmpiArray      afilData17 (1, CMPI_filter);
        CmpiArray      aenuData18 (1, CMPI_enumeration);
        CmpiArray      astrData19 (1, CMPI_string);
        CmpiArray      achData20 (1, CMPI_chars);
        CmpiArray      adtData21 (1, CMPI_dateTime);
        CmpiArray      aptrData22 (1, CMPI_ptr);
        CmpiArray      achptrData23 (1, CMPI_charsptr);
        CmpiArray      aData24;

        // Test CmpiArray size
        if (abData1.size () != 1)
        {
            cerr << "Error: abData1.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (achData2.size () != 1)
        {
            cerr << "Error: achData2.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (ar32Data3.size () != 1)
        {
            cerr << "Error: ar32Data3.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (ar64Data4.size () != 1)
        {
            cerr << "Error: ar64Data4.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aui8Data5.size () != 1)
        {
            cerr << "Error: aui8Data5.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aui16Data6.size () != 1)
        {
            cerr << "Error: aui16Data6.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aui32Data7.size () != 1)
        {
            cerr << "Error: aui32Data7.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aui64Data8.size () != 1)
        {
            cerr << "Error: aui64Data8.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (asi8Data9.size () != 1)
        {
            cerr << "Error: asi8Data9.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (asi16Data10.size () != 1)
        {
            cerr << "Error: asi16Data10.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (asi32Data11.size () != 1)
        {
            cerr << "Error: asi32Data11.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (asi64Data12.size () != 1)
        {
            cerr << "Error: asi64Data12.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (ainstData13.size () != 1)
        {
            cerr << "Error: ainstData13.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (arefData14.size () != 1)
        {
            cerr << "Error: arefData14.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aargData15.size () != 1)
        {
            cerr << "Error: aargData15.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aclaData16.size () != 1)
        {
            cerr << "Error: aclaData16.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (afilData17.size () != 1)
        {
            cerr << "Error: afilData17.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aenuData18.size () != 1)
        {
            cerr << "Error: aenuData18.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (astrData19.size () != 1)
        {
            cerr << "Error: astrData19.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (achData20.size () != 1)
        {
            cerr << "Error: achData20.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (adtData21.size () != 1)
        {
            cerr << "Error: adtData21.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aptrData22.size () != 1)
        {
            cerr << "Error: aptrData22.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (achptrData23.size () != 1)
        {
            cerr << "Error: achptrData23.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (aData24.size () != 0)
        {
            cerr << "Error: aData24.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }

#ifdef PEGASUS_CMPI_DATA_NEED_IMPLICIT_CONVERTERS
        // Test CmpiArray operator[] and CmpiArrayIdx operatorXXX
        abData1[0]      = bData1;
        achData2[0]     = chData1;
        ar32Data3[0]    = r32Data1;
        ar64Data4[0]    = r64Data1;
        aui8Data5[0]    = ui8Data1;
        aui16Data6[0]   = ui16Data1;
        aui32Data7[0]   = ui32Data1;
        aui64Data8[0]   = ui64Data1;
        asi8Data9[0]    = si8Data1;
        asi16Data10[0]  = si16Data1;
        asi32Data11[0]  = si32Data1;
        asi64Data12[0]  = si64Data1;
        ainstData13[0]  = ciData1;
////////arefData14[0]   =
////////aargData15[0]   =
////////aclaData16[0]   =
////////afilData17[0]   =
////////aenuData18[0]   =
        astrData19[0]   = strData1;
        achData20[0]    = achData1;
        adtData21[0]    = dtData1;
////////aptrData22[0]   =
////////achptrData23[0] =
        try
        {
           aData24[0] = abData1;

           cerr << "Error: Set array out of bounds (#1) succeeded!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        catch (const CmpiStatus& e)
        {
           if (e.rc () != CMPI_RC_ERR_FAILED)
           {
              cerr << "Error: Set array out of bounds (#1) expecting "
                   << "CMPI_RC_ERR_FAILED"
                   << endl;
              return CmpiStatus (CMPI_RC_ERR_FAILED);
           }
        }
        try
        {
           abData1[1] = abData1;

           cerr << "Error: Set array out of bounds (#2) succeeded!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        catch (const CmpiStatus& e)
        {
           if (e.rc () != CMPI_RC_ERR_FAILED)
           {
              cerr << "Error: Set array out of bounds (#2) expecting "
                   << "CMPI_RC_ERR_FAILED"
                   << endl;
              return CmpiStatus (CMPI_RC_ERR_FAILED);
           }
        }
#endif

        // Test CmpiArrayIdx getData
        d = abData1[0].getData ();

        // Test CmpiArray operator[] and CmpiArrayIdx getXXX
        abData1[0].getBoolean ();
        asi8Data9[0].getSint8 ();
        aui8Data5[0].getUint8 ();
        asi16Data10[0].getSint16 ();
        aui16Data6[0].getUint16 ();
        asi32Data11[0].getSint32 ();
        aui32Data7[0].getUint32 ();
        asi64Data12[0].getSint64 ();
        aui64Data8[0].getUint64 ();
        ar32Data3[0].getReal32 ();
        ar64Data4[0].getReal64 ();
        achData2[0].getChar16 ();
        astrData19[0].getString ();
        achData20[0].getCString ();
        adtData21[0].getDateTime ();
        ainstData13[0].getInstance ();
        arefData14[0].getObjectPath ();

        // Test CmpiArray operator[] and CmpiArrayIdx setXXX
        // @TBD

        // Test CmpiBooleanData assignment
        CmpiBooleanData bdData1 (true);
        CmpiBooleanData bdData2 (false);
        CmpiBooleanData bdData (true);

        // Test CmpiBooleanData assignment
        bdData = true;
        bdData = false;
        bdData = CmpiTrue;
        bdData = CmpiFalse;

        // Test CmpiData constructors
        {
           CmpiData d1;
#ifdef PEGASUS_CMPI_PROVIDE_BOOLEAN_CHAR16
           CmpiData d2 (bData1);
           CmpiData d3 (chData1);
#endif
           CmpiData d4 (si8Data1);
           CmpiData d5 (si16Data1);
           CmpiData d6 (si32Data1);
           CmpiData d7 (si64Data1);
           CmpiData d8 (ui8Data1);
           CmpiData d9 (ui16Data1);
           CmpiData d10 (ui32Data1);
           CmpiData d11 (ui64Data1);
           CmpiData d12 (r32Data1);
           CmpiData d13 (r64Data1);
           CmpiData d14 (strData1);
           CmpiData d15 (achData1);
           CmpiData d16 (copData1);
           CmpiData d17 (ciData1);
           CmpiData d18 (dtData1);
           CmpiData d19 (abData1);
           CmpiData d20 (abData1);
           CmpiData d21 (achData2);
           CmpiData d22 (ar32Data3);
           CmpiData d23 (ar64Data4);
           CmpiData d24 (aui8Data5);
           CmpiData d25 (aui16Data6);
           CmpiData d26 (aui32Data7);
           CmpiData d27 (aui64Data8);
           CmpiData d38 (asi8Data9);
           CmpiData d39 (asi16Data10);
           CmpiData d30 (asi32Data11);
           CmpiData d31 (asi64Data12);
           CmpiData d32 (ainstData13);
           CmpiData d33 (arefData14);
           CmpiData d34 (aargData15);
           CmpiData d35 (aclaData16);
           CmpiData d36 (afilData17);
           CmpiData d37 (aenuData18);
           CmpiData d48 (astrData19);
           CmpiData d49 (achData20);
           CmpiData d40 (adtData21);
           CmpiData d41 (aptrData22);
           CmpiData d42 (achptrData23);
           CmpiData d43 (aData24);
        }

        // Test CmpiData operator=
        d = bData1;
        d = chData1;
        d = ui8Data1;
        d = ui16Data1;
        d = ui32Data1;
        d = ui64Data1;
        d = si8Data1;
        d = si16Data1;
        d = si32Data1;
        d = si64Data1;
        d = r32Data1;
        d = r64Data1;
        d = strData1;
        d = achData1;
        d = dtData1;
        d = copData1;
        d = ciData1;
        d = d;

#ifdef PEGASUS_CMPI_DATA_NEED_IMPLICIT_CONVERTERS
        // Test CmpiData operatorXXX
#ifdef PEGASUS_CMPI_PROVIDE_BOOLEAN_CHAR16
        d = bData1;    (CMPIBoolean)d;
        d = chData1;   (CMPIChar16)d;
#endif
        d = ui8Data1;  (CMPIUint8)d;
        d = ui16Data1; (CMPIUint16)d;
        d = ui32Data1; (CMPIUint32)d;
        d = ui64Data1; (CMPIUint64)d;
        d = si8Data1;  (CMPISint8)d;
        d = si16Data1; (CMPISint16)d;
        d = si32Data1; (CMPISint32)d;
        d = si64Data1; (CMPISint64)d;
        d = r32Data1;  (CMPIReal32)d;
        d = r64Data1;  (CMPIReal64)d;
        d = strData1;  (CmpiString&)d;
        d = achData1;  (const char *)d;
        d = dtData1;   (CmpiDateTime&)d;
        d = copData1;  (CmpiObjectPath&)d;
        d = ciData1;   (CmpiInstance&)d;
#endif

        // Test CmpiData getXXX
        d = bData1;    PEGASUS_TEST_ASSERT (d.getBoolean () == bData1);
        d = chData1;   PEGASUS_TEST_ASSERT (d.getChar16 () == chData1);
        d = ui8Data1;  PEGASUS_TEST_ASSERT (d.getUint8 () == ui8Data1);
        d = ui16Data1; PEGASUS_TEST_ASSERT (d.getUint16 () == ui16Data1);
        d = ui32Data1; PEGASUS_TEST_ASSERT (d.getUint32 () == ui32Data1);
        d = ui64Data1; PEGASUS_TEST_ASSERT (d.getUint64 () == ui64Data1);
        d = si8Data1;  PEGASUS_TEST_ASSERT (d.getSint8 () == si8Data1);
        d = si16Data1; PEGASUS_TEST_ASSERT (d.getSint16 () == si16Data1);
        d = si32Data1; PEGASUS_TEST_ASSERT (d.getSint32 () == si32Data1);
        d = si64Data1; PEGASUS_TEST_ASSERT (d.getSint64 () == si64Data1);
        d = r32Data1;  PEGASUS_TEST_ASSERT (d.getReal32 () == r32Data1);
        d = r64Data1;  PEGASUS_TEST_ASSERT (d.getReal64 () == r64Data1);
        d = strData1;//PEGASUS_TEST_ASSERT (d.getString () == strData1);
        d = achData1;//PEGASUS_TEST_ASSERT (d.getCString () == achData1);
        d = dtData1; //PEGASUS_TEST_ASSERT (d.getDateTime () == dtData1);
        d = copData1;//PEGASUS_TEST_ASSERT (d.getObjectPath () == copData1);
        d = ciData1; //PEGASUS_TEST_ASSERT (d.getInstance () == ciData1);

        // Test CmpiData setXXX
        d.setBoolean (bData1);
        d.setChar16 (chData1);
        d.setUint8 (ui8Data1);
        d.setUint16 (ui16Data1);
        d.setUint32 (ui32Data1);
        d.setUint64 (ui64Data1);
        d.setSint8 (si8Data1);
        d.setSint16 (si16Data1);
        d.setSint32 (si32Data1);
        d.setSint64 (si64Data1);
        d.setReal32 (r32Data1);
        d.setReal64 (r64Data1);
        d.setString (strData1);
        d.setCString (achData1);
        d.setDateTime (dtData1);
        d.setObjectPath (copData1);
        d.setInstance (ciData1);

        // Test CmpiData isNullValue
        CmpiData dNull;

        if (!dNull.isNullValue ())
        {
           cerr << "Error: CmpiData.isNullValue (#1) failed!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (d.isNullValue ())
        {
           cerr << "Error: CmpiData.isNullValue (#1) failed!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }

        // Test CmpiData isNotFound
        // @TBD

        // Test CmpiDateTime
        CmpiDateTime dtData2;
        CmpiDateTime dtData3 (dtData2);
        CmpiDateTime dtData4 ("20070501152143.164592-300");
        // echo `date --date='2007-05-01 15:21:43' +%s`*1000000+164592 | bc
        // 1178050903164592LL (which is wrong.  off by an hour.)
        CmpiDateTime dtData5 (1178047303164592LL, false);
        CmpiDateTime dtData6 ("00000011125959.123456:000");
        CmpiDateTime dtData7 (997199123456LL, true);

#ifdef PEGASUS_DEBUG
        cout << "Comparing dtData2 to dtData3" << endl;
#endif
        PEGASUS_TEST_ASSERT (dtData2.getDateTime () == dtData3.getDateTime ());
#ifdef PEGASUS_DEBUG
        cout << "Comparing dtData4 to dtData5" << endl;
#endif
        PEGASUS_TEST_ASSERT (dtData4.getDateTime () == dtData5.getDateTime ());
#ifdef PEGASUS_DEBUG
        cout << "Comparing dtData6 to dtData7" << endl;
#endif
        PEGASUS_TEST_ASSERT (dtData6.getDateTime () == dtData7.getDateTime ());

        PEGASUS_TEST_ASSERT (!dtData4.isInterval ());
        PEGASUS_TEST_ASSERT (!dtData5.isInterval ());
        PEGASUS_TEST_ASSERT (dtData6.isInterval ());
        PEGASUS_TEST_ASSERT (dtData7.isInterval ());

#ifdef PEGASUS_DEBUG
////////cout << "dtData2 = " << convertTime (dtData2) << endl;
////////cout << "dtData3 = " << convertTime (dtData3) << endl;
////////cout << "dtData4 = " << convertTime (dtData4) << endl;
////////cout << "dtData5 = " << convertTime (dtData5) << endl;
////////cout << "dtData2 = " << dtData2.getDateTime () << endl;
////////cout << "dtData3 = " << dtData3.getDateTime () << endl;
////////cout << "dtData4 = " << dtData4.getDateTime () << endl;
////////cout << "dtData5 = " << dtData5.getDateTime () << endl;
////////cout << "dtData6 = " << dtData6.getDateTime () << endl;
#endif

        // Test CmpiInstance
#if 0
        copData1.setKey ("CSCreationClassName",
                         "CIM_UnitaryComputerSystem");
        copData1.setKey ("CSName",
                         "localhost");
        copData1.setKey ("FSCreationClassName",
                         "CIM_FileSystem");
        copData1.setKey ("FSName",
                         "/Simulated/CMPI/tests/");
        copData1.setKey ("Name",
                         "/Simulated/CMPI/tests/Providers/CWS_Directory.c");
#endif

        CmpiObjectPath copData2 ("root/PG_InterOp", "CIM_NameSpace");
//      copData2.setNameSpace ("root/PG_InterOp");
//      copData2.setClassName ("CIM_NameSpace");

        CmpiEnumeration enumer;

        enumer = cppBroker.enumInstances (ctx,
                                          copData1,
                                          0);

        if (enumer.hasNext ())
        {
           CmpiInstance inst = enumer.getNext ();

           printInstance (inst);
        }

        enumer = cppBroker.enumInstanceNames (ctx,
                                              copData1);

        if (enumer.hasNext ())
        {
           CmpiObjectPath cop = enumer.getNext ();

           printObjectPath (cop);

           CmpiInstance inst = cppBroker.getInstance (ctx,
                                                      cop,
                                                      0);

           printInstance (inst);
        }

        // Test CmpiString
        CmpiString string1 ("abcdef");
        CmpiString string2 ("aBcdef");
        CmpiString string3 ("abcdef");
        CmpiString string4 ("fedcba");
        CmpiString string5;

        if (  string1.equals (string2)
           || !string1.equals (string3)
           || string1.equals ("aBcdef")
           || !string1.equals ("abcdef")
           )
        {
           cerr << "Error: String.equals (#1) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (  !string1.equalsIgnoreCase (string2)
           || string1.equalsIgnoreCase (string4)
           || !string1.equalsIgnoreCase ("aBcdef")
           || string1.equalsIgnoreCase ("fdecba")
           )
        {
           cerr << "Error: String.equals (#2) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        if (string5.charPtr ())
        {
           cerr << "Error: String.charPtr (#1) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
    }
    catch (const CmpiStatus& e)
    {
#ifdef PEGASUS_DEBUG
        cout << "TestCMPI_CXX::initialize: Caught exception. rc = "
             << e.rc ();
        if (e.msg ())
        {
           cout << "msg = \"" << e.msg () << "\"";
        }
        cout << endl;
#endif

        return e;
    }

#ifdef PEGASUS_DEBUG
    cout << "TestCMPI_CXX::initialize () exit" << endl;
#endif

    return CmpiStatus (CMPI_RC_OK);
}

CmpiStatus
TestCMPI_CXX::enumInstanceNames (const CmpiContext&    ctx,
                                 CmpiResult&           rslt,
                                 const CmpiObjectPath& cop)
{
    return CmpiStatus (CMPI_RC_OK);
}

CmpiStatus
TestCMPI_CXX::enumInstances (const CmpiContext&     ctx,
                             CmpiResult&            rslt,
                             const CmpiObjectPath&  cop,
                             const char*           *properties)
{
   return CmpiStatus (CMPI_RC_ERR_NOT_SUPPORTED, "TestCMPI_CXX: cannot enumInstances");
}

CmpiStatus 
TestCMPI_CXX::getInstance (const CmpiContext&     ctx,
                           CmpiResult&            rslt,
                           const CmpiObjectPath&  cop,
                           const char*           *properties)
{
    return CmpiStatus (CMPI_RC_ERR_NOT_SUPPORTED, "TestCMPI_CXX: cannot getInstance");
}

CmpiStatus
TestCMPI_CXX::setInstance (const CmpiContext&     ctx,
				               CmpiResult&            rslt,
                           const CmpiObjectPath&  cop,
                           const CmpiInstance&    inst,
                           const char*           *properties)
{
    return CmpiStatus (CMPI_RC_ERR_NOT_SUPPORTED, "TestCMPI_CXX: cannot setInstance");
}

CmpiStatus
TestCMPI_CXX::createInstance (const CmpiContext&    ctx,
                              CmpiResult&           rslt,
					               const CmpiObjectPath& cop,
                              const CmpiInstance&   inst)
{
    return CmpiStatus (CMPI_RC_ERR_NOT_SUPPORTED, "TestCMPI_CXX: cannot createInstance");
}

CmpiStatus
TestCMPI_CXX::deleteInstance (const CmpiContext&    ctx,
                              CmpiResult&           rslt,
                              const CmpiObjectPath& cop)
{
    return CmpiStatus (CMPI_RC_ERR_NOT_SUPPORTED, "TestCMPI_CXX: cannot deleteInstance");
}
