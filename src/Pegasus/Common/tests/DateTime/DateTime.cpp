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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//              (sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Willis White (PEP 192)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
    Boolean bad = false;
    try
    {
        //ATTN-P2-KS 20 Mar 2002 - Needs expansion of tests.
        // get the output display flag.
        Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;
        
        CIMDateTime dt;
        dt.set("19991224120000.000000+360");
        
        dt.clear();
        assert (dt.equal (CIMDateTime ("00000000000000.000000:000")));
    
        {
            bad = false;

            try
            {
                dt.set("too short");
            }
            catch (InvalidDateTimeFormatException&)
            {
                bad = true;
            }

            assert(bad);
        }
    
        {
            Boolean bad = false;
    
            try
            {
                dt.set("too short");
                dt.set("19990132120000.000000+360");
            }
            catch (InvalidDateTimeFormatException&)
            {
                bad = true;
            }

            assert(bad);
        }
        if (verbose)
    	cout << dt << endl;
    
        CIMDateTime dt1;
        dt1 = dt;


    /****************************************************************
              testing function added in PEP 192
    *******************************************************************/

          
        Boolean te1 = false;

     
         CIMDateTime te;
         try{
         te.set("2000122412****.002000+360");  //this is not leagal
         }
         catch(Exception & e){
             te1 = true;
         } 
         assert(te1);
         te1 =false;

        
         String stri = "20001224122***.******+360";
        try{
            CIMDateTime st(stri); 
                     }
        catch (InvalidDateTimeFormatException& r){
            te1 = true;
         }
        assert(te1);
        te1 =false;

                        /* makes sure the fields has splats starting 
                        form least significant place and going up
                        */
        String str_test2 = "20001224120000.002*00+360";
       try{
           CIMDateTime cdt_test2(str_test2); 
                    }
       catch (InvalidDateTimeFormatException& r){
           te1 = true;
        }
       assert(te1);
       te1 = false;
       
        try{
            CIMDateTime stt("200012*4120000.002000+360");
        }
        catch (InvalidDateTimeFormatException& r){
            te1 = true;
        }
        assert(te1);
        te1 = false;

         
                                /* check for UTC*/
         String str_test1 = "20001012010920.002000+3*0";
         try{
             CIMDateTime cdt_test1(str_test1); 
         }
         catch (InvalidDateTimeFormatException& r){
             te1 = true;
          }
         assert(te1);
        

                            /*  check days and UTC field when object is an interval*/  
      
          String str_test3 = "20001012010920.002000:000";
          try{
              CIMDateTime cdt_test3(str_test3); 
          }
          catch (InvalidDateTimeFormatException& r){
            te1 = false;
           }
          assert(te1);
          te1 = false;

    
          String str_test4 = "20001012010920.002000:100";
          try{
              CIMDateTime cdt_test4(str_test4); 
          }
          catch (InvalidDateTimeFormatException& r){
              te1 = true;
           }
          assert(te1);
          te1 = false;

          // Check for invalid non-ASCII character
          try
          {
              String str = "20001012010920.002000:000";
              str[7] = Char16(0x0132);
              CIMDateTime cdt(str); 
          }
          catch (const InvalidDateTimeFormatException&)
          {
              te1 = true;
          }
          assert(te1);
          te1 = false;

                               /* verify CIMDateTime::CIMDateTime(const Uint64 microSec, Boolena) 
                               and toMicroSeconds()*/ 
         try{
            int out_loop =1;
            Uint64 co = 1;
            while (co <20) {
                Uint64 num_day =   PEGASUS_UINT64_LITERAL(86400000000)*co;
                CIMDateTime cdt1 = CIMDateTime(num_day, true);

                if (cdt1.toMicroSeconds() != (PEGASUS_UINT64_LITERAL(86400000000)*co)){     
                    assert(false);
                }
                co = co +2;
            }
         }
         catch(Exception & e){
            assert(false);
        }
        catch(exception &  e){
            assert(false);
        }


   
        CIMDateTime cdt1 = CIMDateTime("00000001000000.123456:000");
        Uint64 re1 = cdt1.toMicroSeconds();
        if ((re1 != PEGASUS_UINT64_LITERAL(86400123456))) {
            assert(false);                                                 
        }
                                                                     

        CIMDateTime cdt2 = CIMDateTime("00000101000000.123456+000");
        Uint64 re2 = cdt2.toMicroSeconds();
        if ((re2 != 123456)) {
            assert(false);
        }

        CIMDateTime cdt3 = CIMDateTime("00000000030000.123456:000");
        Uint64 re3 = cdt3.toMicroSeconds();
        Uint64 comp = (PEGASUS_UINT64_LITERAL(3600000000)*3)+123456;
        if ((comp != re3)) {
            assert(false);                                              
        }

        /*CIMDateTime con1 = CIMDateTime(864003660000000,false);
        //CIMDateTime con_comp = CIMDateTime("00270519010100.000000+000");
        //if (!con1.equal(con_comp)) {
            assert(false);
        }
        const Uint64 conr1 = con1.toMicroSeconds();
        if (conr1 != 864003660000000) {
            assert(false);
        }  */   

      
        Uint64 real =  PEGASUS_UINT64_LITERAL(8640000000000000);
        Uint64 fake =  PEGASUS_UINT64_LITERAL(8637408000000000);

        CIMDateTime realC = CIMDateTime(real, false);
        CIMDateTime fakeC = CIMDateTime(fake, false);

        Uint64 realN = realC.toMicroSeconds();
        if (real != realN) {
            assert(false);
        }

        Uint64 fakeN = fakeC.toMicroSeconds();
        if (fake != fakeN) {
            assert(false);
        }

                
        CIMDateTime date1 = CIMDateTime("20041014002133.123456+000");
        Uint64 mic1 = date1.toMicroSeconds();
        CIMDateTime result1 = CIMDateTime(mic1,false);
        if (!date1.equal(result1)) {
            cout << result1.toString() << " was returned for constructor and should equal " << date1.toString()  << endl;
            assert(false);
        }
        
         
        CIMDateTime date2 = CIMDateTime("20000714000233.123456+000");
        Uint64 mic2 = date2.toMicroSeconds();
        CIMDateTime result2 = CIMDateTime(mic2,false);
        if (!date2.equal(result2)) {
            cout << result2.toString() << " was returned for constructor and should equal " << date2.toString()  << endl;
            assert(false);
        }

   
        CIMDateTime date3 = CIMDateTime("90001014100000.000456+000");
        Uint64 mic3 = date3.toMicroSeconds();
        CIMDateTime result3 = CIMDateTime(mic3,false); 
        if (!date3.equal(result3)) {
            cout << result3.toString() << " was returned for constructor and should equal " << date3.toString()  << endl;
            assert(false);
        }

 
        CIMDateTime date4 = CIMDateTime("01234567123456.123456:000");
        Uint64 mic4 = date4.toMicroSeconds();
        CIMDateTime result4 = CIMDateTime(mic4,true);
        if (!date4.equal(result4)) {
            cout << result4.toString() << " was returned for constructor and should equal " << date4.toString()  << endl;
            assert(false);
        }        
      

        CIMDateTime date5 = CIMDateTime("81234567123456.123456:000");
        Uint64 mic5 = date5.toMicroSeconds();
        CIMDateTime result5 = CIMDateTime(mic5,true);   
        if (!date5.equal(result5)) {
            cout << result5.toString() << " was returned for constructor and should equal " << date5.toString()  << endl;
            assert(false);
        }  
                               

        CIMDateTime date6 = CIMDateTime("00000000123456.123456:000");
        Uint64 mic6 = date6.toMicroSeconds();
        CIMDateTime result6 = CIMDateTime(mic6,true); 
        if (!date6.equal(result6)) {
            cout << result6.toString() << " was returned for constructor and should equal " << date6.toString()  << endl;
            assert(false);
        }

                                                                                                    
                                   // testing toMicroSeconds when object has wild cards

        CIMDateTime tms_cdt1 = CIMDateTime("00000000000000.12****:000");
        Uint64 ms1 = tms_cdt1.toMicroSeconds();
        if (ms1 != 120000) {
            cout << "problem converting " << tms_cdt1.toString() << " to MicroSeconds" << endl; 
            assert(false);
        }

        CIMDateTime tms_cdt2 = CIMDateTime("00000101000000.1234**+000");
        Uint64 ms2 = tms_cdt2.toMicroSeconds();
        if (ms2 != 123400) {
            cout << "problem converting " << tms_cdt2.toString() << " to MicroSeconds" << endl;
            assert(false);
        }

        CIMDateTime tms_cdt3 = CIMDateTime("000000000001**.******:000");
        Uint64 ms3 = tms_cdt3.toMicroSeconds();
        if (ms3 != 60000000) {
            cout << "problem converting " << tms_cdt3.toString() << " to MicroSeconds" << endl; 
            assert(false);
        }

        CIMDateTime tms_cdt4 = CIMDateTime("**************.******+000");
        Uint64 ms4 = tms_cdt4.toMicroSeconds();
        if (ms4 != PEGASUS_UINT64_LITERAL(0)) {
            cout << "problem converting " << tms_cdt4.toString() << " to MicroSeconds" << endl; 
            assert(false);
        }

                            /* verifying CIMDateTime::operator= */

        
         CIMDateTime org_vCo1 = CIMDateTime("**************.******+000");
         CIMDateTime se_vCo1 = CIMDateTime("99931231230200.000000-127");
         org_vCo1 = se_vCo1;
         if (String::compare(org_vCo1.toString(), se_vCo1.toString())) {
             assert(false);
         }

         CIMDateTime org_vCo2 = CIMDateTime("12340101010101.******+000");
         CIMDateTime se_vCo2 = CIMDateTime("00001231230200.000000-127");
         org_vCo2 = se_vCo2;
         if (String::compare(org_vCo2.toString(), se_vCo2.toString())) {
             assert(false);
         }



                                  /*testing equals() */

         if (!org_vCo1.equal(se_vCo1)) {
             assert(false);         }

    
         if (!org_vCo2.equal(se_vCo2)) {
             assert(false);
         }

         CIMDateTime teeq3 = CIMDateTime("12340101010101.******+000");
         CIMDateTime teeq3_comp = CIMDateTime("12340101010101.123456+000");
         if (!teeq3.equal(teeq3_comp)) {
             assert(false);
         }

         CIMDateTime teeq4 = CIMDateTime("12340101010101.212345+000");
         CIMDateTime teeq4_comp = CIMDateTime("12340101010101.2*****+000");
         if (!teeq4.equal(teeq4_comp)) {
             assert(false);
         }    

         CIMDateTime teeq5 = CIMDateTime("12340101010101.2*****+000");
         CIMDateTime teeq5_comp = CIMDateTime("12340101010101.2234**+000");
         if (!teeq5.equal(teeq5_comp)) {
             assert(false);
         }  

         CIMDateTime teeq6 = CIMDateTime("12340101010101.2*****+000");
         CIMDateTime teeq6_comp = CIMDateTime("123401010101**.******+000");
         if (!teeq6.equal(teeq6_comp)) {
             assert(false);
         }  

         CIMDateTime teeq7 = CIMDateTime("12340101010101.2*****+000");
         CIMDateTime teeq7_comp = CIMDateTime("1234010101****.******+030");
         if (!teeq7.equal(teeq7_comp)) {
             assert(false);
         }  

                               /*verfiying function put into getDifference in PEP 192*/ 

      CIMDateTime vgD1 = CIMDateTime("20040520041401.000000+000");
      CIMDateTime vgDsub1 = CIMDateTime("20040520041400.000000+000");
      Sint64 diff1 = CIMDateTime::getDifference(vgDsub1,vgD1);
      Sint64 vcom = 1000000;
      Sint32 vdiv = diff1/vcom;
      Sint32 rvdiv = diff1%vcom;
      if (diff1 != vcom) {
          cout << "subtraction of " << vgDsub1.toString() << " form " \
               << vgD1.toString() << " should have equaled 1000000" << endl;
          assert(false);
      }

    
      CIMDateTime vgD2 = CIMDateTime("20040521041400.00****+050");
      CIMDateTime vgDsub2 = CIMDateTime("20040520041400.000000+050");
      Sint64 diff2 = CIMDateTime::getDifference(vgDsub2,vgD2);
      Sint64 vcom2 = PEGASUS_UINT64_LITERAL(86400000000);
      Sint32 vdiv2 = diff2/1000000;
      Sint32 rvdiv2 = diff2%1000000;
      if (diff2 != vcom2) {
          cout << "subtraction of " << vgDsub2.toString() << " form " \
              << vgD2.toString() << " should have equaled 86400000000" << endl;
          assert(false);
      }


      CIMDateTime vgD3 = CIMDateTime("20040520041400.000000+120");
      CIMDateTime vgDsub3 = CIMDateTime("20040520041400.000***+000");
      Sint64 diff3 = CIMDateTime::getDifference(vgDsub3,vgD3);
      Sint64 vcom3 = -PEGASUS_SINT64_LITERAL(7200000000);
      Sint32 vdiv3 = diff3/1000000;
      Sint32 rvdiv3 = diff3%1000000;
      if (diff3 != vcom3) {
          cout << "subtraction of " << vgDsub2.toString() << " form " \
              << vgD2.toString() << " is wrong" << endl;
          assert(false);
      }

            
        //
        // Tests for getCurrentDateTime and getDifference.
        //
        CIMDateTime         startTime, finishTime;
        Sint64              differenceInMicroseconds;
    
        //
        // Call getCurrentDateTime
        //
        //startTime = CIMDateTime::getCurrentDateTime();
        //finishTime = CIMDateTime::getCurrentDateTime();
    
        // Set the start and finish times
        startTime.set("20020507170000.000000-480");
        finishTime.set("20020507170000.000000-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            -PEGASUS_SINT64_LITERAL(10800000000));
    
        //
        //  Test date difference with microseconds
        //
        // Set the start and finish times
        startTime.clear ();
        finishTime.clear ();
        finishTime.set ("20020507170000.000003-480");
        startTime.set ("20020507170000.000000-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime, 
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10800000003));
    
        // Set the start and finish times
        startTime.clear ();
        finishTime.clear ();
        finishTime.set ("20020507170000.000000-480");
        startTime.set ("20020507170000.000003-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime, 
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10799999997));
    
        // Set the start and finish times
        startTime.clear();
        finishTime.clear();
        finishTime.set("20020507170000.000000-480");
        startTime.set("20020507170000.000000-300");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(10800000000));
    
        // Set the start and finish times
        startTime.clear();
        startTime.set("20020507170000.000000+330");
        finishTime.clear(); 
        finishTime.set("20020507170000.000000-480");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(48600000000));
    
        // Set the start and finish times
        startTime.clear();
        finishTime.clear(); 
        finishTime.set("20020507170000.000000+330");
        startTime.set("20020507170000.000000-480");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            -PEGASUS_SINT64_LITERAL(48600000000));
    
        //
        //  ATTN-CAKG-P2-20020819: the following test currently returns an 
        //  incorrect result on Windows
        //
        //
        // Set the start and finish times
        //
        startTime.set ("19011214000000.000000-000");
        finishTime.set("19011215000000.000000-000");
    
        //
        // Call getDifference
        //
        try 
        {
            differenceInMicroseconds = CIMDateTime::getDifference (startTime,
                finishTime);
        }
        catch(const DateTimeOutOfRangeException&)
        {
        }
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
            char buffer [32];
            sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                differenceInMicroseconds);
            cout << "differenceInMicroseconds : " << buffer << endl;
        }
    
            
        //
        // Set the start and finish times
        //
        startTime.set ("20370101000000.000000-000");
        finishTime.set("20370102000000.000000-000");
    
        //
        // Call getDifference
        //
        differenceInMicroseconds = CIMDateTime::getDifference (startTime,
            finishTime);
    
        if (verbose)
        {
            cout << "Format              : yyyymmddhhmmss.mmmmmmsutc" << endl;
            cout << "Start date time is  : " << startTime << endl;
            cout << "Finish date time is : " << finishTime << endl;
        }
    
        assert (differenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(86400000000));

    
        // Check for interval
        CIMDateTime 	 startInterval;
        CIMDateTime		 finishInterval;
        Sint64      	 intervalDifferenceInMicroseconds;
    
        startInterval.set  ("00000001010100.000000:000");
        finishInterval.set ("00000001010200.000000:000");

    
        if (verbose)
        {
            cout << "Format              : ddddddddhhmmss.mmmmmm:000" << endl;
            cout << "Start interval is   : " << startInterval << endl;
            cout << "Finish interval is  : " << finishInterval << endl;
        }
    
        intervalDifferenceInMicroseconds = CIMDateTime::getDifference
                                         (startInterval, finishInterval);
    
        assert ( startInterval.isInterval() == true );
        assert ( intervalDifferenceInMicroseconds == 60000000 );

    
        //
        //  Test maximum interval difference
        //
        startInterval.set  ("00000000000000.000000:000");
        finishInterval.set ("99999999235959.999999:000");
    
        if (verbose)
        {
            cout << "Format              : ddddddddhhmmss.mmmmmm:000" << endl;
            cout << "Start interval is   : " << startInterval << endl;
            cout << "Finish interval is  : " << finishInterval << endl;
        }

    
        intervalDifferenceInMicroseconds = CIMDateTime::getDifference
                                         (startInterval, finishInterval);
    
        assert ( startInterval.isInterval() == true );
        assert ( intervalDifferenceInMicroseconds == 
            PEGASUS_SINT64_LITERAL(8639999999999999999) );

    
        {
            Boolean bad = false;
    
            try
            {
                CIMDateTime::getDifference(startInterval, finishTime);
            }
            catch (InvalidDateTimeFormatException&)
            {
                bad = true;
            }
    
            assert(bad);
        }

        {
            Boolean good = true;
    
            try
            {
                CIMDateTime currDT = CIMDateTime::getCurrentDateTime();
            }
            catch (InvalidDateTimeFormatException&)
            {
                good = false;
            }
            catch (...)
            {
                good = false;
            }
    
            assert(good);
        }


        /*********************************************************
                testing operators added in PEP 192
     ************************************************************/
 
  //testing for operator+

  Uint64 tfo_n = PEGASUS_UINT64_LITERAL(123456732445);
  Uint64 tfo_nA = PEGASUS_UINT64_LITERAL(6789143423);
  CIMDateTime tfo_nCDT = CIMDateTime(tfo_n, true);
  CIMDateTime tfo_nACDT = CIMDateTime(tfo_nA, true);
  Uint64 sum_n = tfo_n + tfo_nA;
  CIMDateTime sum_CDT = CIMDateTime(sum_n, true);
  CIMDateTime CDT_sum = tfo_nCDT + tfo_nACDT;
  if (String::compare(CDT_sum.toString(),sum_CDT.toString())) {
    assert(false);
  }

  
  Uint64 tfo_n2 = PEGASUS_UINT64_LITERAL(1235764732445);
  Uint64 tfo_nA2 = PEGASUS_UINT64_LITERAL(6789143423);
  CIMDateTime tfo_nCDT2 = CIMDateTime(tfo_n2, false);
  CIMDateTime tfo_nACDT2 = CIMDateTime(tfo_nA2, true);
  Uint64 sum_n2 = tfo_n2 + tfo_nA2;
  CIMDateTime sum_CDT2 = CIMDateTime(sum_n2, false);
  CIMDateTime CDT_sum2 = tfo_nCDT2 + tfo_nACDT2;
  if (String::compare(CDT_sum2.toString(),sum_CDT2.toString())) {
    assert(false);
  }

  
  Uint64 tfo_n3 = PEGASUS_UINT64_LITERAL(123456732445134);
  Uint64 tfo_nA3 = PEGASUS_UINT64_LITERAL(678947563423);
  CIMDateTime tfo_nCDT3 = CIMDateTime(tfo_n3, false);
  CIMDateTime tfo_nACDT3 = CIMDateTime(tfo_nA3, true);
  Uint64 sum_n3 = tfo_n3 + tfo_nA3;
  CIMDateTime sum_CDT3 = CIMDateTime(sum_n3, false);
  CIMDateTime CDT_sum3 = tfo_nCDT3 + tfo_nACDT3;
  if (String::compare(CDT_sum3.toString(),sum_CDT3.toString())) {
    assert(false);
  }

 
  Uint64 tfo_n4 = PEGASUS_UINT64_LITERAL(123456732445134);
  Uint64 tfo_nA4 = PEGASUS_UINT64_LITERAL(678947563423);
  CIMDateTime tfo_nCDT4 = CIMDateTime(tfo_n4, false);
  CIMDateTime tfo_nACDT4 = CIMDateTime(tfo_nA4, true);
  Uint64 sum_n4 = tfo_n4 + tfo_nA4;
  CIMDateTime sum_CDT4 = CIMDateTime(sum_n4, false);
  CIMDateTime CDT_sum4 = tfo_nCDT4 + tfo_nACDT4;
  if (String::compare(CDT_sum4.toString(),sum_CDT4.toString())) {
    assert(false);
  }

 
  CIMDateTime tfo_n5 = CIMDateTime("20040520041400.0*****+450");
  CIMDateTime tfo_nA5 = CIMDateTime("000405200414**.******:000");
  Uint64  num_n5 = tfo_n5.toMicroSeconds();
  Uint64  num_nA5 = tfo_nA5.toMicroSeconds();
  Uint64 sum5 = num_n5 + num_nA5;       
  CIMDateTime sum_cdt5 = CIMDateTime(sum5, false);
  CIMDateTime cdt_sum5 = tfo_n5 + tfo_nA5;
  if (String::compare(sum_cdt5.toString(),sum_cdt5.toString())) {
      assert(false);
  }

