//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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

#ifndef Pegasus_DateTime_h
#define Pegasus_DateTime_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>          
#include <Pegasus/Common/Linkage.h>


PEGASUS_NAMESPACE_BEGIN


struct CIMDateTimeRep;

/**

    The CIMDateTime class represents the CIM datetime data type as a C++ class
    CIMDateTime. A CIM datetime may contain a time stamp or an interval. CIMDateTime
    is an intrinsic CIM data type that represents the time as a string with a
    fixed length.

    <PRE>
    A time stamp has the following form:
    yyyymmddhhmmss.mmmmmmsutc

    Where

    yyyy = year (1-9999)
    mm = month (1-12)
    dd = day (1-31)
    hh = hour (0-23)
    mm = minute (0-59)
    ss = second (0-59)
    mmmmmm = microseconds
    s = '+' or '-' to represent the Coordinated Universal Time (UTC) sign
    utc = offset from Coordinated Universal Time (UTC)
        (same as Greenwich Mean Time(GMT) offset)

    An interval has the following form:

    ddddddddhhmmss.mmmmmm:000

    Where

    dddddddd = days
    hh = hours (0-23)
    mm = minutes (0-59)
    ss = seconds (0-59)
    mmmmmm = microseconds
    </PRE>

    Note: intervals always end in ":000". This distinguishes intervals from time stamps.

    CIMDateTime objects are constructed from String objects or from
    other CIMDateTime objects.  Character strings must be exactly
    twenty-five characters in length and conform to either the time stamp or interval
    format.

    CIMDateTime objects that are not explicitly initialized will be
    implicitly initialized with a zero time interval:

    00000000000000.000000:000



The following table shows what arithmetic operations are allowed
between CIMDateTime types. The entries in the last four columns define 
the type of the result when the operation, specified in the column header, 
is performed on operands, of the types specified in the first two columns.

<PRE>
LHS - left hand side    TS - time stamp  int - integer  
RHS - right hand side   IV - interval   
X - operation not allowed between types

 
           
LHS     RHS    +       -       *        /                       
_____________________________________________
TS      TS     X       IV      X       X
           
TS      IV     TS      TS      X       X
           
TS      int    X       X       X       X
           
IV      IV     IV      IV      X       int 
                                                    
IV      TS     X       X       X       X
           
IV      int    X       X       IV      IV
           
int     TS     X       X       X       X
           
int     IV     X       X       X       X
           

</PRE> 
 
The relational operators may only operate on two operands of the same type, 
i.e. two time stamps or two intervals.
           
*/

class PEGASUS_COMMON_LINKAGE CIMDateTime
{

public:

    /** Creates a new CIMDateTime object with a zero interval value.
    */
    CIMDateTime();

    /** Creates a new CIMDateTime object from a string constant representing
        the CIM DateTime formatted datetime.
        See the class documentation for CIMDateTime for the definition of the
        input string for absolute and interval datetime.
        @param str  String object containing the CIMDateTime formatted string.
        This must contain twenty-five characters.
        @exception InvalidDateTimeFormatException if the input sting  is not
        formatted correctly
    */
    CIMDateTime(const String & str);

    /** Creates a CIMDateTime object from another CIMDateTime object.
        @param x  Specifies the name of the CIMDateTime object to copy.
    */
    CIMDateTime(const CIMDateTime& x);

    /** Creates a CIMDateTime object from an integer.
        @param microSec For a time stamp, the number of microseconds since 
        the epoch 0/0/0000 (12 am Jan 1, 1BCE); For an interval, the number 
        of microseconds in the interval. 
        @param interval tells what type of CIMDateTime object  will be created.
        If true an interval will be created, if false a time stamp will
        be created.
        @exception DateTimeOutOfRangeException if param microSec is grater then
        317,455,200,000,000,000 for time stamps or 8,640,000,000,000,000,000 for
        intervals.
        @exception InvalidDateTimeFormatException if CIMDateTime object was not 
        formed correctly.
     */
    CIMDateTime(Uint64 microSec, Boolean interval);

    /** CIMDateTime destructor. */
    ~CIMDateTime();

    /** Assigns one instance of the CIMDateTime object to another.
        @param x  The CIMDateTime Object to assign to the CIMDateTime object.
        For example, you can assign the d1 CIMDateTime instance to the d2
        CIMDateTime instance.
        <PRE>
            CIMDateTime d1;
            CIMDateTime d2 = "00000000000000.000000:000";
            d1 = d2;
        </PRE>
        Therefore, d1 is assigned the same "00000000000000.000000:000" value as d2.
    */
    CIMDateTime& operator=(const CIMDateTime& x);

    /** Returns a string representing the DateTime value of the
        CIMDateTime object.
        @return String representing the DateTime value.
    */
    String toString () const;

