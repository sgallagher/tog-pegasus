#include <Pegasus/CQL/SubRange.h>

PEGASUS_NAMESPACE_BEGIN
                                                                                                          
#define PEGASUS_ARRAY_T SubRange
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

SubRange::SubRange():start(0),end(0){}

SubRange::SubRange(String range){
        parse(range);
}
                                                                                                                                       
Boolean SubRange::operator==(const SubRange &rhs)const{
        return((start == rhs.start) && (end == rhs.end));
}
                                                                                                                                       
Boolean SubRange::operator!=(const SubRange &rhs)const{
        return(!operator==(rhs));
}

String SubRange::toString()const{
	char start_[15];
	char end_[15];
	sprintf(start_,"%d",start);
	sprintf(end_,"%d",end);
	String s(start_);
	s.append("-");
	s.append(end_);
	return s;
}

void SubRange::parse(String range){
        /*
          Look for the following possibilities:
          - "3"   start is set to 3, end is set to 3
          - "3-5" start is set to 3, end is set to 5
          - "3.." start is set to 3, end is set to END_OF_ARRAY
          - "..3" start is set to 0, end is set to 3
          = ".."  start is set to 0, end is set to END_OF_ARRAY
          - ""    start is set to NO_INDEX
          - "*"   start is set to 0, end is set to END_OF_ARRAY
        */
        Uint32 index;
        CString _cstr;
        if(range == String::EMPTY){
                start = SUBRANGE_NO_INDEX;
                end = SUBRANGE_NO_INDEX;
                return;
        }
        if(range == "*" || range == ".."){
                start = 0;
                end = SUBRANGE_END_OF_ARRAY;
                return;
        }
        if(range.size() == 1){
                _cstr = range.getCString();
                if(isNum(_cstr)){
                        start = atoi(_cstr);
                        end = start;
                }else{
                        // error
			 printf("SubRange::parse() error\n");
                }
                return;
        }
	if((index = range.find('-')) != PEG_NOT_FOUND){
                String s = range.subString(0,index);
                String e = range.subString(index+1);
                 _cstr = s.getCString();
                if(isNum(_cstr)){
                        start = atoi(_cstr);
                }else{
                        // error
			 printf("SubRange::parse() error\n");
                }
                 _cstr = e.getCString();
                if(isNum(_cstr)){
                        end = atoi(_cstr);
                }else{
                        // error
			 printf("SubRange::parse() error\n");
                }
                return;
        }
        if((index = range.find("..") == 0)){
                start = 0;
                String e = range.subString(2);
                 _cstr = e.getCString();
                if(isNum(_cstr)){
                        end = atoi(_cstr);
                }else{
                        // error
			 printf("SubRange::parse() error\n");
                }
                return;
        }else if((index = range.find("..")) != PEG_NOT_FOUND){
		// make sure nothing comes after the ..
		if(range.size() > (index+2)){
			// error
			printf("SubRange::parse() error\n");
		}
		printf("index = %d\n", index);
                String s = range.subString(0,index);
                 _cstr = s.getCString();
		printf("%s\n", (const char*)_cstr);
                if(isNum(_cstr)){
                        start = atoi(_cstr);
                }else{
                        // error
			printf("SubRange::parse() error\n");
                }
                end = SUBRANGE_END_OF_ARRAY;
                return;
        }
                                                                                                                                       
}

Boolean SubRange::isNum(CString cstr){
        Uint32 size = (Uint32)strlen(cstr);
        for(Uint32 i = 0; i < size; i++){
                if(!isdigit(cstr[i]))
                        return false;
        }
        return true;
}
                                                                                                                                       
PEGASUS_NAMESPACE_END