/*******************************************************************************************/
 
  //testing operator+=

  
  CIMDateTime top_cdt0 =  CIMDateTime("000405200414**.******:000");
  CIMDateTime topA_cdt0 =  CIMDateTime("00000520041452.******:000");
  Uint64 top_num0 = top_cdt0.toMicroSeconds();
  Uint64 topA_num0 = topA_cdt0.toMicroSeconds();
  Uint64 tot_num0 = top_num0 + topA_num0;
  top_cdt0 += topA_cdt0;
  CIMDateTime topA0 = CIMDateTime(tot_num0,true);
  if (top_cdt0 != topA0) {
      assert(false);
  }


  CIMDateTime top_cdt2 =  CIMDateTime("00040520041412.1234**+000");
  CIMDateTime topA_cdt2 =  CIMDateTime("00000520041452.******:000");
  Uint64 top_num2 = top_cdt2.toMicroSeconds();
  Uint64 topA_num2 = topA_cdt2.toMicroSeconds();
  Uint64 tot_num2 = top_num2 + topA_num2;
  top_cdt2 += topA_cdt2;
  CIMDateTime topA2 = CIMDateTime(tot_num2,false);
  if (top_cdt2 != topA2) {
      assert(false);
  }

 
 
  CIMDateTime top_cdt3 =  CIMDateTime("00020220******.******+230");
  CIMDateTime topA_cdt3 =  CIMDateTime("00000220041452.******:000");
  Uint64 top_num3 = top_cdt3.toMicroSeconds();
  Uint64 topA_num3 = topA_cdt3.toMicroSeconds();
  Uint64 tot_num3 = top_num3 + topA_num3;
  top_cdt3 += topA_cdt3;
  Uint64 top_3n = top_cdt3.toMicroSeconds();
  CIMDateTime tA = CIMDateTime(tot_num3,false);
  if (tA != top_cdt3) {
      assert(false);
  }      


  
  CIMDateTime top_cdt4 =  CIMDateTime("00040520041412.123435+000");
  CIMDateTime topA_cdt4 =  CIMDateTime("00000520041452.123456:000");
  Uint64 top_num4 = top_cdt4.toMicroSeconds();
  Uint64 topA_num4 = topA_cdt4.toMicroSeconds();
  Uint64 tot_num4 = top_num4 + topA_num4;
  CIMDateTime tot_cdt4 = CIMDateTime(tot_num4, false);
  //tot_cdt4 = tot_cdt4.setUctOffSet(-100);
  top_cdt4 += topA_cdt4;
  if (String::compare(top_cdt4.toString(),tot_cdt4.toString())) {
      assert(false);
  }


  /*************************************************************************************************/

 // "testing the operator- 

 
 Uint64 tfo_m = PEGASUS_UINT64_LITERAL(123456732445);
 Uint64 tfo_mA = PEGASUS_UINT64_LITERAL(6789143423);
 CIMDateTime tfo_mCDT = CIMDateTime(tfo_m, true);
 CIMDateTime tfo_mACDT = CIMDateTime(tfo_mA, true);
 Uint64 sum_m = tfo_m - tfo_mA;
 CIMDateTime dif_CDT = CIMDateTime(sum_m, true);
 CIMDateTime CDT_dif = tfo_mCDT - tfo_mACDT;
 if (String::compare(dif_CDT.toString(), CDT_dif.toString())) {
   assert(false);
 }   

 
 Uint64 tfo_m2 = PEGASUS_UINT64_LITERAL(12357647324458);
 Uint64 tfo_mA2 = PEGASUS_UINT64_LITERAL(6789143423);
 CIMDateTime tfo_mCDT2 = CIMDateTime(tfo_m2, false);
 CIMDateTime tfo_mACDT2 = CIMDateTime(tfo_mA2, true);
 Uint64 dif_m2 = tfo_m2 - tfo_mA2;
 CIMDateTime dif_CDT2 = CIMDateTime(dif_m2, false);
 CIMDateTime CDT_dif2 = tfo_mCDT2 - tfo_mACDT2;
 if (String::compare(dif_CDT2.toString(), CDT_dif2.toString())) {
   assert(false);
 }


 Uint64 tfo_m3 = PEGASUS_UINT64_LITERAL(12357647324458);
 Uint64 tfo_mA3 = PEGASUS_UINT64_LITERAL(6789143423);
 CIMDateTime tfo_mCDT3 = CIMDateTime(tfo_m3, false);
 CIMDateTime tfo_mACDT3 = CIMDateTime(tfo_mA3, false);
 Uint64 dif_m3 = tfo_m3 - tfo_mA3;
 CIMDateTime dif_CDT3 = CIMDateTime(dif_m3, true);
 CIMDateTime CDT_dif3 = tfo_mCDT3 - tfo_mACDT3;
 if (String::compare(dif_CDT3.toString(), CDT_dif3.toString())) {
   assert(false);
 }
 


 Uint64 tfo_m4 = PEGASUS_UINT64_LITERAL(123576473244426265);
 Uint64 tfo_mA4 = PEGASUS_UINT64_LITERAL(6789143423435);
 CIMDateTime mCDT4 = CIMDateTime(tfo_m4, false);
 CIMDateTime tfo_mACDT4 = CIMDateTime(tfo_mA4, false);
 Uint64 dif_m4 = tfo_m4 - tfo_mA4;
 CIMDateTime CDT_dif4 = mCDT4 - tfo_mACDT4;
 Uint64 CDT_num = CDT_dif4.toMicroSeconds();
 if (CDT_num != dif_m4) {
   assert(false);
 }
                  
 /************************************************************************************************/

 // testing operator-= 
    