    /** Sets the date and time in the CIMDateTime object from
        the input parameter.
        @param str  String constant containing the datetime
        in the datetime format. This must conform the to formatting rules specified
        in the CIMDateTime class description.  For example, the following sets
        the date to December 24, 1999 and time to 12:00 P.M.

        <PRE>
        CIMDateTime dt;
        dt.set("19991224120000.000000+360");
        </PRE>

        @exception InvalidDateTimeFormatException if the date and time are not
        formatted correctly.  See the CIMDateTime class description for the
        formatting rules.
    */
    void set(const String & str);

    /** Clears the datetime class object.  The date time is set to
        a zero interval value.
    */
    void clear();

    /** Receives the current time as CIMDateTime. The time is returned as the local time.
        @return CIMDateTime object containing the current date and time.
    */
    static CIMDateTime getCurrentDateTime();

    /** Computes the difference in microseconds between two CIMDateTime time stamps or
        two CIMDateTime intervals.
        @param startTime  Contains the start datetime.
        @param finishTime  Contains the finish datetime.
        @returns Integer that contains the difference between the two datetime values
        in microseconds.
        @exception InvalidDateTimeFormatException If arguments are not the same
        type of CIMDateTime.
        
    */
    static Sint64 getDifference(const CIMDateTime& startTime, const CIMDateTime& finishTime);

    /** Checks whether the datetime is an interval.
        @return True if the datetime is an interval; otherwise, false.
    */
    Boolean isInterval() const;

    /** Compares the CIMDateTime object to another CIMDateTime object for
        equality.
        @param x  CIMDateTime object to be compared.
        @returns true if the two CIMDateTime objects are equal; otherwise, false. 
        @exception TypeMismatchException If arguments are of different types.
    */
    Boolean equal (const CIMDateTime & x) const;

    /** Converts a CIMDateTime object to its microsecond representation.
        @returns  microseconds since the epoch, for time stamps. Number of
        microseconds in span of time, for intervals
        @exception DateTimeOutOfRangeException if value overflow occurs during 
        conversion to UTC (internal operation).
    */
    Uint64 toMicroSeconds();

     /** Adds two CIMDateTime objects and returns a CIMDateTime object that
         represents the sum.
         @param cDT operand on the RHS of the operator
         @returns a CIMDateTime object that is the result of adding the calling 
         object to the RHS operand 
         @exception DateTimeOutOfRangeException if overflow conditions occur.
         @exception TypeMismatchException if input and caller are not the correct
         type. (see table of operations)
    */
    CIMDateTime operator+(const CIMDateTime & cDT) const;

    /** Adds two CIMDateTime objects, returns the sum and changes
        the value of the calling CIMDateTime object to match the return value.
        @param cDT operand on the RHS of the operator
        @returns a CIMDateTime object that is the result of adding the calling 
        object to the RHS operand 
        @exception DateTimeOutOfRangeException if overflow conditions occur.
        @exception TypeMismatchException if input and caller are not the correct
        type. (see table of operations)

    */
    CIMDateTime & operator+=(const CIMDateTime & cDT);

    /** Subtracts one CIMDateTime object from another and returns a CIMDateTime object
        that represents the difference.
        @param cDT operand on the RHS of the operator
        @returns a CIMDateTime object that is the result of subtracting the 
        the RHS object from the calling.
        @exception DateTimeOutOfRangeException if overflow conditions occur.
        @exception TypeMismatchException if input and caller are not the correct
        type. (see table of operations)  
    */
    CIMDateTime operator-(const CIMDateTime & cDT) const;

    /** Subtracts one CIMDateTime object from another, returns the difference and changes
        the value of the calling CIMDateTime object to match the return value.
        @param cDT operand on the RHS of the operator
        @returns a CIMDateTime object that is the result of subtracting the object on 
        the RHS from the calling object.
        @exception DateTimeOutOfRangeException if underflow conditions occur for 
        result or overflow condition occurs during conversion to UTC (internal operation).
        @exception TypeMismatchException if input and caller are not the correct
        type. (see table of operations)       
    */
    CIMDateTime & operator-=(const CIMDateTime & cDT);

    /**Multiplies a CIMDateTime object by an integer and returns a CIMDateTime
       object that represents the product.
       @param num integer operand on the RHS of the operator
       @returns a CIMDateTime object that is the result of multiplying the calling 
       object by the RHS operand.
       @exception DateTimeOutOfRangeException if overflow conditions occur.
       @exception TypeMismatchException if caller is not the correct
       type. (see table of operations)   
    */
    CIMDateTime operator*(Uint64 num) const;

