#ifndef Pegasus_SubRange_h
#define Pegasus_SubRange_h
                                                                                                                                       
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
                                                                                                                                       
PEGASUS_NAMESPACE_BEGIN
                                                                                                                                       
#define SUBRANGE_END_OF_ARRAY -1;  // signifies the array range ends with the last element in the array
#define SUBRANGE_NO_INDEX -2; // signifies the array range is empty                                                                                                                                      
class PEGASUS_CQL_LINKAGE SubRange{
/*
Exceptions:
        CQLInvalidArrayRangeException - for example: "invalid characters ![0-9*-..]"
                                                                                                                                       
*/
        public:
	  SubRange();
          SubRange(String range);
                                                                                                                                       
          Boolean operator==(const SubRange &rhs)const;
          Boolean operator!=(const SubRange &rhs)const;

	  String toString()const;
                                                                                                                                       
          Sint32 start;
          Sint32 end;
                                                                                                                                       
        private:
          void parse(String range);
          Boolean isNum(CString cstr);
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T SubRange
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
                                                                                                                                       
PEGASUS_NAMESPACE_END
 
#endif
#endif /* Pegasus_CQLIdentifier_h */

