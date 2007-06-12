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

// Please be aware that the CMPI C++ API is NOT a standard currently.

#include "TestCMPI_CXX.h"

#include <ctime>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Config.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

// This solves the problem of figuring out what line causes
// the exception.
#define L line = __LINE__
int line = 0;

// Microsoft Visual C++ version 6.0 cannot output 64 bit values
#if defined(_MSC_VER) && _MSC_VER < 1300
std::ostream& operator<<(std::ostream& os, CMPIUint64 i)
{
    char buf[65];
    sprintf (buf, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", i);
    os << buf;
    return os;
}
std::ostream& operator<<(std::ostream& os, CMPISint64 i)
{
    char buf[65];
    sprintf (buf, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", i);
    os << buf;
    return os;
}
#endif

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

#if defined(PEGASUS_OS_TYPE_WINDOWS)
   return ctime (&tTime);
#else
   return ctime_r (&tTime, timeBuffer);
#endif
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
      catch (...)
      {
         cerr << "Error: Caught unknown exception @ line "
              << __LINE__
              << ". Rethrowing."
              << endl;
         throw;
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
        L;CMPIBoolean    bData1     = true;
        L;CMPIChar16     chData1    = '1';
        L;CMPIUint8      ui8Data1   = 2;
        L;CMPIUint16     ui16Data1  = 3;
        L;CMPIUint32     ui32Data1  = 4;
        L;CMPIUint64     ui64Data1  = 5;
        L;CMPISint8      si8Data1   = 6;
        L;CMPISint16     si16Data1  = 7;
        L;CMPISint32     si32Data1  = 8;
        L;CMPISint64     si64Data1  = 9;
        L;CMPIReal32     r32Data1   = 10;
        L;CMPIReal64     r64Data1   = 11;
        L;CmpiString     strData1   = "12";
        L;const char     achData1[] = { '1', '3', '\0' };
        L;CmpiDateTime   dtData1;
        L;CmpiObjectPath copData1 ("root/SampleProvider", "CWS_PlainFile_CXX");
        L;CmpiInstance   ciData1 (copData1);
        L;CmpiData       d;

        // Test CmpiArray constructors
        L;CmpiArray      abData1 (1, CMPI_boolean);
        L;CmpiArray      achData2 (1, CMPI_char16);
        L;CmpiArray      ar32Data3 (1, CMPI_real32);
        L;CmpiArray      ar64Data4 (1, CMPI_real64);
        L;CmpiArray      aui8Data5 (1, CMPI_uint8);
        L;CmpiArray      aui16Data6 (1, CMPI_uint16);
        L;CmpiArray      aui32Data7 (1, CMPI_uint32);
        L;CmpiArray      aui64Data8 (1, CMPI_uint64);
        L;CmpiArray      asi8Data9 (1, CMPI_sint8);
        L;CmpiArray      asi16Data10 (1, CMPI_sint16);
        L;CmpiArray      asi32Data11 (1, CMPI_sint32);
        L;CmpiArray      asi64Data12 (1, CMPI_sint64);
        L;CmpiArray      ainstData13 (1, CMPI_instance);
        L;CmpiArray      arefData14 (1, CMPI_ref);
        L;CmpiArray      aargData15 (1, CMPI_args);
        L;CmpiArray      aclaData16 (1, CMPI_class);
        L;CmpiArray      afilData17 (1, CMPI_filter);
        L;CmpiArray      aenuData18 (1, CMPI_enumeration);
        L;CmpiArray      astrData19 (1, CMPI_string);
        L;CmpiArray      achData20 (1, CMPI_chars);
        L;CmpiArray      adtData21 (1, CMPI_dateTime);
        L;CmpiArray      aptrData22 (1, CMPI_ptr);
        L;CmpiArray      achptrData23 (1, CMPI_charsptr);
        L;CmpiArray      aData24;

        // Test CmpiArray size
        L;if (abData1.size () != 1)
        {
            cerr << "Error: abData1.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (achData2.size () != 1)
        {
            cerr << "Error: achData2.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (ar32Data3.size () != 1)
        {
            cerr << "Error: ar32Data3.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (ar64Data4.size () != 1)
        {
            cerr << "Error: ar64Data4.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aui8Data5.size () != 1)
        {
            cerr << "Error: aui8Data5.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aui16Data6.size () != 1)
        {
            cerr << "Error: aui16Data6.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aui32Data7.size () != 1)
        {
            cerr << "Error: aui32Data7.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aui64Data8.size () != 1)
        {
            cerr << "Error: aui64Data8.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (asi8Data9.size () != 1)
        {
            cerr << "Error: asi8Data9.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (asi16Data10.size () != 1)
        {
            cerr << "Error: asi16Data10.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (asi32Data11.size () != 1)
        {
            cerr << "Error: asi32Data11.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (asi64Data12.size () != 1)
        {
            cerr << "Error: asi64Data12.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (ainstData13.size () != 1)
        {
            cerr << "Error: ainstData13.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (arefData14.size () != 1)
        {
            cerr << "Error: arefData14.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aargData15.size () != 1)
        {
            cerr << "Error: aargData15.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aclaData16.size () != 1)
        {
            cerr << "Error: aclaData16.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (afilData17.size () != 1)
        {
            cerr << "Error: afilData17.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aenuData18.size () != 1)
        {
            cerr << "Error: aenuData18.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (astrData19.size () != 1)
        {
            cerr << "Error: astrData19.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (achData20.size () != 1)
        {
            cerr << "Error: achData20.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (adtData21.size () != 1)
        {
            cerr << "Error: adtData21.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aptrData22.size () != 1)
        {
            cerr << "Error: aptrData22.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (achptrData23.size () != 1)
        {
            cerr << "Error: achptrData23.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (aData24.size () != 0)
        {
            cerr << "Error: aData24.size () != 1" << endl;
            return CmpiStatus (CMPI_RC_ERR_FAILED);
        }

#ifdef PEGASUS_CMPI_DATA_NEED_IMPLICIT_CONVERTERS
        // Test CmpiArray operator[] and CmpiArrayIdx operatorXXX
        L;abData1[0]      = bData1;
        L;achData2[0]     = chData1;
        L;ar32Data3[0]    = r32Data1;
        L;ar64Data4[0]    = r64Data1;
        L;aui8Data5[0]    = ui8Data1;
        L;aui16Data6[0]   = ui16Data1;
        L;aui32Data7[0]   = ui32Data1;
        L;aui64Data8[0]   = ui64Data1;
        L;asi8Data9[0]    = si8Data1;
        L;asi16Data10[0]  = si16Data1;
        L;asi32Data11[0]  = si32Data1;
        L;asi64Data12[0]  = si64Data1;
        L;ainstData13[0]  = ciData1;
////////L;arefData14[0]   =
////////L;aargData15[0]   =
////////L;aclaData16[0]   =
////////L;afilData17[0]   =
////////L;aenuData18[0]   =
        L;astrData19[0]   = strData1;
        L;achData20[0]    = achData1;
        L;adtData21[0]    = dtData1;
////////L;aptrData22[0]   =
////////L;achptrData23[0] =
        try
        {
           L;aData24[0] = abData1;

           cerr << "Error: Set array out of bounds (#1) succeeded!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        catch (const CmpiStatus& e)
        {
           if (e.rc () != CMPI_RC_ERR_NO_SUCH_PROPERTY)
           {
              cerr << "Error: Set array out of bounds (#1) expecting "
                   << "CMPI_RC_ERR_NO_SUCH_PROPERTY"
                   << endl;
              return CmpiStatus (CMPI_RC_ERR_FAILED);
           }
        }
        catch (...)
        {
           cerr << "Error: Caught unknown exception @ line "
                << __LINE__
                << ". Rethrowing."
                << endl;
           throw;
        }
        try
        {
           L;abData1[1] = abData1;

           cerr << "Error: Set array out of bounds (#2) succeeded!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        catch (const CmpiStatus& e)
        {
           if (e.rc () != CMPI_RC_ERR_NO_SUCH_PROPERTY)
           {
              cerr << "Error: Set array out of bounds (#2) expecting "
                   << "CMPI_RC_ERR_NO_SUCH_PROPERTY"
                   << endl;
              throw CmpiStatus (CMPI_RC_ERR_FAILED);
           }
        }
        catch (...)
        {
           cerr << "Error: Caught unknown exception @ line "
                << __LINE__
                << ". Rethrowing."
                << endl;
           throw;
        }
#endif

        // Test CmpiArrayIdx getData
        L;d = abData1[0].getData ();

        // Test CmpiArray operator[] and CmpiArrayIdx getXXX
        L;abData1[0].getBoolean ();
        L;asi8Data9[0].getSint8 ();
        L;aui8Data5[0].getUint8 ();
        L;asi16Data10[0].getSint16 ();
        L;aui16Data6[0].getUint16 ();
        L;asi32Data11[0].getSint32 ();
        L;aui32Data7[0].getUint32 ();
        L;asi64Data12[0].getSint64 ();
        L;aui64Data8[0].getUint64 ();
        L;ar32Data3[0].getReal32 ();
        L;ar64Data4[0].getReal64 ();
        L;achData2[0].getChar16 ();
        L;astrData19[0].getString ();
        L;achData20[0].getCString ();
        L;adtData21[0].getDateTime ();
        L;ainstData13[0].getInstance ();
        L;arefData14[0].getObjectPath ();

        // Test CmpiArray operator[] and CmpiArrayIdx setXXX
        // @TBD

        // Test CmpiBooleanData assignment
        L;CmpiBooleanData bdData1 (true);
        L;CmpiBooleanData bdData2 (false);
        L;CmpiBooleanData bdData (true);

        // Test CmpiBooleanData assignment
        L;bdData = true;
        L;bdData = false;
        L;bdData = CmpiTrue;
        L;bdData = CmpiFalse;

        // Test CmpiData constructors
        {
           L;CmpiData d1;
#ifdef PEGASUS_CMPI_PROVIDE_BOOLEAN_CHAR16
           L;CmpiData d2 (bData1);
           L;CmpiData d3 (chData1);
#endif
           L;CmpiData d4 (si8Data1);
           L;CmpiData d5 (si16Data1);
           L;CmpiData d6 (si32Data1);
           L;CmpiData d7 (si64Data1);
           L;CmpiData d8 (ui8Data1);
           L;CmpiData d9 (ui16Data1);
           L;CmpiData d10 (ui32Data1);
           L;CmpiData d11 (ui64Data1);
           L;CmpiData d12 (r32Data1);
           L;CmpiData d13 (r64Data1);
           L;CmpiData d14 (strData1);
           L;CmpiData d15 (achData1);
           L;CmpiData d16 (copData1);
           L;CmpiData d17 (ciData1);
           L;CmpiData d18 (dtData1);
           L;CmpiData d19 (abData1);
           L;CmpiData d20 (abData1);
           L;CmpiData d21 (achData2);
           L;CmpiData d22 (ar32Data3);
           L;CmpiData d23 (ar64Data4);
           L;CmpiData d24 (aui8Data5);
           L;CmpiData d25 (aui16Data6);
           L;CmpiData d26 (aui32Data7);
           L;CmpiData d27 (aui64Data8);
           L;CmpiData d38 (asi8Data9);
           L;CmpiData d39 (asi16Data10);
           L;CmpiData d30 (asi32Data11);
           L;CmpiData d31 (asi64Data12);
           L;CmpiData d32 (ainstData13);
           L;CmpiData d33 (arefData14);
           L;CmpiData d34 (aargData15);
           L;CmpiData d35 (aclaData16);
           L;CmpiData d36 (afilData17);
           L;CmpiData d37 (aenuData18);
           L;CmpiData d48 (astrData19);
           L;CmpiData d49 (achData20);
           L;CmpiData d40 (adtData21);
           L;CmpiData d41 (aptrData22);
           L;CmpiData d42 (achptrData23);
           L;CmpiData d43 (aData24);
        }

        // Test CmpiData operator=
        L;d = bData1;
        L;d = chData1;
        L;d = ui8Data1;
        L;d = ui16Data1;
        L;d = ui32Data1;
        L;d = ui64Data1;
        L;d = si8Data1;
        L;d = si16Data1;
        L;d = si32Data1;
        L;d = si64Data1;
        L;d = r32Data1;
        L;d = r64Data1;
        L;d = strData1;
        L;d = achData1;
        L;d = dtData1;
        L;d = copData1;
        L;d = ciData1;
        L;d = d;

#ifdef PEGASUS_CMPI_DATA_NEED_IMPLICIT_CONVERTERS
        // Test CmpiData operatorXXX
#ifdef PEGASUS_CMPI_PROVIDE_BOOLEAN_CHAR16
        L;d = bData1;    (CMPIBoolean)d;
        L;d = chData1;   (CMPIChar16)d;
#endif
        L;d = ui8Data1;  (CMPIUint8)d;
        L;d = ui16Data1; (CMPIUint16)d;
        L;d = ui32Data1; (CMPIUint32)d;
        L;d = ui64Data1; (CMPIUint64)d;
        L;d = si8Data1;  (CMPISint8)d;
        L;d = si16Data1; (CMPISint16)d;
        L;d = si32Data1; (CMPISint32)d;
        L;d = si64Data1; (CMPISint64)d;
        L;d = r32Data1;  (CMPIReal32)d;
        L;d = r64Data1;  (CMPIReal64)d;
        L;d = strData1;  /* (CmpiString&)d; */
        L;d = achData1;  (const char *)d;
        L;d = dtData1;   /* (CmpiDateTime&)d; */
        L;d = copData1;  /* (CmpiObjectPath&)d; */
        L;d = ciData1;   /* (CmpiInstance&)d; */
#endif

        // Test CmpiData getXXX
        L;d = bData1;    PEGASUS_TEST_ASSERT (d.getBoolean () == bData1);
        L;d = chData1;   PEGASUS_TEST_ASSERT (d.getChar16 () == chData1);
        L;d = ui8Data1;  PEGASUS_TEST_ASSERT (d.getUint8 () == ui8Data1);
        L;d = ui16Data1; PEGASUS_TEST_ASSERT (d.getUint16 () == ui16Data1);
        L;d = ui32Data1; PEGASUS_TEST_ASSERT (d.getUint32 () == ui32Data1);
        L;d = ui64Data1; PEGASUS_TEST_ASSERT (d.getUint64 () == ui64Data1);
        L;d = si8Data1;  PEGASUS_TEST_ASSERT (d.getSint8 () == si8Data1);
        L;d = si16Data1; PEGASUS_TEST_ASSERT (d.getSint16 () == si16Data1);
        L;d = si32Data1; PEGASUS_TEST_ASSERT (d.getSint32 () == si32Data1);
        L;d = si64Data1; PEGASUS_TEST_ASSERT (d.getSint64 () == si64Data1);
        L;d = r32Data1;  PEGASUS_TEST_ASSERT (d.getReal32 () == r32Data1);
        L;d = r64Data1;  PEGASUS_TEST_ASSERT (d.getReal64 () == r64Data1);
        L;d = strData1;//PEGASUS_TEST_ASSERT (d.getString () == strData1);
        L;d = achData1;//PEGASUS_TEST_ASSERT (d.getCString () == achData1);
        L;d = dtData1; //PEGASUS_TEST_ASSERT (d.getDateTime () == dtData1);
        L;d = copData1;//PEGASUS_TEST_ASSERT (d.getObjectPath () == copData1);
        L;d = ciData1; //PEGASUS_TEST_ASSERT (d.getInstance () == ciData1);

        // Test CmpiData setXXX
        L;d.setBoolean (bData1);
        L;d.setChar16 (chData1);
        L;d.setUint8 (ui8Data1);
        L;d.setUint16 (ui16Data1);
        L;d.setUint32 (ui32Data1);
        L;d.setUint64 (ui64Data1);
        L;d.setSint8 (si8Data1);
        L;d.setSint16 (si16Data1);
        L;d.setSint32 (si32Data1);
        L;d.setSint64 (si64Data1);
        L;d.setReal32 (r32Data1);
        L;d.setReal64 (r64Data1);
        L;d.setString (strData1);
        L;d.setCString (achData1);
        L;d.setDateTime (dtData1);
        L;d.setObjectPath (copData1);
        L;d.setInstance (ciData1);

        // Test CmpiData isNullValue
        L;CmpiData dNull;

        L;if (!dNull.isNullValue ())
        {
           cerr << "Error: CmpiData.isNullValue (#1) failed!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (d.isNullValue ())
        {
           cerr << "Error: CmpiData.isNullValue (#1) failed!"
                << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }

        // Test CmpiData isNotFound
        // @TBD

        // Test CmpiDateTime
        L;CmpiDateTime dtData2;
        L;CmpiDateTime dtData3 (dtData2);
        L;CmpiDateTime dtData4 ("20070501152143.164592-300");
        // echo `date --date='2007-05-01 15:21:43' +%s`*1000000+164592 | bc
        // 1178050903164592LL
        L;CmpiDateTime dtData5 (PEGASUS_UINT64_LITERAL(1178050903164592), false);
        L;CmpiDateTime dtData6 ("00000011125959.123456:000");
        L;CmpiDateTime dtData7 (PEGASUS_UINT64_LITERAL(997199123456), true);

#ifdef PEGASUS_DEBUG
        L;cout << "dtData2 = " << dtData2.getDateTime () << endl;
        L;cout << "dtData3 = " << dtData3.getDateTime () << endl;
        L;cout << "dtData4 = " << dtData4.getDateTime () << endl;
        L;cout << "dtData5 = " << dtData5.getDateTime () << endl;
        L;cout << "dtData6 = " << dtData6.getDateTime () << endl;
        L;cout << "dtData7 = " << dtData7.getDateTime () << endl;
#endif

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
////////L;cout << "dtData2 = " << convertTime (dtData2) << endl;
////////L;cout << "dtData3 = " << convertTime (dtData3) << endl;
////////L;cout << "dtData4 = " << convertTime (dtData4) << endl;
////////L;cout << "dtData5 = " << convertTime (dtData5) << endl;
////////L;cout << "dtData6 = " << convertTime (dtData6) << endl;
////////L;cout << "dtData7 = " << convertTime (dtData7) << endl;
#endif

        // Test CmpiInstance
#if 0
        L;copData1.setKey ("CSCreationClassName",
                           "CIM_UnitaryComputerSystem");
        L;copData1.setKey ("CSName",
                           "localhost");
        L;copData1.setKey ("FSCreationClassName",
                           "CIM_FileSystem");
        L;copData1.setKey ("FSName",
                           "/Simulated/CMPI/tests/");
        L;copData1.setKey ("Name",
                           "/Simulated/CMPI/tests/Providers/CWS_Directory.c");
#endif

        L;CmpiObjectPath copData2 ("root/PG_InterOp", "CIM_NameSpace");
//      L;copData2.setNameSpace ("root/PG_InterOp");
//      L;copData2.setClassName ("CIM_NameSpace");

        L;CmpiEnumeration enumer;

        L;enumer = cppBroker.enumInstances (ctx,
                                            copData1,
                                            0);

        L;if (enumer.hasNext ())
        {
           CmpiInstance inst = enumer.getNext ();

           printInstance (inst);
        }

        L;enumer = cppBroker.enumInstanceNames (ctx,
                                                copData1);

        L;if (enumer.hasNext ())
        {
           L;CmpiObjectPath cop = enumer.getNext ();

           L;printObjectPath (cop);

           L;CmpiInstance inst = cppBroker.getInstance (ctx,
                                                        cop,
                                                        0);

           L;printInstance (inst);
        }

        // Test CmpiString
        L;CmpiString string1 ("abcdef");
        L;CmpiString string2 ("aBcdef");
        L;CmpiString string3 ("abcdef");
        L;CmpiString string4 ("fedcba");
        L;CmpiString string5;

        L;if (  string1.equals (string2)
           || !string1.equals (string3)
           || string1.equals ("aBcdef")
           || !string1.equals ("abcdef")
           )
        {
           cerr << "Error: String.equals (#1) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (  !string1.equalsIgnoreCase (string2)
           || string1.equalsIgnoreCase (string4)
           || !string1.equalsIgnoreCase ("aBcdef")
           || string1.equalsIgnoreCase ("fdecba")
           )
        {
           cerr << "Error: String.equals (#2) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
        L;if (string5.charPtr ())
        {
           cerr << "Error: String.charPtr (#1) failed!" << endl;
           return CmpiStatus (CMPI_RC_ERR_FAILED);
        }
    }
    catch (const CmpiStatus& e)
    {
        static char achMessage[512];

#ifdef PEGASUS_DEBUG
        cout << "TestCMPI_CXX::initialize: Caught exception. rc = "
             << e.rc ();
        if (e.msg ())
        {
           cout << "msg = \"" << e.msg () << "\"";
        }
        cout << ", line # " << line << endl;
#endif

        sprintf (achMessage,
                 "TestCMPI_CXX::initialize: Caught exception. rc = %d line = %d",
                 e.rc (),
                 line);

        return CmpiStatus (e.rc (), achMessage);
    }
    catch (...)
    {
       cerr << "Error: Caught unknown exception @ line "
            << __LINE__
            << ". Rethrowing."
            << endl;
       throw;
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