    /**Multiplies a CIMDateTime object by an integer, returns the product
       and changes the value of the calling object to match the returned product.
      @param num integer operand on the RHS of the operator
      @returns a CIMDateTime object that is the result of multiplying the calling 
      object by the RHS operand.
      @exception DateTimeOutOfRangeException if overflow condition occur.
      @exception TypeMismatchException if caller is not the correct
      type. (see table of operations)    
   */
    CIMDateTime & operator*=(Uint64 num);
    
    /**Divides a CIMDateTime object by an integer and returns a CIMDateTime
      object that represents the quotient.
      @param num integer operand on the RHS of the operator
      @returns a CIMDateTime object that is the result of dividing the calling 
      object by the RHS operand.
      @exception DateTimeOutOfRangeException if overflow condition occur.
      @exception TypeMismatchException if caller is not the correct
      type. (see table of operations)
      @exception Exception if param num is zero.   
   */
    CIMDateTime operator/(Uint64 num) const;

    /**Divides a CIMDateTime object by an integer, returns the quotient
       and changes the value of the calling object to match the returned quotient.
      @param num integer operand on the RHS of the operator
      @returns a CIMDateTime object that is the result of dividing the calling 
      object by the RHS operand
      @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC.(internal operation)
      @exception TypeMismatchException if caller is not the correct
      type. (see table of operations)
      @exception Exception if input is zero. 
   */
    CIMDateTime & operator/=(Uint64 num);

    /**Divides a CIMDateTime object by another CIMDateTime object and returns 
       an integer quotient.
      @param cdt CIMDateTime object on the RHS of the operator
      @returns an integer that is the result of dividing the number of microseconds 
      represented by the calling CIMDateTime object by the 
      number of microseconds represented by the CIMDateTime object on the RHS of 
      the operator.
      @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC.(internal operation)
      @exception TypeMismatchException if operands are not the correct
      type. (see table of operations)
 
   */
    Uint64 operator/(const CIMDateTime & cdt) const;

    /**Compare two CIMDateTime objects and returns true if the LHS is
       greater then RHS.
       @param cDT operand on the RHS of the operator
       @returns true if the LHS is greater then RHS.
       @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC.(internal operation)
       @exception TypeMismatchException if operands are not of the same
       type. 
     */
    Boolean operator<(const CIMDateTime & cDT) const;

    /**Compare two CIMDateTime objects and returns true if the LHS is
       greater then or equal to the RHS.
       @param cDT operand on the RHS of the operator
       @returns true if the LHS is greater then RHS.
       @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC.(internal operation)
       @exception TypeMismatchException if operands are not of the same
       type.  
    */
    Boolean operator<=(const CIMDateTime & cDT) const;

    /**Compare two CIMDateTime objects and returns true if the LHS is
       less then RHS.
       @param cDT operand on the RHS of the operator
       @returns true if the LHS is less then RHS.
       @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC (internal operation).
       @exception TypeMismatchException if operands are not of the same
       type.  
    */
    Boolean operator>(const CIMDateTime & cDT) const;

    /**Compare two CIMDateTime objects and returns true if the LHS is
       less then or equal to the RHS.
       @param cDT operand on the RHS of the operator
       @returns true if the LHS is less then RHS.
       @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC (internal operation).
       @exception TypeMismatchException if operands are not of the same
       type. 
    */
    Boolean operator>=(const CIMDateTime & cDT) const;

    /**Compare two CIMDateTime objects and returns true if the LHS is
       not equal to the RHS.
       @param cDT operand on the RHS of the operator
       @returns true if the LHS is not equal to RHS.
       @exception DateTimeOutOfRangeException if overflow condition
       occurs during conversion to UTC (internal operation).
       @exception TypeMismatchException if operands are not of the same
       type. 
    */
    Boolean operator!=(const CIMDateTime & cDT) const;

            
  
private: 

    CIMDateTimeRep* _rep;
    Boolean _set(const String & dateTimeStr);
    enum Field {ONLY_WILD_CARDS, SOME_WILD_CARDS, ONLY_DIGITS, ERR};

    Field fieldcheck(const String & in_p, String & rep_field);

    Boolean restOfFields(Uint32 start_position,const String & inStr);
    
    Uint64 _toMicroSeconds();

    void convertToUTC();

    void setUtcOffSet(Sint32 utc);

    void insert_WildCard(Uint32 index);
 
    Uint32 getHighestWildCardPosition(const CIMDateTime & cDT_s);
};

/**Compares two CIMDateTime objects and returns true if they represent the
   same time or length of time.
   @param x one of the CIMDateTime objects to be compared
   @param y one of the CIMDateTime objects to be compared
   @returns true if the two objects passed in represent the same time or 
   length of time.                                   
*/
PEGASUS_COMMON_LINKAGE Boolean operator==( const CIMDateTime& x, const CIMDateTime& y);

#define PEGASUS_ARRAY_T CIMDateTime
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T


PEGASUS_NAMESPACE_END

#endif /* Pegasus_DateTime_h */