CIMDateTime top_tko4 =  CIMDateTime("00040520041412.123435-000");
CIMDateTime topA_tko4 =  CIMDateTime("00000520041452.123456:000");
Uint64 top_yup4 = top_tko4.toMicroSeconds();
Uint64 topA_yup4 = topA_tko4.toMicroSeconds();
Uint64 ot_yup4 = top_yup4 - topA_yup4;
top_tko4 -= topA_tko4;
CIMDateTime tko4 = CIMDateTime(ot_yup4,false);
if (top_tko4 != tko4) {
    assert(false);
}



CIMDateTime top_tko2 =  CIMDateTime("00040520041412.123435:000");
CIMDateTime topA_tko2 =  CIMDateTime("00000520041452.123456:000");
Uint64 top_yup2 = top_tko2.toMicroSeconds();
Uint64 topA_yup2 = topA_tko2.toMicroSeconds();
Uint64 ot_yup2 = top_yup2 - topA_yup2;
top_tko2 -= topA_tko2;
Uint64 tko2 = top_tko2.toMicroSeconds();
if (ot_yup2 != tko2) {
    assert(false);
}                              



CIMDateTime top_tko3 =  CIMDateTime("00040520041412.123435+000");
CIMDateTime topA_tko3 =  CIMDateTime("00000520041452.123***:000");
Uint64 top_yup3 = top_tko3.toMicroSeconds();
Uint64 topA_yup3 = topA_tko3.toMicroSeconds();
Uint64 ot_yup3 = top_yup3 - topA_yup3;
top_tko3 -= topA_tko3;
CIMDateTime tko3 = CIMDateTime(ot_yup3,false);
if (top_tko3 != tko3) {
    assert(false);
}


 /*******************************************************************************************/
                                          
//test operator* 


CIMDateTime mul1 = CIMDateTime("00000020041452.123456:000");
Uint64 m_num1 = 12;
CIMDateTime pr_c1 = mul1 * m_num1;
Uint64 mul_cn1 = mul1.toMicroSeconds() * m_num1;
CIMDateTime c_pr1 = CIMDateTime(mul_cn1, true);
if (String::compare(c_pr1.toString(), pr_c1.toString())) {
    assert(false);
}


CIMDateTime mul2 = CIMDateTime("00000020041452.12****:000");
Uint64 m_num2 = 2;
CIMDateTime pr_c2 = mul2 * m_num2;
Uint64 mul_cn2 = mul2.toMicroSeconds() * m_num2;
Uint64 pr_num2 = pr_c2.toMicroSeconds();
if (pr_num2 != mul_cn2) {
    assert(false);
}


CIMDateTime mul3 = CIMDateTime("000123200414**.******:000");
Uint64 m_num3 = 2;
CIMDateTime pr_c3 = mul3 * m_num3;
Uint64 mul_cn3 = mul3.toMicroSeconds() * m_num2;
Uint64 pr_num3 = pr_c3.toMicroSeconds();
if (pr_num3 != mul_cn3) {
    assert(false);
}



 /*************************************************************************************/

    //test operator*= 


CIMDateTime ul1 = CIMDateTime("00000020041452.123456:000");
m_num1 = 12;
Uint64 ul_cn1 = ul1.toMicroSeconds() * m_num1;
ul1 *= m_num1;
Uint64 ul_num1 = ul1.toMicroSeconds();
if (ul_num1 != ul_cn1) {
    assert(false);
}


CIMDateTime ul2 = CIMDateTime("00000020041452.123***:000");
m_num2 = 12;
Uint64 ul_cn2 = ul2.toMicroSeconds() * m_num2;
ul2 *= m_num2;
Uint64 ul_num2 = ul2.toMicroSeconds();
if (ul_num2 != ul_cn2) {
    assert(false);
}


/*********************************************************************************************/
 
    //testing operator


CIMDateTime dul1 = CIMDateTime("00000020041452.123456:000");
Uint64 d_num1 = 12;
CIMDateTime r_c1 = dul1/d_num1;
Uint64 dul_cn1 = dul1.toMicroSeconds() / d_num1;
Uint64 r_num1 = r_c1.toMicroSeconds();
if (r_num1 != dul_cn1) {
    assert(false);
}


CIMDateTime dul2 = CIMDateTime("00023020041452.12****:000");
Uint64 d_num2 = 12;
CIMDateTime r_c2 = dul2/d_num2;
Uint64 dul_cn2 = dul2.toMicroSeconds() / d_num2;
CIMDateTime dulc = CIMDateTime(dul_cn2,true);
Uint64 r_num2 = r_c2.toMicroSeconds();
if (dulc != r_c2) {
    assert(false);
}


/*****************************************************************************************/

    //testing operator/= 

    d_num1 = 50;

   
    CIMDateTime dull1 = CIMDateTime("00000520041452.123456:000");  
    Uint64 dull_cn1 = dull1.toMicroSeconds() / d_num1;
    dull1 /= d_num1;
    Uint64 dul_num1 = dull1.toMicroSeconds();
    if (dul_num1 != dull_cn1) {
        assert(false);
    }

/*********************************************************************************************/

    //"testing I/I operator/ "

 
    CIMDateTime tii1 =  CIMDateTime("00040520041412.123435:000");
    CIMDateTime tii_a1 =  CIMDateTime("00000520041452.123456:000");
    Uint64 tii_n1 = tii1.toMicroSeconds();
    Uint64 tii_an1 = tii_a1.toMicroSeconds();
    Uint64 div_n = tii_n1/tii_an1;
    Uint64 div_c = tii1/tii_a1;
    if (div_n != div_c) {
        assert(false); 
    }


    CIMDateTime tii2 =  CIMDateTime("000405200414**.******:000");
    CIMDateTime tii_a2 =  CIMDateTime("00000520041452.1234**:000");
    Uint64 tii_n2 = tii2.toMicroSeconds();
    Uint64 tii_an2 = tii_a2.toMicroSeconds();
    Uint64 div_n2 = tii_n2/tii_an2;
    Uint64 div_c2 = tii2/tii_a2 ;
    if (div_n != div_c) {
        assert(false); 
    }


    /****************************************************************************************/

    //testing of comparison

    CIMDateTime las = CIMDateTime("000405200414**.******:000");
    CIMDateTime last =  CIMDateTime("00040520041435.74****:000");

    if (las > last) {
        assert(false);
    }
    if (las < last) {
        assert(false);
    }
    if (!(las >= last)) {
        assert(false);
    }
    if (!(las <= last)) {
        assert(false);
    }
    if (las != last) {
        assert(false);
    }

    CIMDateTime las2 = CIMDateTime("000405200414**.******+000");
    CIMDateTime last2 =  CIMDateTime("00040520041435.74****+000");

    if (las2 > last2) {
        assert(false);
    }
    if (las2 < last2) {
        assert(false);
    }
    if (!(las2 >= last2)) {
        assert(false);
    }
    if (!(las2 <= last2)) {
        assert(false);
    }
    if (las2 != last2) {
        assert(false);
    }

    CIMDateTime las3 = CIMDateTime("0004052004****.******+050");
    CIMDateTime last3 =  CIMDateTime("00040520041435.74****+000");

    if (las3 > last3) {
        assert(false);
    }
    if (las3 < last3) {
        assert(false);
    }
    if (!(las3 >= last3)) {
        assert(false);
    }
    if (!(las3 <= last3)) {
        assert(false);
    }
    if (las3 != last3) {
        assert(false);
    }


    CIMDateTime las4 =   CIMDateTime("000405200444**.******+030");
    CIMDateTime last4 =  CIMDateTime("00040520041435.74****+000");

    if (las4 > last4) {
        assert(false);
    }
    if (las4 < last4) {
        assert(false);
    }
    if (!(las4 >= last4)) {
        assert(false);
    }
    if (!(las4 <= last4)) {
        assert(false);
    }
    if (las4 != last4) {
        assert(false);
    }                    


    }
    catch (Exception & e)
    {
        cout << "Exception: " << e.getMessage() << endl;
        cout << "Exception caught at the end of test file" << endl;
        exit (1);
    }

    
    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
