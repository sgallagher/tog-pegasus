#line 1 "Headers.cpp"




























#line 1 "Array.h"







































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"
















#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"




 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"



















#pragma comment(lib,"msvcprtd")


#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"







#line 32 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"







#line 40 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"

#line 42 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"
#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"










#line 16 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
#line 17 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"


#pragma pack(push,8)
#line 22 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

 #pragma warning(4: 4018 4114 4146 4244 4245)
 #pragma warning(4: 4663 4664 4665)
 #pragma warning(disable: 4237 4284 4290 4514)
		
 
  
  
  
  
 



#line 37 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
namespace std {
		
 
typedef bool _Bool;
 #line 42 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
		



		




#line 52 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

		
 
  
  
 


#line 61 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

		
 
class __declspec(dllimport) _Lockit
	{	
public:
  
   
	_Lockit();
	~_Lockit();
  





#line 78 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
	};
 #line 80 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
		




};

#pragma pack(pop)
#line 89 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



extern "C" {
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"









#line 40 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"
#line 41 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"









#line 51 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"
#line 52 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"























#line 76 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"









#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"
#line 87 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"








__declspec(dllimport) extern int * __cdecl _errno(void);



#line 100 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"





typedef int ptrdiff_t;

#line 108 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



typedef unsigned int size_t;

#line 114 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



typedef unsigned short wchar_t;

#line 120 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



__declspec(dllimport) extern unsigned long  __cdecl __threadid(void);

__declspec(dllimport) extern unsigned long  __cdecl __threadhandle(void);
#line 127 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



}
#line 132 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"

#line 134 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"





#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"















#pragma pack(push,8)
#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
namespace std {
		
 
 
 
 
 
 
 
 
 
		
		
 
 
		


 
 
 
 
 
		
enum _Uninitialized {_Noinit};
		
__declspec(dllimport) void __cdecl _Nomemory();
};

#pragma pack(pop)
#line 56 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"

#line 58 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"





#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"













#pragma once
#line 16 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"






#line 23 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"





#pragma pack(push,8)
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"











































typedef void (__cdecl *terminate_function)();
typedef void (__cdecl *unexpected_function)();
typedef void (__cdecl *terminate_handler)();
typedef void (__cdecl *unexpected_handler)();


struct _EXCEPTION_POINTERS;
typedef void (__cdecl *_se_translator_function)(unsigned int, struct _EXCEPTION_POINTERS*);
#line 82 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"

__declspec(dllimport) void __cdecl terminate(void);
__declspec(dllimport) void __cdecl unexpected(void);

__declspec(dllimport) terminate_function __cdecl set_terminate(terminate_function);
__declspec(dllimport) unexpected_function __cdecl set_unexpected(unexpected_function);

__declspec(dllimport) _se_translator_function __cdecl _set_se_translator(_se_translator_function);
#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"


#pragma pack(pop)
#line 95 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"

#line 97 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"


#pragma pack(push,8)
#line 23 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"

 

#line 27 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"















typedef const char *__exString;

class __declspec(dllimport) exception
{
public:
    exception();
    exception(const __exString&);
    exception(const exception&);
    exception& operator= (const exception&);
    virtual ~exception();
    virtual __exString what() const;
private:
    __exString _m_what;
    int _m_doFree;
};
namespace std {
using ::exception;

		
class __declspec(dllimport) bad_exception : public exception {
public:
	bad_exception(const char *_S = "bad exception") throw ()
		: exception(_S) {}
	virtual ~bad_exception() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};

__declspec(dllimport) bool __cdecl uncaught_exception();

};






#pragma pack(pop)
#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"

#line 85 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"






#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"
namespace std {
		
class __declspec(dllimport) bad_alloc : public exception {
public:
	bad_alloc(const char *_S = "bad allocation") throw ()
		: exception(_S) {}
	virtual ~bad_alloc() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
struct nothrow_t {};
extern __declspec(dllimport) const nothrow_t nothrow;
};

typedef void (__cdecl *new_handler)();
extern new_handler _New_hand;

		
void __cdecl operator delete(void *) throw ();
void *__cdecl operator new(size_t) throw (std::bad_alloc);
void *__cdecl operator new(size_t, const std::nothrow_t&)
	throw ();



inline void *__cdecl operator new(size_t, void *_P)
	{return (_P); }
#line 40 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"


__declspec(dllimport)
#line 44 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"
	new_handler __cdecl set_new_handler(new_handler) throw ();


#pragma pack(pop)
#line 49 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"

#line 51 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"





#line 41 "Array.h"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"



extern "C" {
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"













































#line 76 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"













#line 90 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"




#line 95 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
























        void *  __cdecl memcpy(void *, const void *, size_t);
        int     __cdecl memcmp(const void *, const void *, size_t);
        void *  __cdecl memset(void *, int, size_t);
        char *  __cdecl _strset(char *, int);
        char *  __cdecl strcpy(char *, const char *);
        char *  __cdecl strcat(char *, const char *);
        int     __cdecl strcmp(const char *, const char *);
        size_t  __cdecl strlen(const char *);
#line 128 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
__declspec(dllimport) void *  __cdecl _memccpy(void *, const void *, int, unsigned int);
__declspec(dllimport) void *  __cdecl memchr(const void *, int, size_t);
__declspec(dllimport) int     __cdecl _memicmp(const void *, const void *, unsigned int);





__declspec(dllimport) void *  __cdecl memmove(void *, const void *, size_t);
#line 138 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


__declspec(dllimport) char *  __cdecl strchr(const char *, int);
__declspec(dllimport) int     __cdecl _strcmpi(const char *, const char *);
__declspec(dllimport) int     __cdecl _stricmp(const char *, const char *);
__declspec(dllimport) int     __cdecl strcoll(const char *, const char *);
__declspec(dllimport) int     __cdecl _stricoll(const char *, const char *);
__declspec(dllimport) int     __cdecl _strncoll(const char *, const char *, size_t);
__declspec(dllimport) int     __cdecl _strnicoll(const char *, const char *, size_t);
__declspec(dllimport) size_t  __cdecl strcspn(const char *, const char *);
__declspec(dllimport) char *  __cdecl _strdup(const char *);
__declspec(dllimport) char *  __cdecl _strerror(const char *);
__declspec(dllimport) char *  __cdecl strerror(int);
__declspec(dllimport) char *  __cdecl _strlwr(char *);
__declspec(dllimport) char *  __cdecl strncat(char *, const char *, size_t);
__declspec(dllimport) int     __cdecl strncmp(const char *, const char *, size_t);
__declspec(dllimport) int     __cdecl _strnicmp(const char *, const char *, size_t);
__declspec(dllimport) char *  __cdecl strncpy(char *, const char *, size_t);
__declspec(dllimport) char *  __cdecl _strnset(char *, int, size_t);
__declspec(dllimport) char *  __cdecl strpbrk(const char *, const char *);
__declspec(dllimport) char *  __cdecl strrchr(const char *, int);
__declspec(dllimport) char *  __cdecl _strrev(char *);
__declspec(dllimport) size_t  __cdecl strspn(const char *, const char *);
__declspec(dllimport) char *  __cdecl strstr(const char *, const char *);
__declspec(dllimport) char *  __cdecl strtok(char *, const char *);
__declspec(dllimport) char *  __cdecl _strupr(char *);
__declspec(dllimport) size_t  __cdecl strxfrm (char *, const char *, size_t);









#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"





















__declspec(dllimport) void * __cdecl memccpy(void *, const void *, int, unsigned int);
__declspec(dllimport) int __cdecl memicmp(const void *, const void *, unsigned int);
__declspec(dllimport) int __cdecl strcmpi(const char *, const char *);
__declspec(dllimport) int __cdecl stricmp(const char *, const char *);
__declspec(dllimport) char * __cdecl strdup(const char *);
__declspec(dllimport) char * __cdecl strlwr(char *);
__declspec(dllimport) int __cdecl strnicmp(const char *, const char *, size_t);
__declspec(dllimport) char * __cdecl strnset(char *, int, size_t);
__declspec(dllimport) char * __cdecl strrev(char *);
        char * __cdecl strset(char *, int);
__declspec(dllimport) char * __cdecl strupr(char *);

#line 209 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 211 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"







__declspec(dllimport) wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
__declspec(dllimport) int __cdecl wcscmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
__declspec(dllimport) size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
__declspec(dllimport) size_t __cdecl wcslen(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
__declspec(dllimport) size_t __cdecl wcsspn(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsstr(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);

__declspec(dllimport) wchar_t * __cdecl _wcsdup(const wchar_t *);
__declspec(dllimport) int __cdecl _wcsicmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl _wcsnset(wchar_t *, wchar_t, size_t);
__declspec(dllimport) wchar_t * __cdecl _wcsrev(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wcsset(wchar_t *, wchar_t);

__declspec(dllimport) wchar_t * __cdecl _wcslwr(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wcsupr(wchar_t *);
__declspec(dllimport) size_t __cdecl wcsxfrm(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl wcscoll(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsicoll(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsncoll(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl _wcsnicoll(const wchar_t *, const wchar_t *, size_t);






















__declspec(dllimport) wchar_t * __cdecl wcsdup(const wchar_t *);
__declspec(dllimport) int __cdecl wcsicmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl wcsnicmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsnset(wchar_t *, wchar_t, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsrev(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsset(wchar_t *, wchar_t);
__declspec(dllimport) wchar_t * __cdecl wcslwr(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsupr(wchar_t *);
__declspec(dllimport) int __cdecl wcsicoll(const wchar_t *, const wchar_t *);

#line 280 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 282 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


#line 285 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 287 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


}
#line 291 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 293 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"





#line 42 "Array.h"
#line 1 "../../../src\\Pegasus/Common/Config.h"






































#line 1 "../../../src\\Pegasus/Common/ConfigWindows.h"























































namespace Pegasus {





#line 63 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 69 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 75 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 81 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 87 "../../../src\\Pegasus/Common/ConfigWindows.h"



typedef unsigned char Uint8;
typedef char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned int Uint32;
typedef int Sint32;
typedef float Real32;
typedef double Real64;
typedef bool Boolean;
typedef unsigned __int64 Uint64;
typedef __int64 Sint64;

#pragma warning ( disable : 4251 )



}

#line 109 "../../../src\\Pegasus/Common/ConfigWindows.h"
#line 40 "../../../src\\Pegasus/Common/Config.h"


#line 43 "../../../src\\Pegasus/Common/Config.h"

#line 45 "../../../src\\Pegasus/Common/Config.h"
#line 43 "Array.h"
#line 1 "../../../src\\Pegasus/Common/Type.h"



























































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 61 "../../../src\\Pegasus/Common/Type.h"

namespace Pegasus {

class __declspec(dllexport) Type
{
public:

    enum Tag
    {
	NONE,
	BOOLEAN,
	UINT8,
	SINT8,
	UINT16,
	SINT16,
	UINT32,
	SINT32,
	UINT64,
	SINT64,
	REAL32,
	REAL64,
	CHAR16,
	STRING,
	DATETIME,
	REFERENCE
    };

    Type() : _tag(NONE) { }

    Type(Tag tag) : _tag(tag) { }

    explicit Type(Uint32 tag) : _tag(Tag(tag)) { }

    Type(const Type& x) : _tag(x._tag) { }

    Type& operator=(Tag tag) { _tag = tag; return *this; }

    operator Tag() const { return _tag; }

    Boolean equal(const Type& x) const { return x._tag == _tag; }

private:

    Tag _tag;
};

inline Boolean operator==(Type x, Type y)
{
    return Type::Tag(x) == Type::Tag(y);
}

inline Boolean operator!=(Type x, Type y)
{
    return !operator==(x, y);
}



inline Boolean operator==(Type x, Type::Tag y)
{
    return x.equal(y);
}

inline Boolean operator==(Type::Tag x, Type y)
{
    return y.equal(x);
}

inline Boolean operator!=(Type x, Type::Tag y)
{
    return !operator==(x, y);
}

inline Boolean operator!=(Type::Tag x, Type y)
{
    return !operator==(x, y);
}




__declspec(dllexport) const char* TypeToString(Type type);

}

#line 147 "../../../src\\Pegasus/Common/Type.h"
#line 44 "Array.h"
#line 1 "../../../src\\Pegasus/Common/Char16.h"










































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
















#pragma once
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"






#line 26 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"



extern "C" {
#line 31 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"













































#line 77 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"





__declspec(dllimport) extern int * __cdecl _errno(void);



#line 87 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
















































}
#line 137 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"

#line 139 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"













#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
















#line 56 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"





#line 62 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"





#line 68 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"








#line 77 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"








#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
































#line 119 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"
#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"







#pragma pack(push,8)
#line 34 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


extern "C" {
#line 38 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"













































#line 84 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
















typedef wchar_t wint_t;
typedef wchar_t wctype_t;

#line 104 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 105 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"









typedef char *  va_list;
#line 116 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 118 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






#line 125 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 127 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






























#line 158 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"





struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;

#line 176 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"











#line 188 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"










#line 199 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

















































__declspec(dllimport) extern FILE * __cdecl __p__iob(void);





#line 255 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 257 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 259 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"








typedef __int64 fpos_t;







#line 276 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


#line 279 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"




























__declspec(dllimport) int __cdecl _filbuf(FILE *);
__declspec(dllimport) int __cdecl _flsbuf(int, FILE *);




__declspec(dllimport) FILE * __cdecl _fsopen(const char *, const char *, int);
#line 315 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) void __cdecl clearerr(FILE *);
__declspec(dllimport) int __cdecl fclose(FILE *);
__declspec(dllimport) int __cdecl _fcloseall(void);




__declspec(dllimport) FILE * __cdecl _fdopen(int, const char *);
#line 325 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) int __cdecl feof(FILE *);
__declspec(dllimport) int __cdecl ferror(FILE *);
__declspec(dllimport) int __cdecl fflush(FILE *);
__declspec(dllimport) int __cdecl fgetc(FILE *);
__declspec(dllimport) int __cdecl _fgetchar(void);
__declspec(dllimport) int __cdecl fgetpos(FILE *, fpos_t *);
__declspec(dllimport) char * __cdecl fgets(char *, int, FILE *);




__declspec(dllimport) int __cdecl _fileno(FILE *);
#line 339 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) int __cdecl _flushall(void);
__declspec(dllimport) FILE * __cdecl fopen(const char *, const char *);
__declspec(dllimport) int __cdecl fprintf(FILE *, const char *, ...);
__declspec(dllimport) int __cdecl fputc(int, FILE *);
__declspec(dllimport) int __cdecl _fputchar(int);
__declspec(dllimport) int __cdecl fputs(const char *, FILE *);
__declspec(dllimport) size_t __cdecl fread(void *, size_t, size_t, FILE *);
__declspec(dllimport) FILE * __cdecl freopen(const char *, const char *, FILE *);
__declspec(dllimport) int __cdecl fscanf(FILE *, const char *, ...);
__declspec(dllimport) int __cdecl fsetpos(FILE *, const fpos_t *);
__declspec(dllimport) int __cdecl fseek(FILE *, long, int);
__declspec(dllimport) long __cdecl ftell(FILE *);
__declspec(dllimport) size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
__declspec(dllimport) int __cdecl getc(FILE *);
__declspec(dllimport) int __cdecl getchar(void);
__declspec(dllimport) int __cdecl _getmaxstdio(void);
__declspec(dllimport) char * __cdecl gets(char *);
__declspec(dllimport) int __cdecl _getw(FILE *);
__declspec(dllimport) void __cdecl perror(const char *);
__declspec(dllimport) int __cdecl _pclose(FILE *);
__declspec(dllimport) FILE * __cdecl _popen(const char *, const char *);
__declspec(dllimport) int __cdecl printf(const char *, ...);
__declspec(dllimport) int __cdecl putc(int, FILE *);
__declspec(dllimport) int __cdecl putchar(int);
__declspec(dllimport) int __cdecl puts(const char *);
__declspec(dllimport) int __cdecl _putw(int, FILE *);
__declspec(dllimport) int __cdecl remove(const char *);
__declspec(dllimport) int __cdecl rename(const char *, const char *);
__declspec(dllimport) void __cdecl rewind(FILE *);
__declspec(dllimport) int __cdecl _rmtmp(void);
__declspec(dllimport) int __cdecl scanf(const char *, ...);
__declspec(dllimport) void __cdecl setbuf(FILE *, char *);
__declspec(dllimport) int __cdecl _setmaxstdio(int);
__declspec(dllimport) int __cdecl setvbuf(FILE *, char *, int, size_t);
__declspec(dllimport) int __cdecl _snprintf(char *, size_t, const char *, ...);
__declspec(dllimport) int __cdecl sprintf(char *, const char *, ...);
__declspec(dllimport) int __cdecl sscanf(const char *, const char *, ...);
__declspec(dllimport) char * __cdecl _tempnam(const char *, const char *);
__declspec(dllimport) FILE * __cdecl tmpfile(void);
__declspec(dllimport) char * __cdecl tmpnam(char *);
__declspec(dllimport) int __cdecl ungetc(int, FILE *);
__declspec(dllimport) int __cdecl _unlink(const char *);
__declspec(dllimport) int __cdecl vfprintf(FILE *, const char *, va_list);
__declspec(dllimport) int __cdecl vprintf(const char *, va_list);
__declspec(dllimport) int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
__declspec(dllimport) int __cdecl vsprintf(char *, const char *, va_list);








#line 395 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"




__declspec(dllimport) FILE * __cdecl _wfsopen(const wchar_t *, const wchar_t *, int);
#line 401 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) wint_t __cdecl fgetwc(FILE *);
__declspec(dllimport) wint_t __cdecl _fgetwchar(void);
__declspec(dllimport) wint_t __cdecl fputwc(wint_t, FILE *);
__declspec(dllimport) wint_t __cdecl _fputwchar(wint_t);
__declspec(dllimport) wint_t __cdecl getwc(FILE *);
__declspec(dllimport) wint_t __cdecl getwchar(void);
__declspec(dllimport) wint_t __cdecl putwc(wint_t, FILE *);
__declspec(dllimport) wint_t __cdecl putwchar(wint_t);
__declspec(dllimport) wint_t __cdecl ungetwc(wint_t, FILE *);

__declspec(dllimport) wchar_t * __cdecl fgetws(wchar_t *, int, FILE *);
__declspec(dllimport) int __cdecl fputws(const wchar_t *, FILE *);
__declspec(dllimport) wchar_t * __cdecl _getws(wchar_t *);
__declspec(dllimport) int __cdecl _putws(const wchar_t *);

__declspec(dllimport) int __cdecl fwprintf(FILE *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl wprintf(const wchar_t *, ...);
__declspec(dllimport) int __cdecl _snwprintf(wchar_t *, size_t, const wchar_t *, ...);
__declspec(dllimport) int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl vfwprintf(FILE *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl vwprintf(const wchar_t *, va_list);
__declspec(dllimport) int __cdecl _vsnwprintf(wchar_t *, size_t, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl vswprintf(wchar_t *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl fwscanf(FILE *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl swscanf(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl wscanf(const wchar_t *, ...);






__declspec(dllimport) FILE * __cdecl _wfdopen(int, const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wfreopen(const wchar_t *, const wchar_t *, FILE *);
__declspec(dllimport) void __cdecl _wperror(const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wpopen(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wremove(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wtempnam(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wtmpnam(wchar_t *);



#line 446 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 447 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


#line 450 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"





















#line 472 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


























__declspec(dllimport) int __cdecl fcloseall(void);
__declspec(dllimport) FILE * __cdecl fdopen(int, const char *);
__declspec(dllimport) int __cdecl fgetchar(void);
__declspec(dllimport) int __cdecl fileno(FILE *);
__declspec(dllimport) int __cdecl flushall(void);
__declspec(dllimport) int __cdecl fputchar(int);
__declspec(dllimport) int __cdecl getw(FILE *);
__declspec(dllimport) int __cdecl putw(int, FILE *);
__declspec(dllimport) int __cdecl rmtmp(void);
__declspec(dllimport) char * __cdecl tempnam(const char *, const char *);
__declspec(dllimport) int __cdecl unlink(const char *);

#line 511 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 513 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


}
#line 517 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"



#pragma pack(pop)
#line 522 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 524 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"





#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
















#pragma once
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






#line 26 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"







#pragma pack(push,8)
#line 35 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


extern "C" {
#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"













































#line 85 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
































typedef int (__cdecl * _onexit_t)(void);



#line 122 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 124 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct _ldiv_t {
        long quot;
        long rem;
} ldiv_t;


#line 142 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




























__declspec(dllimport) int * __cdecl __p___mb_cur_max(void);



#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 177 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 179 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"





















#line 201 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"












__declspec(dllimport) int * __cdecl _errno(void);
__declspec(dllimport) unsigned long * __cdecl __doserrno(void);





#line 221 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



#line 225 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

































































































__declspec(dllimport) extern char * _sys_errlist[];   
__declspec(dllimport) extern int _sys_nerr;   












#line 337 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 338 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






#line 345 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




__declspec(dllimport) int *          __cdecl __p___argc(void);
__declspec(dllimport) char ***       __cdecl __p___argv(void);

__declspec(dllimport) wchar_t ***    __cdecl __p___wargv(void);
#line 354 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) char ***       __cdecl __p__environ(void);

__declspec(dllimport) wchar_t ***    __cdecl __p__wenviron(void);
#line 358 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) int *          __cdecl __p__fmode(void);
__declspec(dllimport) int *          __cdecl __p__fileinfo(void);
__declspec(dllimport) unsigned int * __cdecl __p__osver(void);
__declspec(dllimport) char **        __cdecl __p__pgmptr(void);

__declspec(dllimport) wchar_t **     __cdecl __p__wpgmptr(void);
#line 365 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) unsigned int * __cdecl __p__winver(void);
__declspec(dllimport) unsigned int * __cdecl __p__winmajor(void);
__declspec(dllimport) unsigned int * __cdecl __p__winminor(void);































#line 400 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 402 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



__declspec(dllimport) void   __cdecl abort(void);



#line 410 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        int    __cdecl abs(int);
#line 412 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        int    __cdecl atexit(void (__cdecl *)(void));
__declspec(dllimport) double __cdecl atof(const char *);
__declspec(dllimport) int    __cdecl atoi(const char *);
__declspec(dllimport) long   __cdecl atol(const char *);



__declspec(dllimport) void * __cdecl bsearch(const void *, const void *, size_t, size_t,
        int (__cdecl *)(const void *, const void *));
__declspec(dllimport) void * __cdecl calloc(size_t, size_t);
__declspec(dllimport) div_t  __cdecl div(int, int);
__declspec(dllimport) void   __cdecl exit(int);
__declspec(dllimport) void   __cdecl free(void *);
__declspec(dllimport) char * __cdecl getenv(const char *);
__declspec(dllimport) char * __cdecl _itoa(int, char *, int);

__declspec(dllimport) char * __cdecl _i64toa(__int64, char *, int);
__declspec(dllimport) char * __cdecl _ui64toa(unsigned __int64, char *, int);
__declspec(dllimport) __int64 __cdecl _atoi64(const char *);
#line 432 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 435 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        long __cdecl labs(long);
#line 437 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) ldiv_t __cdecl ldiv(long, long);
__declspec(dllimport) char * __cdecl _ltoa(long, char *, int);
__declspec(dllimport) void * __cdecl malloc(size_t);
__declspec(dllimport) int    __cdecl mblen(const char *, size_t);
__declspec(dllimport) size_t __cdecl _mbstrlen(const char *s);
__declspec(dllimport) int    __cdecl mbtowc(wchar_t *, const char *, size_t);
__declspec(dllimport) size_t __cdecl mbstowcs(wchar_t *, const char *, size_t);
__declspec(dllimport) void   __cdecl qsort(void *, size_t, size_t, int (__cdecl *)
        (const void *, const void *));
__declspec(dllimport) int    __cdecl rand(void);
__declspec(dllimport) void * __cdecl realloc(void *, size_t);
__declspec(dllimport) int    __cdecl _set_error_mode(int);
__declspec(dllimport) void   __cdecl srand(unsigned int);
__declspec(dllimport) double __cdecl strtod(const char *, char **);
__declspec(dllimport) long   __cdecl strtol(const char *, char **, int);



__declspec(dllimport) unsigned long __cdecl strtoul(const char *, char **, int);

__declspec(dllimport) int    __cdecl system(const char *);
#line 459 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) char * __cdecl _ultoa(unsigned long, char *, int);
__declspec(dllimport) int    __cdecl wctomb(char *, wchar_t);
__declspec(dllimport) size_t __cdecl wcstombs(char *, const wchar_t *, size_t);






__declspec(dllimport) wchar_t * __cdecl _itow (int, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ltow (long, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ultow (unsigned long, wchar_t *, int);
__declspec(dllimport) double __cdecl wcstod(const wchar_t *, wchar_t **);
__declspec(dllimport) long   __cdecl wcstol(const wchar_t *, wchar_t **, int);
__declspec(dllimport) unsigned long __cdecl wcstoul(const wchar_t *, wchar_t **, int);
__declspec(dllimport) wchar_t * __cdecl _wgetenv(const wchar_t *);
__declspec(dllimport) int    __cdecl _wsystem(const wchar_t *);
__declspec(dllimport) int __cdecl _wtoi(const wchar_t *);
__declspec(dllimport) long __cdecl _wtol(const wchar_t *);

__declspec(dllimport) wchar_t * __cdecl _i64tow(__int64, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ui64tow(unsigned __int64, wchar_t *, int);
__declspec(dllimport) __int64   __cdecl _wtoi64(const wchar_t *);
#line 483 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 486 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 487 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



__declspec(dllimport) char * __cdecl _ecvt(double, int, int *, int *);
__declspec(dllimport) void   __cdecl _exit(int);
__declspec(dllimport) char * __cdecl _fcvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl _fullpath(char *, const char *, size_t);
__declspec(dllimport) char * __cdecl _gcvt(double, int, char *);
        unsigned long __cdecl _lrotl(unsigned long, int);
        unsigned long __cdecl _lrotr(unsigned long, int);

__declspec(dllimport) void   __cdecl _makepath(char *, const char *, const char *, const char *,
        const char *);
#line 501 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        _onexit_t __cdecl _onexit(_onexit_t);
__declspec(dllimport) void   __cdecl perror(const char *);
__declspec(dllimport) int    __cdecl _putenv(const char *);
        unsigned int __cdecl _rotl(unsigned int, int);
        unsigned int __cdecl _rotr(unsigned int, int);
__declspec(dllimport) void   __cdecl _searchenv(const char *, const char *, char *);

__declspec(dllimport) void   __cdecl _splitpath(const char *, char *, char *, char *, char *);
#line 510 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) void   __cdecl _swab(char *, char *, int);






__declspec(dllimport) wchar_t * __cdecl _wfullpath(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) void   __cdecl _wmakepath(wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *,
        const wchar_t *);
__declspec(dllimport) void   __cdecl _wperror(const wchar_t *);
__declspec(dllimport) int    __cdecl _wputenv(const wchar_t *);
__declspec(dllimport) void   __cdecl _wsearchenv(const wchar_t *, const wchar_t *, wchar_t *);
__declspec(dllimport) void   __cdecl _wsplitpath(const wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *);


#line 527 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 528 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




__declspec(dllimport) void __cdecl _seterrormode(int);
__declspec(dllimport) void __cdecl _beep(unsigned, unsigned);
__declspec(dllimport) void __cdecl _sleep(unsigned long);
#line 536 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 539 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






__declspec(dllimport) int __cdecl tolower(int);
#line 547 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

__declspec(dllimport) int __cdecl toupper(int);
#line 550 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 552 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"











































__declspec(dllimport) char * __cdecl ecvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl fcvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl gcvt(double, int, char *);
__declspec(dllimport) char * __cdecl itoa(int, char *, int);
__declspec(dllimport) char * __cdecl ltoa(long, char *, int);
        _onexit_t __cdecl onexit(_onexit_t);
__declspec(dllimport) int    __cdecl putenv(const char *);
__declspec(dllimport) void   __cdecl swab(char *, char *, int);
__declspec(dllimport) char * __cdecl ultoa(unsigned long, char *, int);

#line 606 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 608 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 610 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


}

#line 615 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



#pragma pack(pop)
#line 620 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 622 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"





#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"










#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"


























































































#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"























































#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"


#pragma pack(push,8)
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


















#pragma once
#line 21 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"








#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



#pragma pack(push,8)
#line 35 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


extern "C" {
#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





















































#line 93 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"








typedef long time_t;

#line 104 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"













































































typedef unsigned long _fsize_t; 

#line 184 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



struct _wfinddata_t {
        unsigned attrib;
        time_t   time_create;   
        time_t   time_access;   
        time_t   time_write;
        _fsize_t size;
        wchar_t  name[260];
};



struct _wfinddatai64_t {
        unsigned attrib;
        time_t   time_create;   
        time_t   time_access;   
        time_t   time_write;
        __int64  size;
        wchar_t  name[260];
};
#line 207 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 208 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 211 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"




































__declspec(dllimport) extern unsigned short _ctype[];




__declspec(dllimport) unsigned short ** __cdecl __p__pctype(void);


__declspec(dllimport) wctype_t ** __cdecl ___p__pwctype(void);






#line 263 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 265 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"








                        
















__declspec(dllimport) int __cdecl iswalpha(wint_t);
__declspec(dllimport) int __cdecl iswupper(wint_t);
__declspec(dllimport) int __cdecl iswlower(wint_t);
__declspec(dllimport) int __cdecl iswdigit(wint_t);
__declspec(dllimport) int __cdecl iswxdigit(wint_t);
__declspec(dllimport) int __cdecl iswspace(wint_t);
__declspec(dllimport) int __cdecl iswpunct(wint_t);
__declspec(dllimport) int __cdecl iswalnum(wint_t);
__declspec(dllimport) int __cdecl iswprint(wint_t);
__declspec(dllimport) int __cdecl iswgraph(wint_t);
__declspec(dllimport) int __cdecl iswcntrl(wint_t);
__declspec(dllimport) int __cdecl iswascii(wint_t);
__declspec(dllimport) int __cdecl isleadbyte(int);

__declspec(dllimport) wchar_t __cdecl towupper(wchar_t);
__declspec(dllimport) wchar_t __cdecl towlower(wchar_t);

__declspec(dllimport) int __cdecl iswctype(wint_t, wctype_t);


__declspec(dllimport) int __cdecl is_wctype(wint_t, wctype_t);



#line 315 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) int __cdecl _wchdir(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wgetcwd(wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _wgetdcwd(int, wchar_t *, int);
__declspec(dllimport) int __cdecl _wmkdir(const wchar_t *);
__declspec(dllimport) int __cdecl _wrmdir(const wchar_t *);


#line 328 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) int __cdecl _waccess(const wchar_t *, int);
__declspec(dllimport) int __cdecl _wchmod(const wchar_t *, int);
__declspec(dllimport) int __cdecl _wcreat(const wchar_t *, int);
__declspec(dllimport) long __cdecl _wfindfirst(wchar_t *, struct _wfinddata_t *);
__declspec(dllimport) int __cdecl _wfindnext(long, struct _wfinddata_t *);
__declspec(dllimport) int __cdecl _wunlink(const wchar_t *);
__declspec(dllimport) int __cdecl _wrename(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wopen(const wchar_t *, int, ...);
__declspec(dllimport) int __cdecl _wsopen(const wchar_t *, int, int, ...);
__declspec(dllimport) wchar_t * __cdecl _wmktemp(wchar_t *);


__declspec(dllimport) long __cdecl _wfindfirsti64(wchar_t *, struct _wfinddatai64_t *);
__declspec(dllimport) int __cdecl _wfindnexti64(long, struct _wfinddatai64_t *);
#line 348 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 351 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) wchar_t * __cdecl _wsetlocale(int, const wchar_t *);


#line 360 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) int __cdecl _wexecl(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexecle(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexeclp(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexeclpe(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexecv(const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecve(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecvp(const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecvpe(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnl(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnle(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnlp(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnlpe(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnv(int, const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnve(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnvp(int, const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnvpe(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
__declspec(dllimport) int __cdecl _wsystem(const wchar_t *);


#line 387 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



















inline int __cdecl iswalpha(wint_t _C) {return (iswctype(_C,(0x0100|0x1|0x2))); }
inline int __cdecl iswupper(wint_t _C) {return (iswctype(_C,0x1)); }
inline int __cdecl iswlower(wint_t _C) {return (iswctype(_C,0x2)); }
inline int __cdecl iswdigit(wint_t _C) {return (iswctype(_C,0x4)); }
inline int __cdecl iswxdigit(wint_t _C) {return (iswctype(_C,0x80)); }
inline int __cdecl iswspace(wint_t _C) {return (iswctype(_C,0x8)); }
inline int __cdecl iswpunct(wint_t _C) {return (iswctype(_C,0x10)); }
inline int __cdecl iswalnum(wint_t _C) {return (iswctype(_C,(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswprint(wint_t _C)
	{return (iswctype(_C,0x40|0x10|(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswgraph(wint_t _C)
	{return (iswctype(_C,0x10|(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswcntrl(wint_t _C) {return (iswctype(_C,0x20)); }
inline int __cdecl iswascii(wint_t _C) {return ((unsigned)(_C) < 0x80); }

inline int __cdecl isleadbyte(int _C)
	{return ((*__p__pctype())[(unsigned char)(_C)] & 0x8000); }
#line 424 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 426 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"







typedef unsigned short _ino_t;      






typedef unsigned short ino_t;
#line 442 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 443 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 446 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"







typedef unsigned int _dev_t;        
#line 455 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"






typedef unsigned int dev_t;
#line 463 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 464 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 467 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



typedef long _off_t;                






typedef long off_t;
#line 479 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 480 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 483 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



struct _stat {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };





struct stat {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };

#line 519 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


struct _stati64 {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        __int64 st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };
#line 535 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 538 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"






__declspec(dllimport) int __cdecl _wstat(const wchar_t *, struct _stat *);


__declspec(dllimport) int __cdecl _wstati64(const wchar_t *, struct _stati64 *);
#line 549 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#line 552 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 554 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"










































































































#line 661 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

















































































struct tm {
        int tm_sec;     
        int tm_min;     
        int tm_hour;    
        int tm_mday;    
        int tm_mon;     
        int tm_year;    
        int tm_wday;    
        int tm_yday;    
        int tm_isdst;   
        };

#line 755 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) wchar_t * __cdecl _wasctime(const struct tm *);
__declspec(dllimport) wchar_t * __cdecl _wctime(const time_t *);
__declspec(dllimport) size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *,
        const struct tm *);
__declspec(dllimport) wchar_t * __cdecl _wstrdate(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wstrtime(wchar_t *);


#line 769 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



typedef int mbstate_t;
typedef wchar_t _Wint_t;

__declspec(dllimport) wint_t __cdecl btowc(int);
__declspec(dllimport) size_t __cdecl mbrlen(const char *, size_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);

__declspec(dllimport) size_t __cdecl wcrtomb(char *, wchar_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
__declspec(dllimport) int __cdecl wctob(wint_t);


inline int fwide(FILE *, int _M)
	{return (_M); }
inline int mbsinit(const mbstate_t *)
	{return (1); }
inline const wchar_t *wmemchr(const wchar_t *_S, wchar_t _C, size_t _N)
	{if (0) ; else for (; 0 < _N; ++_S, --_N)
		if (*_S == _C)
			return (_S);
	return (0); }
inline int wmemcmp(const wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{if (0) ; else for (; 0 < _N; ++_S1, ++_S2, --_N)
		if (*_S1 != *_S2)
			return (*_S1 < *_S2 ? -1 : +1);
	return (0); }
inline wchar_t *wmemcpy(wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{wchar_t *_Su1 = _S1;
	if (0) ; else for (; 0 < _N; ++_Su1, ++_S2, --_N)
		*_Su1 = *_S2;
	return (_S1); }
inline wchar_t *wmemmove(wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{wchar_t *_Su1 = _S1;
	if (_S2 < _Su1 && _Su1 < _S2 + _N)
		if (0) ; else for (_Su1 += _N, _S2 += _N; 0 < _N; --_N)
			*--_Su1 = *--_S2;
	else
		if (0) ; else for (; 0 < _N; --_N)
			*_Su1++ = *_S2++;
	return (_S1); }
inline wchar_t *wmemset(wchar_t *_S, wchar_t _C, size_t _N)
	{wchar_t *_Su = _S;
	if (0) ; else for (; 0 < _N; ++_Su, --_N)
		*_Su = _C;
	return (_S); }
}	
inline wchar_t *wmemchr(wchar_t *_S, wchar_t _C, size_t _N)
	{return ((wchar_t *)wmemchr((const wchar_t *)_S, _C, _N)); }
inline wchar_t *wcschr(wchar_t *_S, wchar_t _C)
	{return ((wchar_t *)wcschr((const wchar_t *)_S, _C)); }
inline wchar_t *wcspbrk(wchar_t *_S, const wchar_t *_P)
	{return ((wchar_t *)wcspbrk((const wchar_t *)_S, _P)); }
inline wchar_t *wcsrchr(wchar_t *_S, wchar_t _C)
	{return ((wchar_t *)wcsrchr((const wchar_t *)_S, _C)); }
inline wchar_t *wcsstr(wchar_t *_S, const wchar_t *_P)
	{return ((wchar_t *)wcsstr((const wchar_t *)_S, _P)); }
#line 830 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#pragma pack(pop)
#line 834 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 836 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"













#line 850 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"





#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"






























































#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"


#pragma pack(push,8)
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
namespace std {
                
typedef long streamoff;
const streamoff _BADOFF = -1;
typedef int streamsize;
extern __declspec(dllimport) const fpos_t _Fpz;
                
template<class _St>
        class fpos {
        typedef fpos<_St> _Myt;
public:

        fpos(streamoff _O = 0)
                : _Off(_O), _Fpos(_Fpz)
                {_Lockit _Lk;
                _State = _Stz; }



#line 32 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
        fpos(_St _S, fpos_t _F)
                : _Off(0), _Fpos(_F), _State(_S) {}
        _St state() const
                {return (_State); }
        void state(_St _S)
                {_State = _S; }
        fpos_t get_fpos_t() const
                {return (_Fpos); }
        operator streamoff() const
                {return (_Off + ((long)(_Fpos))); }
        streamoff operator-(const _Myt& _R) const
                {return ((streamoff)*this - (streamoff)_R); }
        _Myt& operator+=(streamoff _O)
                {_Off += _O;
                return (*this); }
        _Myt& operator-=(streamoff _O)
                {_Off -= _O;
                return (*this); }
        _Myt operator+(streamoff _O) const
                {_Myt _Tmp = *this;
                return (_Tmp += _O); }
        _Myt operator-(streamoff _O) const
                {_Myt _Tmp = *this;
                return (_Tmp -= _O); }
        bool operator==(const _Myt& _R) const
                {return ((streamoff)*this == (streamoff)_R); }
        bool operator!=(const _Myt& _R) const
                {return (!(*this == _R)); }
private:
        static _St _Stz;
        streamoff _Off;
        fpos_t _Fpos;
        _St _State;
        };
template<class _St>
        _St fpos<_St>::_Stz;
typedef fpos<mbstate_t> streampos;
typedef streampos wstreampos;
                
template<class _E>
        struct char_traits {
        typedef _E char_type;
        typedef _E int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {if (0) ; else for (size_t _I = 0; _I < _N; ++_I, ++_U, ++_V)
                        if (!eq(*_U, *_V))
                                return (lt(*_U, *_V) ? -1 : +1);
                return (0); }
        static size_t __cdecl length(const _E *_U)
                {size_t _N;
                if (0) ; else for (_N = 0; !eq(*_U, _E(0)); ++_U)
                        ++_N;
                return (_N); }
        static _E * __cdecl copy(_E *_U, const _E *_V, size_t _N)
                {_E *_S = _U;
                if (0) ; else for (; 0 < _N; --_N, ++_U, ++_V)
                        assign(*_U, *_V);
                return (_S); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {if (0) ; else for (; 0 < _N; --_N, ++_U)
                        if (eq(*_U, _C))
                                return (_U);
                return (0); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {_E *_Ans = _U;
                if (_V < _U && _U < _V + _N)
                        if (0) ; else for (_U += _N, _V += _N; 0 < _N; --_N)
                                assign(*--_U, *--_V);
                else
                        if (0) ; else for (; 0 < _N; --_N, ++_U, ++_V)
                                assign(*_U, *_V);
                return (_Ans); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {_E *_Ans = _U;
                if (0) ; else for (; 0 < _N; --_N, ++_U)
                        assign(*_U, _C);
                return (_Ans); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return (_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((-1)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
struct __declspec(dllimport) char_traits<wchar_t> {
        typedef wchar_t _E;
        typedef _E char_type;   
        typedef wint_t int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {return (wmemcmp(_U, _V, _N)); }
        static size_t __cdecl length(const _E *_U)
                {return (wcslen(_U)); }
        static _E *__cdecl copy(_E *_U, const _E *_V, size_t _N)
                {return (wmemcpy(_U, _V, _N)); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {return ((const _E *)wmemchr(_U, _C, _N)); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {return (wmemmove(_U, _V, _N)); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {return (wmemset(_U, _C, _N)); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return (_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((wint_t)(0xFFFF)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
struct __declspec(dllimport) char_traits<char> {
        typedef char _E;
        typedef _E char_type;
        typedef int int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {return (memcmp(_U, _V, _N)); }
        static size_t __cdecl length(const _E *_U)
                {return (strlen(_U)); }
        static _E * __cdecl copy(_E *_U, const _E *_V, size_t _N)
                {return ((_E *)memcpy(_U, _V, _N)); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {return ((const _E *)memchr(_U, _C, _N)); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {return ((_E *)memmove(_U, _V, _N)); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {return ((_E *)memset(_U, _C, _N)); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return ((unsigned char)_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((-1)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
template<class T>
        class allocator;
class ios_base;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ios;
template<class _E, class _Tr = char_traits<_E> >
        class istreambuf_iterator;
template<class _E, class _Tr = char_traits<_E> >
        class ostreambuf_iterator;
template<class _E, class _Tr = char_traits<_E> >
        class basic_streambuf;
template<class _E, class _Tr = char_traits<_E> >
        class basic_istream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ostream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_iostream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_stringbuf;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_istringstream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_ostringstream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_stringstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_filebuf;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ifstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ofstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_fstream;


template<class _E, class _II >
    class num_get;
template<class _E, class _OI >
    class num_put;

template<class _E>
    class collate;

#pragma warning(disable:4231) 
#pragma warning(disable:4662)

extern template class __declspec(dllimport) basic_ios<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_streambuf<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >;


extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, char&);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, signed char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, signed char&);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, unsigned char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, unsigned char&);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, wchar_t *);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, wchar_t&);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, signed short *);

extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, char);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const signed char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const signed char);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const unsigned char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const unsigned char);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const wchar_t);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const signed short *);

template<class _E, class _Tr = char_traits<_E>, class _A = allocator<_E> >
    class basic_string;
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const char, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl operator>>(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl getline(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl getline(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&, const char);
extern template class __declspec(dllimport)
    basic_ostream<char, char_traits<char> >& __cdecl operator<<(
        basic_ostream<char, char_traits<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);

extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const wchar_t, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl getline(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl getline(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t);
extern template class __declspec(dllimport)
    basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
        basic_ostream<wchar_t, char_traits<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);

template<class _E>
    class complex;
extern template class __declspec(dllimport) complex<float>& __cdecl operator+=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator-=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator*=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator/=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<float>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<float>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<float>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<float>&);

extern template class __declspec(dllimport) complex<double>& __cdecl operator+=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator-=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator*=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator/=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<double>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<double>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<double>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<double>&);

extern template class __declspec(dllimport) complex<long double>& __cdecl operator+=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator-=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator*=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator/=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<long double>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<long double>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<long double>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<long double>&);

extern template __declspec(dllimport) float __cdecl imag(const complex<float>&);
extern template __declspec(dllimport) float __cdecl real(const complex<float>&);
extern template __declspec(dllimport) float __cdecl _Fabs(const complex<float>&, int *);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&, const float&);
extern template class __declspec(dllimport)	complex<float> __cdecl operator-(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<float>&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<float>&, const float&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const float&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<float>&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<float>&, const float&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const float&, const complex<float>&);
extern template __declspec(dllimport) float __cdecl abs(const complex<float>&);
extern template __declspec(dllimport) float __cdecl arg(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl conj(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl cos(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl cosh(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl exp(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl log(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl log10(const complex<float>&);
extern template __declspec(dllimport) float __cdecl norm(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl polar(const float&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl polar(const float&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, int);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sin(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sinh(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sqrt(const complex<float>&);

extern template __declspec(dllimport) double __cdecl imag(const complex<double>&);
extern template __declspec(dllimport) double __cdecl real(const complex<double>&);
extern template __declspec(dllimport) double __cdecl _Fabs(const complex<double>&, int *);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&, const double&);
extern template class __declspec(dllimport)	complex<double> __cdecl operator-(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<double>&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<double>&, const double&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const double&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<double>&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<double>&, const double&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const double&, const complex<double>&);
extern template __declspec(dllimport) double __cdecl abs(const complex<double>&);
extern template __declspec(dllimport) double __cdecl arg(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl conj(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl cos(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl cosh(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl exp(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl log(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl log10(const complex<double>&);
extern template __declspec(dllimport) double __cdecl norm(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl polar(const double&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl polar(const double&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, int);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sin(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sinh(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sqrt(const complex<double>&);

extern template __declspec(dllimport) long double __cdecl imag(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl real(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl _Fabs(const complex<long double>&, int *);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport)	complex<long double> __cdecl operator-(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<long double>&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<long double>&, const long double&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const long double&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<long double>&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<long double>&, const long double&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const long double&, const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl abs(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl arg(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl conj(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl cos(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl cosh(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl exp(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl log(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl log10(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl norm(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl polar(const long double&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl polar(const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, int);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sin(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sinh(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sqrt(const complex<long double>&);
#line 718 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_iostream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_stringbuf<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_istringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_ostringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_stringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_filebuf<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ifstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ofstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_fstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ios<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_streambuf<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_iostream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_stringbuf<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_istringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_ostringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_stringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_filebuf<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ifstream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ofstream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_fstream<wchar_t, char_traits<wchar_t> >;

extern template class __declspec(dllimport) num_get<char,
                      istreambuf_iterator<char, char_traits<char> > >;
extern template class __declspec(dllimport) num_get<wchar_t,
                      istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
extern template class __declspec(dllimport) num_put<char,
                      ostreambuf_iterator<char, char_traits<char> > >;
extern template class __declspec(dllimport) num_put<wchar_t,
                      ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >;

extern template class __declspec(dllimport) collate<char>;
extern template class __declspec(dllimport) collate<wchar_t>;
#line 754 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

                
typedef basic_ios<char, char_traits<char> > ios;
typedef basic_streambuf<char, char_traits<char> > streambuf;
typedef basic_istream<char, char_traits<char> > istream;
typedef basic_ostream<char, char_traits<char> > ostream;
typedef basic_iostream<char, char_traits<char> > iostream;
typedef basic_stringbuf<char, char_traits<char>,
        allocator<char> > stringbuf;
typedef basic_istringstream<char, char_traits<char>,
        allocator<char> > istringstream;
typedef basic_ostringstream<char, char_traits<char>,
        allocator<char> > ostringstream;
typedef basic_stringstream<char, char_traits<char>,
        allocator<char> > stringstream;
typedef basic_filebuf<char, char_traits<char> > filebuf;
typedef basic_ifstream<char, char_traits<char> > ifstream;
typedef basic_ofstream<char, char_traits<char> > ofstream;
typedef basic_fstream<char, char_traits<char> > fstream;
                
typedef basic_ios<wchar_t, char_traits<wchar_t> > wios;
typedef basic_streambuf<wchar_t, char_traits<wchar_t> >
        wstreambuf;
typedef basic_istream<wchar_t, char_traits<wchar_t> > wistream;
typedef basic_ostream<wchar_t, char_traits<wchar_t> > wostream;
typedef basic_iostream<wchar_t, char_traits<wchar_t> > wiostream;
typedef basic_stringbuf<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wstringbuf;
typedef basic_istringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wistringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wostringstream;
typedef basic_stringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wstringstream;
typedef basic_filebuf<wchar_t, char_traits<wchar_t> > wfilebuf;
typedef basic_ifstream<wchar_t, char_traits<wchar_t> > wifstream;
typedef basic_ofstream<wchar_t, char_traits<wchar_t> > wofstream;
typedef basic_fstream<wchar_t, char_traits<wchar_t> > wfstream;


typedef num_get<char, istreambuf_iterator<char, char_traits<char> > > numget;
typedef num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >
                                                                                                                                          wnumget;
typedef num_put<char, ostreambuf_iterator<char, char_traits<char> > > numput;
typedef num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >
                                                                                                                                          wnumput;
typedef collate<char> ncollate;
typedef collate<wchar_t> wcollate;

#pragma warning(default:4662)
#pragma warning(default:4231) 
#line 806 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

};

#pragma pack(pop)
#line 811 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

#line 813 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"
namespace std {
		
template<class _T1, class _T2> struct pair {
	typedef _T1 first_type;
	typedef _T2 second_type;
	pair()
		: first(_T1()), second(_T2()) {}
	pair(const _T1& _V1, const _T2& _V2)
		: first(_V1), second(_V2) {}
	_T1 first;
	_T2 second;
	};
template<class _T1, class _T2> inline
	bool __cdecl operator==(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first == _Y.first && _X.second == _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator!=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X == _Y)); }
template<class _T1, class _T2> inline
	bool __cdecl operator<(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first < _Y.first ||
		!(_Y.first < _X.first) && _X.second < _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator>(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_Y < _X); }
template<class _T1, class _T2> inline
	bool __cdecl operator<=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_Y < _X)); }
template<class _T1, class _T2> inline
	bool __cdecl operator>=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X < _Y)); }
template<class _T1, class _T2> inline
	pair<_T1, _T2> __cdecl make_pair(const _T1& _X, const _T2& _Y)
	{return (pair<_T1, _T2>(_X, _Y)); }
		
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag
	: public input_iterator_tag {};
struct bidirectional_iterator_tag
	: public forward_iterator_tag {};
struct random_access_iterator_tag
	: public bidirectional_iterator_tag  {};
		
template<class _C, class _Ty, class _D = ptrdiff_t>
	struct iterator {
	typedef _C iterator_category;
	typedef _Ty value_type;
	typedef _D distance_type;
	};
template<class _Ty, class _D>
	struct _Bidit : public iterator<bidirectional_iterator_tag,
		_Ty, _D> {};
template<class _Ty, class _D>
	struct _Ranit : public iterator<random_access_iterator_tag,
		_Ty, _D> {};
		
template<class _It>
	struct iterator_traits {
	typedef _It::iterator_category iterator_category;
	typedef _It::value_type value_type;
	typedef _It::distance_type distance_type;
	};
		
template<class _C, class _Ty, class _D> inline
	_C __cdecl _Iter_cat(const iterator<_C, _Ty, _D>&)
	{_C _X;
	return (_X); }
template<class _Ty> inline
	random_access_iterator_tag __cdecl _Iter_cat(const _Ty *)
	{random_access_iterator_tag _X;
	return (_X); }
		
template<class _II> inline
	ptrdiff_t __cdecl distance(_II _F, _II _L)
	{ptrdiff_t _N = 0;
	_Distance(_F, _L, _N, _Iter_cat(_F));
	return (_N); }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N)
	{_Distance(_F, _L, _N, _Iter_cat(_F)); }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N, input_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N, forward_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N,
		bidirectional_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _RI, class _D> inline
	void __cdecl _Distance(_RI _F, _RI _L, _D& _N,
		random_access_iterator_tag)
	{_N += _L - _F; }
		
template<class _RI,
	class _Ty,
	class _Rt = _Ty&,
	class _Pt = _Ty *,
	class _D = ptrdiff_t>
	class reverse_iterator : public _Ranit<_Ty, _D> {
public:
	typedef reverse_iterator<_RI, _Ty, _Rt, _Pt, _D> _Myt;
	typedef _RI iter_type;
	typedef _Rt reference_type;
	typedef _Pt pointer_type;
	reverse_iterator()
		{}
	explicit reverse_iterator(_RI _X)
		: current(_X) {}
	_RI base() const
		{return (current); }
	_Rt operator*() const
		{return (*(current - 1)); }
	_Pt operator->() const
		{return (&**this); }
	_Myt& operator++()
		{--current;
		return (*this); }
	_Myt operator++(int)
		{_Myt _Tmp = *this;
		--current;
		return (_Tmp); }
	_Myt& operator--()
		{++current;
		return (*this); }
	_Myt operator--(int)
		{_Myt _Tmp = *this;
		++current;
		return (_Tmp); }
	_Myt& operator+=(_D _N)
		{current -= _N;
		return (*this); }
	_Myt operator+(_D _N) const
		{return (_Myt(current - _N)); }
	_Myt& operator-=(_D _N)
		{current += _N;
		return (*this); }
	_Myt operator-(_D _N) const
		{return (_Myt(current + _N)); }
	_Rt operator[](_D _N) const
		{return (*(*this + _N)); }
protected:
	_RI current;
	};
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator==(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_X.base() == _Y.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator!=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_X == _Y)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator<(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y.base() < _X.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator>(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y < _X); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator<=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_Y < _X)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator>=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_X < _Y)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	_D __cdecl operator-(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y.base() - _X.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	reverse_iterator<_RI, _Ty, _Rt, _Pt, _D> __cdecl operator+(_D _N,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>(
		_Y.base() - _N)); }
		
template<class _E, class _Tr = char_traits<_E> >
	class istreambuf_iterator
		: public iterator<input_iterator_tag, _E, _Tr::off_type> {
public:
	typedef istreambuf_iterator<_E, _Tr> _Myt;
	typedef _E char_type;
	typedef _Tr traits_type;
	typedef _Tr::int_type int_type;
	typedef basic_streambuf<_E, _Tr> streambuf_type;
	typedef basic_istream<_E, _Tr> istream_type;
	istreambuf_iterator(streambuf_type *_Sb = 0) throw ()
		: _Sbuf(_Sb), _Got(_Sb == 0) {}
	istreambuf_iterator(istream_type& _I) throw ()
		: _Sbuf(_I.rdbuf()), _Got(_I.rdbuf() == 0) {}
	const _E& operator*() const
		{if (!_Got)
			((_Myt *)this)->_Peek();
		return (_Val); }
	const _E *operator->() const
		{return (&**this); }
	_Myt& operator++()
		{_Inc();
		return (*this); }
	_Myt operator++(int)
		{if (!_Got)
			_Peek();
		_Myt _Tmp = *this;
		_Inc();
		return (_Tmp); }
	bool equal(const _Myt& _X) const
		{if (!_Got)
			((_Myt *)this)->_Peek();
		if (!_X._Got)
			((_Myt *)&_X)->_Peek();
		return (_Sbuf == 0 && _X._Sbuf == 0
			|| _Sbuf != 0 && _X._Sbuf != 0); }
private:
	void _Inc()
		{if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _Sbuf->sbumpc()))
			_Sbuf = 0, _Got = true;
		else
			_Got = false; }
	_E _Peek()
		{int_type _C;
		if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _C = _Sbuf->sgetc()))
			_Sbuf = 0;
		else
			_Val = _Tr::to_char_type(_C);
		_Got = true;
		return (_Val); }
	streambuf_type *_Sbuf;
	bool _Got;
	_E _Val;
	};
template<class _E, class _Tr> inline
	bool __cdecl operator==(const istreambuf_iterator<_E, _Tr>& _X,
		const istreambuf_iterator<_E, _Tr>& _Y)
	{return (_X.equal(_Y)); }
template<class _E, class _Tr> inline
	bool __cdecl operator!=(const istreambuf_iterator<_E, _Tr>& _X,
		const istreambuf_iterator<_E, _Tr>& _Y)
	{return (!(_X == _Y)); }
		
template<class _E, class _Tr = char_traits<_E> >
	class ostreambuf_iterator
		: public iterator<output_iterator_tag, void, void> {
	typedef ostreambuf_iterator<_E, _Tr> _Myt;
public:
	typedef _E char_type;
	typedef _Tr traits_type;
	typedef basic_streambuf<_E, _Tr> streambuf_type;
	typedef basic_ostream<_E, _Tr> ostream_type;
	ostreambuf_iterator(streambuf_type *_Sb) throw ()
		: _Failed(false), _Sbuf(_Sb) {}
	ostreambuf_iterator(ostream_type& _O) throw ()
		: _Failed(false), _Sbuf(_O.rdbuf()) {}
	_Myt& operator=(_E _X)
		{if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _Sbuf->sputc(_X)))
			_Failed = true;
		return (*this); }
	_Myt& operator*()
		{return (*this); }
	_Myt& operator++()
		{return (*this); }
	_Myt& operator++(int)
		{return (*this); }
	bool failed() const throw ()
		{return (_Failed); }
private:
	bool _Failed;
	streambuf_type *_Sbuf;
	};
		
	namespace rel_ops {
template<class _Ty> inline
	bool __cdecl operator!=(const _Ty& _X, const _Ty& _Y)
	{return (!(_X == _Y)); }
template<class _Ty> inline
	bool __cdecl operator>(const _Ty& _X, const _Ty& _Y)
	{return (_Y < _X); }
template<class _Ty> inline
	bool __cdecl operator<=(const _Ty& _X, const _Ty& _Y)
	{return (!(_Y < _X)); }
template<class _Ty> inline
	bool __cdecl operator>=(const _Ty& _X, const _Ty& _Y)
	{return (!(_X < _Y)); }
	};
};

#pragma pack(pop)
#line 327 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"

#line 329 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"






















#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"

 
 
 
#line 16 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
 
 
namespace std {
		
template<class _Ty> inline
	_Ty  *_Allocate(ptrdiff_t _N, _Ty  *)
	{if (_N < 0)
		_N = 0;
	return ((_Ty  *)operator new(
		(size_t)_N * sizeof (_Ty))); }
		
template<class _T1, class _T2> inline
	void _Construct(_T1  *_P, const _T2& _V)
	{new ((void  *)_P) _T1(_V); }
		
template<class _Ty> inline
	void _Destroy(_Ty  *_P)
	{(_P)->~_Ty(); }
inline void _Destroy(char  *_P)
	{}
inline void _Destroy(wchar_t  *_P)
	{}
		
template<class _Ty>
	class allocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef _Ty  *pointer;
	typedef const _Ty  *const_pointer;
	typedef _Ty & reference;
	typedef const _Ty & const_reference;
	typedef _Ty value_type;
	pointer address(reference _X) const
		{return (&_X); }
	const_pointer address(const_reference _X) const
		{return (&_X); }
	pointer allocate(size_type _N, const void *)
		{return (_Allocate((difference_type)_N, (pointer)0)); }
	char  *_Charalloc(size_type _N)
		{return (_Allocate((difference_type)_N,
			(char  *)0)); }
	void deallocate(void  *_P, size_type)
		{operator delete(_P); }
	void construct(pointer _P, const _Ty& _V)
		{_Construct(_P, _V); }
	void destroy(pointer _P)
		{_Destroy(_P); }
	size_t max_size() const
		{size_t _N = (size_t)(-1) / sizeof (_Ty);
		return (0 < _N ? _N : 1); }
	};
template<class _Ty, class _U> inline
	bool operator==(const allocator<_Ty>&, const allocator<_U>&)
	{return (true); }
template<class _Ty, class _U> inline
	bool operator!=(const allocator<_Ty>&, const allocator<_U>&)
	{return (false); }
		
class __declspec(dllimport) allocator<void> {
public:
	typedef void _Ty;
	typedef _Ty  *pointer;
	typedef const _Ty  *const_pointer;
	typedef _Ty value_type;
	};
};

#pragma pack(pop)
#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"

#line 88 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"






















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"






























































































































































































































































































































































#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"
namespace std {
		
template<class _II, class _OI> inline
	_OI copy(_II _F, _II _L, _OI _X)
	{if (0) ; else for (; _F != _L; ++_X, ++_F)
		*_X = *_F;
	return (_X); }
		
template<class _BI1, class _BI2> inline
	_BI2 copy_backward(_BI1 _F, _BI1 _L, _BI2 _X)
	{while (_F != _L)
		*--_X = *--_L;
	return (_X); }
		
template<class _II1, class _II2> inline
	bool equal(_II1 _F, _II1 _L, _II2 _X)
	{return (mismatch(_F, _L, _X).first == _L); }
		
template<class _II1, class _II2, class _Pr> inline
	bool equal(_II1 _F, _II1 _L, _II2 _X, _Pr _P)
	{return (mismatch(_F, _L, _X, _P).first == _L); }
		
template<class _FI, class _Ty> inline
	void fill(_FI _F, _FI _L, const _Ty& _X)
	{if (0) ; else for (; _F != _L; ++_F)
		*_F = _X; }
		
template<class _OI, class _Sz, class _Ty> inline
	void fill_n(_OI _F, _Sz _N, const _Ty& _X)
	{if (0) ; else for (; 0 < _N; --_N, ++_F)
		*_F = _X; }
		
template<class _II1, class _II2> inline
	bool lexicographical_compare(_II1 _F1, _II1 _L1,
		_II2 _F2, _II2 _L2)
	{if (0) ; else for (; _F1 != _L1 && _F2 != _L2; ++_F1, ++_F2)
		if (*_F1 < *_F2)
			return (true);
		else if (*_F2 < *_F1)
			return (false);
	return (_F1 == _L1 && _F2 != _L2); }
		
template<class _II1, class _II2, class _Pr> inline
	bool lexicographical_compare(_II1 _F1, _II1 _L1,
		_II2 _F2, _II2 _L2, _Pr _P)
	{if (0) ; else for (; _F1 != _L1 && _F2 != _L2; ++_F1, ++_F2)
		if (_P(*_F1, *_F2))
			return (true);
		else if (_P(*_F2, *_F1))
			return (false);
	return (_F1 == _L1 && _F2 != _L2); }
		




template<class _Ty> inline
	const _Ty& _cpp_max(const _Ty& _X, const _Ty& _Y)
	{return (_X < _Y ? _Y : _X); }
		
template<class _Ty, class _Pr> inline
	const _Ty& _cpp_max(const _Ty& _X, const _Ty& _Y, _Pr _P)
	{return (_P(_X, _Y) ? _Y : _X); }
		
template<class _Ty> inline
	const _Ty& _cpp_min(const _Ty& _X, const _Ty& _Y)
	{return (_Y < _X ? _Y : _X); }
		
template<class _Ty, class _Pr> inline
	const _Ty& _cpp_min(const _Ty& _X, const _Ty& _Y, _Pr _P)
	{return (_P(_Y, _X) ? _Y : _X); }
		
template<class _II1, class _II2> inline
	pair<_II1, _II2> mismatch(_II1 _F, _II1 _L, _II2 _X)
	{if (0) ; else for (; _F != _L && *_F == *_X; ++_F, ++_X)
		;
	return (pair<_II1, _II2>(_F, _X)); }
		
template<class _II1, class _II2, class _Pr> inline
	pair<_II1, _II2> mismatch(_II1 _F, _II1 _L, _II2 _X, _Pr _P)
	{if (0) ; else for (; _F != _L && _P(*_F, *_X); ++_F, ++_X)
		;
	return (pair<_II1, _II2>(_F, _X)); }
		
template<class _Ty> inline
	void swap(_Ty& _X, _Ty& _Y)
	{_Ty _Tmp = _X;
	_X = _Y, _Y = _Tmp; }
};

#pragma pack(pop)
#line 101 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"

#line 103 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"






















#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"
namespace std {
__declspec(dllimport) void __cdecl _Xlen();
__declspec(dllimport) void __cdecl _Xran();
		
template<class _E,
	class _Tr = char_traits<_E>,
	class _A = allocator<_E> >
	class basic_string {
public:
	typedef basic_string<_E, _Tr, _A> _Myt;
	typedef _A::size_type size_type;
	typedef _A::difference_type difference_type;
	typedef _A::pointer pointer;
	typedef _A::const_pointer const_pointer;
	typedef _A::reference reference;
	typedef _A::const_reference const_reference;
	typedef _A::value_type value_type;
	typedef _A::pointer iterator;
	typedef _A::const_pointer const_iterator;
	typedef reverse_iterator<const_iterator, value_type,
		const_reference, const_pointer, difference_type>
			const_reverse_iterator;
	typedef reverse_iterator<iterator, value_type,
		reference, pointer, difference_type>
			reverse_iterator;
	explicit basic_string(const _A& _Al = _A())
		: allocator(_Al) {_Tidy(); }
	basic_string(const _Myt& _X)
		: allocator(_X.allocator)
		{_Tidy(), assign(_X, 0, npos); }
	basic_string(const _Myt& _X, size_type _P, size_type _M,
		const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_X, _P, _M); }
	basic_string(const _E *_S, size_type _N,
		const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_S, _N); }
	basic_string(const _E *_S, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_S); }
	basic_string(size_type _N, _E _C, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(), assign(_N, _C); }
	typedef const_iterator _It;
	basic_string(_It _F, _It _L, const _A& _Al = _A())
		: allocator(_Al) {_Tidy(); assign(_F, _L); }
	~basic_string()
		{_Tidy(true); }
	typedef _Tr traits_type;
	typedef _A allocator_type;
	enum _Mref {_FROZEN = 255};
	static const size_type npos;
	_Myt& operator=(const _Myt& _X)
		{return (assign(_X)); }
	_Myt& operator=(const _E *_S)
		{return (assign(_S)); }
	_Myt& operator=(_E _C)
		{return (assign(1, _C)); }
	_Myt& operator+=(const _Myt& _X)
		{return (append(_X)); }
	_Myt& operator+=(const _E *_S)
		{return (append(_S)); }
	_Myt& operator+=(_E _C)
		{return (append(1, _C)); }
	_Myt& append(const _Myt& _X)
		{return (append(_X, 0, npos)); }
	_Myt& append(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S, size_type _M)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::copy(_Ptr + _Len, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& append(const _E *_S)
		{return (append(_S, _Tr::length(_S))); }
	_Myt& append(size_type _M, _E _C)
		{if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::assign(_Ptr + _Len, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& append(_It _F, _It _L)
		{return (replace(end(), end(), _F, _L)); }
	_Myt& assign(const _Myt& _X)
		{return (assign(_X, 0, npos)); }
	_Myt& assign(const _Myt& _X, size_type _P, size_type _M)
		{if (_X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_M < _N)
			_N = _M;
		if (this == &_X)
			erase((size_type)(_P + _N)), erase(0, _P);
		else if (0 < _N && _N == _X.size()
			&& _Refcnt(_X.c_str()) < _FROZEN - 1
			&& allocator == _X.allocator)
			{_Tidy(true);
			_Ptr = (_E *)_X.c_str();
			_Len = _X.size();
			_Res = _X.capacity();
			++_Refcnt(_Ptr); }
		else if (_Grow(_N, true))
			{_Tr::copy(_Ptr, &_X.c_str()[_P], _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S, size_type _N)
		{if (_Grow(_N, true))
			{_Tr::copy(_Ptr, _S, _N);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(const _E *_S)
		{return (assign(_S, _Tr::length(_S))); }
	_Myt& assign(size_type _N, _E _C)
		{if (_N == npos)
			_Xlen();
		if (_Grow(_N, true))
			{_Tr::assign(_Ptr, _N, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& assign(_It _F, _It _L)
		{return (replace(begin(), end(), _F, _L)); }
	_Myt& insert(size_type _P0, const _Myt& _X)
		{return (insert(_P0, _X, 0, npos)); }
	_Myt& insert(size_type _P0, const _Myt& _X, size_type _P,
		size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _Len <= _M)
			_Xlen();
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S, size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& insert(size_type _P0, const _E *_S)
		{return (insert(_P0, _S, _Tr::length(_S))); }
	_Myt& insert(size_type _P0, size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (npos - _Len <= _M)
			_Xlen();
		size_type _N;
		if (0 < _M && _Grow(_N = _Len + _M))
			{_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0, _Len - _P0);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	iterator insert(iterator _P, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, 1, _C);
		return (begin() + _P0); }
	void insert(iterator _P, size_type _M, _E _C)
		{size_type _P0 = _Pdif(_P, begin());
		insert(_P0, _M, _C); }
	void insert(iterator _P, _It _F, _It _L)
		{replace(_P, _P, _F, _L); }
	_Myt& erase(size_type _P0 = 0, size_type _M = npos)
		{if (_Len < _P0)
			_Xran();
		_Split();
		if (_Len - _P0 < _M)
			_M = _Len - _P0;
		if (0 < _M)
			{_Tr::move(_Ptr + _P0, _Ptr + _P0 + _M,
				_Len - _P0 - _M);
			size_type _N = _Len - _M;
			if (_Grow(_N))
				_Eos(_N); }
		return (*this); }
	iterator erase(iterator _P)
		{size_t _M = _Pdif(_P, begin());
		erase(_M, 1);
		return (_Psum(_Ptr, _M)); }
	iterator erase(iterator _F, iterator _L)
		{size_t _M = _Pdif(_F, begin());
		erase(_M, _Pdif(_L, _F));
		return (_Psum(_Ptr, _M)); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X)
		{return (replace(_P0, _N0, _X, 0, npos)); }
	_Myt& replace(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M)
		{if (_Len < _P0 || _X.size() < _P)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _N = _X.size() - _P;
		if (_N < _M)
			_M = _N;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, &_X.c_str()[_P], _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S,
		size_type _M)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
			_Tr::copy(_Ptr + _P0, _S, _M);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(size_type _P0, size_type _N0, const _E *_S)
		{return (replace(_P0, _N0, _S, _Tr::length(_S))); }
	_Myt& replace(size_type _P0, size_type _N0,
		size_type _M, _E _C)
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		if (npos - _M <= _Len - _N0)
			_Xlen();
		_Split();
		size_type _Nm = _Len - _N0 - _P0;
		if (_M < _N0)
			_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0, _Nm);
		size_type _N;
		if ((0 < _M || 0 < _N0) && _Grow(_N = _Len + _M - _N0))
			{if (_N0 < _M)
				_Tr::move(_Ptr + _P0 + _M, _Ptr + _P0 + _N0,
					_Nm);
			_Tr::assign(_Ptr + _P0, _M, _C);
			_Eos(_N); }
		return (*this); }
	_Myt& replace(iterator _F, iterator _L, const _Myt& _X)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _X)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S,
		size_type _M)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S, _M)); }
	_Myt& replace(iterator _F, iterator _L, const _E *_S)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _S)); }
	_Myt& replace(iterator _F, iterator _L,	size_type _M, _E _C)
		{return (replace(
			_Pdif(_F, begin()), _Pdif(_L, _F), _M, _C)); }
	_Myt& replace(iterator _F1, iterator _L1,
		_It _F2, _It _L2)
		{size_type _P0 = _Pdif(_F1, begin());
		size_type _M = 0;
		_Distance(_F2, _L2, _M);
		replace(_P0, _Pdif(_L1, _F1), _M, _E(0));
		if (0) ; else for (_F1 = begin() + _P0; 0 < _M; ++_F1, ++_F2, --_M)
			*_F1 = *_F2;
		return (*this); }
	iterator begin()
		{_Freeze();
		return (_Ptr); }
	const_iterator begin() const
		{return (_Ptr); }
	iterator end()
		{_Freeze();
		return ((iterator)_Psum(_Ptr, _Len)); }
	const_iterator end() const
		{return ((const_iterator)_Psum(_Ptr, _Len)); }
	reverse_iterator rbegin()
		{return (reverse_iterator(end())); }
	const_reverse_iterator rbegin() const
		{return (const_reverse_iterator(end())); }
	reverse_iterator rend()
		{return (reverse_iterator(begin())); }
	const_reverse_iterator rend() const
		{return (const_reverse_iterator(begin())); }
	reference at(size_type _P0)
		{if (_Len <= _P0)
			_Xran();
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference at(size_type _P0) const
		{if (_Len <= _P0)
			_Xran();
		return (_Ptr[_P0]); }
	reference operator[](size_type _P0)
		{if (_Len < _P0 || _Ptr == 0)
			return ((reference)*_Nullstr());
		_Freeze();
		return (_Ptr[_P0]); }
	const_reference operator[](size_type _P0) const
		{if (_Ptr == 0)
			return (*_Nullstr());
		else
			return (_Ptr[_P0]); }
	const _E *c_str() const
		{return (_Ptr == 0 ? _Nullstr() : _Ptr); }
	const _E *data() const
		{return (c_str()); }
	size_type length() const
		{return (_Len); }
	size_type size() const
		{return (_Len); }
	size_type max_size() const
		{size_type _N = allocator.max_size();
		return (_N <= 2 ? 1 : _N - 2); }
	void resize(size_type _N, _E _C)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _C); }
	void resize(size_type _N)
		{_N <= _Len ? erase(_N) : append(_N - _Len, _E(0)); }
	size_type capacity() const
		{return (_Res); }
	void reserve(size_type _N = 0)
		{if (_Res < _N)
			_Grow(_N); }
	bool empty() const
		{return (_Len == 0); }
	size_type copy(_E *_S, size_type _N, size_type _P0 = 0) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N)
			_N = _Len - _P0;
		if (0 < _N)
			_Tr::copy(_S, _Ptr + _P0, _N);
		return (_N); }
	void swap(_Myt& _X)
		{if (allocator == _X.allocator)
			{std::swap(_Ptr, _X._Ptr);
			std::swap(_Len, _X._Len);
			std::swap(_Res, _X._Res); }
		else
			{_Myt _Ts = *this; *this = _X, _X = _Ts; }}
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	size_type find(const _Myt& _X, size_type _P = 0) const
		{return (find(_X.c_str(), _P, _X.size())); }
	size_type find(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0 && _P <= _Len)
			return (_P);
		size_type _Nm;
		if (_P < _Len && _N <= (_Nm = _Len - _P))
			{const _E *_U, *_V;
			if (0) ; else for (_Nm -= _N - 1, _V = _Ptr + _P;
				(_U = _Tr::find(_V, _Nm, *_S)) != 0;
				_Nm -= _U - _V + 1, _V = _U + 1)
				if (_Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find(const _E *_S, size_type _P = 0) const
		{return (find(_S, _P, _Tr::length(_S))); }
	size_type find(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type rfind(const _Myt& _X, size_type _P = npos) const
		{return (rfind(_X.c_str(), _P, _X.size())); }
	size_type rfind(const _E *_S, size_type _P,
		size_type _N) const
		{if (_N == 0)
			return (_P < _Len ? _P : _Len);
		if (_N <= _Len)
			if (0) ; else for (const _E *_U = _Ptr +
				+ (_P < _Len - _N ? _P : _Len - _N); ; --_U)
				if (_Tr::eq(*_U, *_S)
					&& _Tr::compare(_U, _S, _N) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type rfind(const _E *_S, size_type _P = npos) const
		{return (rfind(_S, _P, _Tr::length(_S))); }
	size_type rfind(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_of(_X.c_str(), _P, _X.size())); }
	size_type find_first_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && _P < _Len)
			{const _E *const _V = _Ptr + _Len;
			if (0) ; else for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_of(const _E *_S, size_type _P = 0) const
		{return (find_first_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_of(_E _C, size_type _P = 0) const
		{return (find((const _E *)&_C, _P, 1)); }
	size_type find_last_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (0 < _N && 0 < _Len)
			if (0) ; else for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) != 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_of(_E _C, size_type _P = npos) const
		{return (rfind((const _E *)&_C, _P, 1)); }
	size_type find_first_not_of(const _Myt& _X,
		size_type _P = 0) const
		{return (find_first_not_of(_X.c_str(), _P,
			_X.size())); }
	size_type find_first_not_of(const _E *_S, size_type _P,
		size_type _N) const
		{if (_P < _Len)
			{const _E *const _V = _Ptr + _Len;
			if (0) ; else for (const _E *_U = _Ptr + _P; _U < _V; ++_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr); }
		return (npos); }
	size_type find_first_not_of(const _E *_S,
		size_type _P = 0) const
		{return (find_first_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_first_not_of(_E _C, size_type _P = 0) const
		{return (find_first_not_of((const _E *)&_C, _P, 1)); }
	size_type find_last_not_of(const _Myt& _X,
		size_type _P = npos) const
		{return (find_last_not_of(_X.c_str(), _P, _X.size())); }
	size_type find_last_not_of(const _E *_S, size_type _P,
		 size_type _N) const
		{if (0 < _Len)
			if (0) ; else for (const _E *_U = _Ptr
				+ (_P < _Len ? _P : _Len - 1); ; --_U)
				if (_Tr::find(_S, _N, *_U) == 0)
					return (_U - _Ptr);
				else if (_U == _Ptr)
					break;
		return (npos); }
	size_type find_last_not_of(const _E *_S,
		size_type _P = npos) const
		{return (find_last_not_of(_S, _P, _Tr::length(_S))); }
	size_type find_last_not_of(_E _C, size_type _P = npos) const
		{return (find_last_not_of((const _E *)&_C, _P, 1)); }
	_Myt substr(size_type _P = 0, size_type _M = npos) const
		{return (_Myt(*this, _P, _M)); }
	int compare(const _Myt& _X) const
		{return (compare(0, _Len, _X.c_str(), _X.size())); }
	int compare(size_type _P0, size_type _N0,
		const _Myt& _X) const
		{return (compare(_P0, _N0, _X, 0, npos)); }
	int compare(size_type _P0, size_type _N0, const _Myt& _X,
		size_type _P, size_type _M) const
		{if (_X.size() < _P)
			_Xran();
		if (_X._Len - _P < _M)
			_M = _X._Len - _P;
		return (compare(_P0, _N0, _X.c_str() + _P, _M)); }
	int compare(const _E *_S) const
		{return (compare(0, _Len, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S) const
		{return (compare(_P0, _N0, _S, _Tr::length(_S))); }
	int compare(size_type _P0, size_type _N0, const _E *_S,
		size_type _M) const
		{if (_Len < _P0)
			_Xran();
		if (_Len - _P0 < _N0)
			_N0 = _Len - _P0;
		size_type _Ans = _Tr::compare(_Psum(_Ptr, _P0), _S,
			_N0 < _M ? _N0 : _M);
		return (_Ans != 0 ? _Ans : _N0 < _M ? -1
			: _N0 == _M ? 0 : +1); }
	_A get_allocator() const
		{return (allocator); }
protected:
	_A allocator;
private:
	enum {_MIN_SIZE = sizeof (_E) <= 32 ? 31 : 7};
	void _Copy(size_type _N)
		{size_type _Ns = _N | _MIN_SIZE;
		if (max_size() < _Ns)
			_Ns = _N;
		_E *_S;
		try {
			_S = allocator.allocate(_Ns + 2, (void *)0);
		} catch (...) {
			_Ns = _N;
			_S = allocator.allocate(_Ns + 2, (void *)0);
		}
		if (0 < _Len)
			_Tr::copy(_S + 1, _Ptr, _Len);
		size_type _Olen = _Len;
		_Tidy(true);
		_Ptr = _S + 1;
		_Refcnt(_Ptr) = 0;
		_Res = _Ns;
		_Eos(_Olen); }
	void _Eos(size_type _N)
		{_Tr::assign(_Ptr[_Len = _N], _E(0)); }
	void _Freeze()
		{if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			_Grow(_Len);
		if (_Ptr != 0)
			_Refcnt(_Ptr) = _FROZEN; }
	bool _Grow(size_type _N, bool _Trim = false)
		{if (max_size() < _N)
			_Xlen();
		if (_Ptr != 0
			&& _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			if (_N == 0)
				{--_Refcnt(_Ptr), _Tidy();
				return (false); }
			else
				{_Copy(_N);
				return (true); }
		if (_N == 0)
			{if (_Trim)
				_Tidy(true);
			else if (_Ptr != 0)
				_Eos(0);
			return (false); }
		else
			{if (_Trim && (_MIN_SIZE < _Res || _Res < _N))
				{_Tidy(true);
				_Copy(_N); }
			else if (!_Trim && _Res < _N)
				_Copy(_N);
			return (true); }}
	static const _E *_Nullstr()
		{static const _E _C = _E(0);
		return (&_C); }
	static size_type _Pdif(const_pointer _P2, const_pointer _P1)
		{return (_P2 == 0 ? 0 : _P2 - _P1); }
	static const_pointer _Psum(const_pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	static pointer _Psum(pointer _P, size_type _N)
		{return (_P == 0 ? 0 : _P + _N); }
	unsigned char& _Refcnt(const _E *_U)
		{return (((unsigned char *)_U)[-1]); }
	void _Split()
		{if (_Ptr != 0 && _Refcnt(_Ptr) != 0 && _Refcnt(_Ptr) != _FROZEN)
			{_E *_Temp = _Ptr;
			_Tidy(true);
			assign(_Temp); }}
	void _Tidy(bool _Built = false)
		{if (!_Built || _Ptr == 0)
			;
		else if (_Refcnt(_Ptr) == 0 || _Refcnt(_Ptr) == _FROZEN)
			allocator.deallocate(_Ptr - 1, _Res + 2);
		else
			--_Refcnt(_Ptr);
		_Ptr = 0, _Len = 0, _Res = 0; }
	_E *_Ptr;
	size_type _Len, _Res;
	};
template<class _E, class _Tr, class _A>
	const basic_string<_E, _Tr, _A>::size_type
		basic_string<_E, _Tr, _A>::npos = -1;


#pragma warning(disable:4231) 

extern template class __declspec(dllimport) basic_string<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;

#pragma warning(default:4231) 
#line 605 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"

typedef basic_string<char, char_traits<char>, allocator<char> >
	string;

typedef basic_string<wchar_t, char_traits<wchar_t>,
	allocator<wchar_t> > wstring;

};

#pragma pack(pop)
#line 616 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"

#line 618 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"


#pragma pack(push,8)
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"
namespace std {
		
class __declspec(dllimport) logic_error : public exception {
public:
	explicit logic_error(const string& _S)
		: exception(""), _Str(_S) {}
	virtual ~logic_error()
		{}
	virtual const char *what() const
		{return (_Str.c_str()); }
protected:
	virtual void _Doraise() const
		{throw (*this); }
private:
	string _Str;
	};
		
class __declspec(dllimport) domain_error : public logic_error {
public:
	explicit domain_error(const string& _S)
		: logic_error(_S) {}
	virtual ~domain_error()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class invalid_argument : public logic_error {
public:
	explicit invalid_argument(const string& _S)
		: logic_error(_S) {}
	virtual ~invalid_argument()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) length_error : public logic_error {
public:
	explicit length_error(const string& _S)
		: logic_error(_S) {}
	virtual ~length_error()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) out_of_range : public logic_error {
public:
	explicit out_of_range(const string& _S)
		: logic_error(_S) {}
	virtual ~out_of_range()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) runtime_error : public exception {
public:
	explicit runtime_error(const string& _S)
		: exception(""), _Str(_S) {}
	virtual ~runtime_error()
		{}
	virtual const char *what() const
		{return (_Str.c_str()); }
protected:
	virtual void _Doraise() const
		{throw (*this); }
private:
	string _Str;
	};
		
class __declspec(dllimport) overflow_error : public runtime_error {
public:
	explicit overflow_error(const string& _S)
		: runtime_error(_S) {}
	virtual ~overflow_error()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) underflow_error : public runtime_error {
public:
	explicit underflow_error(const string& _S)
		: runtime_error(_S) {}
	virtual ~underflow_error()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) range_error : public runtime_error {
public:
	explicit range_error(const string& _S)
		: runtime_error(_S) {}
	virtual ~range_error()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
};

#pragma pack(pop)
#line 122 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"

#line 124 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"





#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"





















 

#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"


















#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"






























































#line 44 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"


#pragma pack(push,8)
#line 48 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"

class type_info {
public:
    __declspec(dllimport) virtual ~type_info();
    __declspec(dllimport) int operator==(const type_info& rhs) const;
    __declspec(dllimport) int operator!=(const type_info& rhs) const;
    __declspec(dllimport) int before(const type_info& rhs) const;
    __declspec(dllimport) const char* name() const;
    __declspec(dllimport) const char* raw_name() const;
private:
    void *_m_data;
    char _m_d_name[1];
    type_info(const type_info& rhs);
    type_info& operator=(const type_info& rhs);
};



 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"


























































































#line 67 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"

 namespace std {
		
class __declspec(dllimport) bad_cast : public exception {
public:
	bad_cast(const char *_S = "bad cast") throw ()
		: exception(_S) {}
	virtual ~bad_cast() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
class __declspec(dllimport) bad_typeid : public exception {
public:
	bad_typeid(const char *_S = "bad typeid") throw ()
		: exception(_S) {}
	virtual ~bad_typeid() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};

class __declspec(dllimport) __non_rtti_object : public bad_typeid {
public:
    __non_rtti_object(const char * what_arg) : bad_typeid(what_arg) {}
};
using ::type_info;
 };
using std::__non_rtti_object;









#pragma pack(pop)
#line 110 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"

#line 112 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\typeinfo"






#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo.h"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"














#pragma once
#line 17 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"






#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"



extern "C" {
#line 29 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"













































#line 75 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"
















#line 92 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"































__declspec(dllimport) extern unsigned short _ctype[];




__declspec(dllimport) unsigned short ** __cdecl __p__pctype(void);


__declspec(dllimport) wctype_t ** __cdecl ___p__pwctype(void);








#line 141 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

#line 143 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

#line 145 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"







				












__declspec(dllimport) int __cdecl _isctype(int, int);
__declspec(dllimport) int __cdecl isalpha(int);
__declspec(dllimport) int __cdecl isupper(int);
__declspec(dllimport) int __cdecl islower(int);
__declspec(dllimport) int __cdecl isdigit(int);
__declspec(dllimport) int __cdecl isxdigit(int);
__declspec(dllimport) int __cdecl isspace(int);
__declspec(dllimport) int __cdecl ispunct(int);
__declspec(dllimport) int __cdecl isalnum(int);
__declspec(dllimport) int __cdecl isprint(int);
__declspec(dllimport) int __cdecl isgraph(int);
__declspec(dllimport) int __cdecl iscntrl(int);
__declspec(dllimport) int __cdecl toupper(int);
__declspec(dllimport) int __cdecl tolower(int);
__declspec(dllimport) int __cdecl _tolower(int);
__declspec(dllimport) int __cdecl _toupper(int);
__declspec(dllimport) int __cdecl __isascii(int);
__declspec(dllimport) int __cdecl __toascii(int);
__declspec(dllimport) int __cdecl __iscsymf(int);
__declspec(dllimport) int __cdecl __iscsym(int);

#line 187 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

































#line 221 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"



































































#line 289 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"







































#line 329 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"
#line 330 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"


















































#line 381 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

























#line 407 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

#line 409 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"

#line 411 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"


}
#line 415 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"


#line 418 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ctype.h"
#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo.h"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"














#pragma once
#line 17 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"






#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"







#pragma pack(push,8)
#line 33 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"


extern "C" {
#line 37 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"













































#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"







#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"



























struct lconv {
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	};

#line 140 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"












__declspec(dllimport) char * __cdecl setlocale(int, const char *);
__declspec(dllimport) struct lconv * __cdecl localeconv(void);










#line 165 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"


}
#line 169 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"


#pragma pack(pop)
#line 173 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"

#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\locale.h"
#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo.h"






		











		



















typedef struct _Collvec {
	unsigned long _Hand;	
	unsigned int _Page;		
	} _Collvec;

typedef struct _Ctypevec {
	unsigned long _Hand;	
	unsigned int _Page;		
	const short *_Table;
	int _Delfl;
	} _Ctypevec;

typedef struct _Cvtvec {
	unsigned long _Hand;	
	unsigned int _Page;		
	} _Cvtvec;

		
extern "C" {
__declspec(dllimport) _Collvec __cdecl _Getcoll();
__declspec(dllimport) _Ctypevec __cdecl _Getctype();
__declspec(dllimport) _Cvtvec __cdecl _Getcvt();
char *__cdecl _Getdays();
char *__cdecl _Getmonths();
void *__cdecl _Gettnames();
__declspec(dllimport) int __cdecl _Mbrtowc(wchar_t *, const char *, size_t,
	mbstate_t *, const _Cvtvec *);
extern float __cdecl _Stof(const char *, char **, long);
extern double __cdecl _Stod(const char *, char **, long);
extern long double __cdecl _Stold(const char *, char **, long);
__declspec(dllimport) int __cdecl _Strcoll(const char *, const char *,
	const char *, const char *, const _Collvec *);
size_t __cdecl _Strftime(char *, size_t, const char *,
	const struct tm *, void *);
__declspec(dllimport) size_t __cdecl _Strxfrm(char *, char *,
	const char *, const char *, const _Collvec *);
__declspec(dllimport) int __cdecl _Tolower(int, const _Ctypevec *);
__declspec(dllimport) int __cdecl _Toupper(int, const _Ctypevec *);
__declspec(dllimport) int __cdecl _Wcrtomb(char *, wchar_t, mbstate_t *,
	const _Cvtvec *);
__declspec(dllimport) int __cdecl _Wcscoll(const wchar_t *, const wchar_t *,
	const wchar_t *, const wchar_t *, const _Collvec *);
__declspec(dllimport) size_t __cdecl _Wcsxfrm(wchar_t *, wchar_t *,
	const wchar_t *, const wchar_t *, const _Collvec *);
}
#line 90 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo.h"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
















#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"






























































#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"


#pragma pack(push,8)
#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"
namespace std {

		
class __declspec(dllimport) _Timevec {
public:
	_Timevec(void *_P = 0)
		: _Ptr(_P) {}
	_Timevec(const _Timevec& _Rhs)
		{*this = _Rhs; }
	~_Timevec()
		{free(_Ptr); }
	_Timevec& operator=(const _Timevec& _Rhs)
		{_Ptr = _Rhs._Ptr;
		((_Timevec *)&_Rhs)->_Ptr = 0;
		return (*this); }
	void *_Getptr() const
		{return (_Ptr); }
private:
	void *_Ptr;
	};
		
class __declspec(dllimport) _Locinfo {
public:
	typedef ::_Collvec _Collvec;
	typedef ::_Ctypevec _Ctypevec;
	typedef ::_Cvtvec _Cvtvec;
	typedef std::_Timevec _Timevec;
	_Locinfo(const char * = "C");
	_Locinfo(int, const char *);
	~_Locinfo();
	_Locinfo& _Addcats(int, const char *);
	string _Getname() const
		{return (_Nname); }
	_Collvec _Getcoll() const
		{return (::_Getcoll()); }
	_Ctypevec _Getctype() const
		{return (::_Getctype()); }
	_Cvtvec _Getcvt() const
		{return (::_Getcvt()); }
	const lconv *_Getlconv() const
		{return (localeconv()); }
	_Timevec _Gettnames() const
		{return (_Timevec(::_Gettnames())); }
	const char *_Getdays() const
		{char *_S = ::_Getdays();
		if (_S != 0)
			{((_Locinfo *)this)->_Days = _S, free(_S); }
		return (_Days.size() != 0 ? _Days.c_str()
				: ":Sun:Sunday:Mon:Monday:Tue:Tuesday"
				":Wed:Wednesday:Thu:Thursday:Fri:Friday"
				":Sat:Saturday"); }
	const char *_Getmonths() const
		{char *_S = ::_Getmonths();
		if (_S != 0)
			{((_Locinfo *)this)->_Months = _S, free(_S); }
		return (_Months.size() != 0 ? _Months.c_str()
				: ":Jan:January:Feb:February:Mar:March"
				":Apr:April:May:May:Jun:June"
				":Jul:July:Aug:August:Sep:September"
				":Oct:October:Nov:November:Dec:December"); }
	const char *_Getfalse() const
		{return ("false"); }
	const char *_Gettrue() const
		{return ("true"); }
	const char *_Getno() const
		{return ("no"); }
	const char *_Getyes() const
		{return ("yes"); }
private:
	string _Days, _Months, _Oname, _Nname;
	};
		
template<class _E> inline
	int __cdecl _Strcoll(const _E *_F1, const _E *_L1,
		const _E *_F2, const _E *_L2, const _Locinfo::_Collvec *)
	{if (0) ; else for (; _F1 != _L1 && _F2 != _L2; ++_F1, ++_F2)
		if (*_F1 < *_F2)
			return (-1);
		else if (*_F2 < *_F1)
			return (+1);
	return (_F2 != _L2 ? -1 : _F1 != _L1 ? +1 : 0); }
template<class _E> inline
	size_t __cdecl _Strxfrm(_E *_F1, _E *_L1,
		const _E *_F2, const _E *_L2, const _Locinfo::_Collvec *)
	{size_t _N = _L2 - _F2;
	if (_N <= _L1 - _F1)
		memcpy(_F1, _F2, _N * sizeof (_E));
	return (_N); }
};

#pragma pack(pop)
#line 103 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"

#line 105 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocinfo"





#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"


#pragma pack(push,8)
#line 13 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
namespace std {
                
class __declspec(dllimport) locale {
public:
        enum _Category {collate = ((1 << (1)) >> 1), ctype = ((1 << (2)) >> 1),
                monetary = ((1 << (3)) >> 1), numeric = ((1 << (4)) >> 1),
                time = ((1 << (5)) >> 1), messages = ((1 << (6)) >> 1),
                all = (((1 << (7)) >> 1) - 1), none = 0};
        typedef int category;
                        
        class __declspec(dllimport) id {
        public:
                operator size_t()
                        {_Lockit _Lk;
                        if (_Id == 0)
                                _Id = ++_Id_cnt;
                        return (_Id); }
        private:
                size_t _Id;
                static int _Id_cnt;
                };
        class _Locimp;
                        
        class __declspec(dllimport) facet {
                friend class locale;
                friend class _Locimp;
        public:
                static size_t __cdecl _Getcat()
                        {return ((size_t)(-1)); }
                void _Incref()
                        {_Lockit _Lk;
                        if (_Refs < (size_t)(-1))
                                ++_Refs; }
                facet *_Decref()
                        {_Lockit _Lk;
                        if (0 < _Refs && _Refs < (size_t)(-1))
                                --_Refs;
                        return (_Refs == 0 ? this : 0); }
        public:
                virtual ~facet()
                        {}
        protected:
                explicit facet(size_t _R = 0)
                        : _Refs(_R) {}
        private:
                facet(const facet&);    
                const facet& operator=(const facet&);   
                size_t _Refs;
                };
                        
        class _Locimp : public facet {
        public:
                ~_Locimp();
        private:
                friend class locale;
                _Locimp(bool _Xp = false);
                _Locimp(const _Locimp&);
                void _Addfac(facet *, size_t);
                static _Locimp *__cdecl _Makeloc(const _Locinfo&,
                        category, _Locimp *, const locale *);
                static void __cdecl _Makewloc(const _Locinfo&,
                        category, _Locimp *, const locale *);
                static void __cdecl _Makexloc(const _Locinfo&,
                        category, _Locimp *, const locale *);
                facet **_Fv;
                size_t _Nfv;
                category _Cat;
                bool _Xpar;
                string _Name;
                static __declspec(dllimport) _Locimp *_Clocptr, *_Global;
                };
        locale& _Addfac(facet *, size_t, size_t);
        bool operator()(const string&, const string&) const;
        locale() throw ()
                : _Ptr(_Init())
                {_Lockit _lk;
                _Locimp::_Global->_Incref(); }
        locale(_Uninitialized)
                {}
        locale(const locale& _X) throw ()
                : _Ptr(_X._Ptr)
                {_Ptr->_Incref(); }
        locale(const locale&, const locale&, category);
        explicit locale(const char *, category = all);
        locale(const locale&, const char *, category);
        ~locale() throw ()
                {if (_Ptr != 0)
                        delete _Ptr->_Decref(); }
        locale& operator=(const locale& _X) throw ()
                {if (_Ptr != _X._Ptr)
                        {delete _Ptr->_Decref();
                        _Ptr = _X._Ptr;
                        _Ptr->_Incref(); }
                return (*this); }
        string name() const
                {return (_Ptr->_Name); }
        const facet *_Getfacet(size_t _Id, bool _Xp = false) const;
        bool _Iscloc() const;
        bool operator==(const locale& _X) const;
        bool operator!=(const locale& _X) const
                {return (!(*this == _X)); }
        static const locale& __cdecl classic();
        static locale __cdecl global(const locale&);
        static locale __cdecl empty();
private:
        locale(_Locimp *_P)
                : _Ptr(_P) {}
        static _Locimp *__cdecl _Init();
        static void __cdecl _Tidy();
        _Locimp *_Ptr;
        };
;
                
template<class _F>
        class _Tidyfac {
public:
        static _F *__cdecl _Save(_F *_Fac)
                {_Lockit _Lk;
                _Facsav = _Fac;
                _Facsav->_Incref();
                atexit(_Tidy);
                return (_Fac); }
        static void __cdecl _Tidy()
                {_Lockit _Lk;
                delete _Facsav->_Decref();
                _Facsav = 0; }
private:
        static _F *_Facsav;
        };
template<class _F>
        _F *_Tidyfac<_F>::_Facsav = 0;
 
 
 
template<class _F> inline
        locale _Addfac(locale _X, _F *_Fac)
                {_Lockit _Lk;
                return (_X._Addfac(_Fac, _F::id, _F::_Getcat())); }
template<class _F> inline
        const _F& __cdecl use_facet(const locale& _L, const _F *,
                bool _Cfacet)
        {static const locale::facet *_Psave = 0;
        _Lockit _Lk;
        size_t _Id = _F::id;
        const locale::facet *_Pf = _L._Getfacet(_Id, true);
        if (_Pf != 0)
                ;
        else if (!_Cfacet || !_L._Iscloc())
                throw bad_cast("missing locale facet");
        else if (_Psave == 0)
                _Pf = _Psave = _Tidyfac<_F>::_Save(new _F);
        else
                _Pf = _Psave;
        return (*(const _F *)_Pf); }
                

template<class _E> inline
        int _Narrow(_E _C)      
        {return ((unsigned char)(char)_C); }
inline int _Narrow(wchar_t _C)
        {return (wctob(_C)); }
                

template<class _E> inline
        _E _Widen(char _Ch, _E *)       
        {return (_Ch); }
inline wchar_t _Widen(char _Ch, wchar_t *)
        {return (btowc(_Ch)); }
                
template<class _E, class _II> inline
        int __cdecl _Getloctxt(_II& _F, _II& _L, size_t _N,
                const _E *_S)
        {if (0) ; else for (size_t _I = 0; _S[_I] != (_E)0; ++_I)
                if (_S[_I] == _S[0])
                        ++_N;
        string _Str(_N, '\0');
        int _Ans = -2;
        if (0) ; else for (size_t _J = 1; ; ++_J, ++_F, _Ans = -1)
                {bool  _Pfx;
                size_t _I, _K;
                if (0) ; else for (_I = 0, _K = 0, _Pfx = false; _K < _N; ++_K)
                        {if (0) ; else for (; _S[_I] != (_E)0 && _S[_I] != _S[0]; ++_I)
                                ;
                        if (_Str[_K] != '\0')
                                _I += _Str[_K];
                        else if (_S[_I += _J] == _S[0] || _S[_I] == (_E)0)
                                {_Str[_K] = _J < 127 ? _J : 127;
                                _Ans = _K; }
                        else if (_F == _L || _S[_I] != *_F)
                                _Str[_K] = _J < 127 ? _J : 127;
                        else
                                _Pfx = true; }
                if (!_Pfx || _F == _L)
                        break; }
        return (_Ans); }
                

template<class _E> inline
        _E *__cdecl _Maklocstr(const char *_S, _E *)
        {size_t _L = strlen(_S) + 1;
        _E *_X = new _E[_L];
        if (0) ; else for (_E *_P = _X; 0 < _L; --_L, ++_P, ++_S)
                *_P = _Widen(*_S, (_E *)0);
        return (_X); }
                
class __declspec(dllimport) codecvt_base : public locale::facet {
public:
        enum _Result {ok, partial, error, noconv};
        typedef int result;
        codecvt_base(size_t _R = 0)
                : locale::facet(_R) {}
        bool always_noconv() const throw ()
                {return (do_always_noconv()); }
        int max_length() const throw ()
                {return (do_max_length()); }
        int encoding() const throw ()
                {return (do_encoding()); }
protected:
        virtual bool do_always_noconv() const throw ()
                {return (true); }
        virtual int do_max_length() const throw ()
                {return (1); }
        virtual int do_encoding() const throw ()
                {return (1); }
        };
;
                
template<class _E, class _To, class _St>
        class codecvt : public codecvt_base {
public:
        typedef _E from_type;
        typedef _To to_type;
        typedef _St state_type;
        result in(_St& _State,
                const _To *_F1, const _To *_L1, const _To *& _Mid1,
                _E *_F2, _E *_L2, _E *& _Mid2) const
                {return (do_in(_State,
                        _F1, _L1, _Mid1, _F2, _L2, _Mid2)); }
        result out(_St& _State,
                const _E *_F1, const _E *_L1, const _E *& _Mid1,
                _To *_F2, _To *_L2, _To *& _Mid2) const
                {return (do_out(_State,
                        _F1, _L1, _Mid1, _F2, _L2, _Mid2)); }
        int length(_St& _State, const _E *_F1,
                const _E *_L1, size_t _N2) const throw ()
                {return (do_length(_State, _F1, _L1, _N2)); }
        static locale::id id;
        explicit codecvt(size_t _R = 0)
                : codecvt_base(_R) {_Init(_Locinfo()); }
        codecvt(const _Locinfo& _Lobj, size_t _R = 0)
                : codecvt_base(_R) {_Init(_Lobj); }
        static size_t __cdecl _Getcat()
                {return (2); }
public:
        virtual ~codecvt()
                {};
protected:
        void _Init(const _Locinfo& _Lobj)
                {_Cvt = _Lobj._Getcvt(); }
        virtual result do_in(_St& _State,
                const _To *_F1, const _To *, const _To *& _Mid1,
                _E *_F2, _E *, _E *& _Mid2) const
                {_Mid1 = _F1, _Mid2 = _F2;
                return (noconv); }
        virtual result do_out(_St& _State,
                const _E *_F1, const _E *, const _E *& _Mid1,
                _To *_F2, _To *, _To *& _Mid2) const
                {_Mid1 = _F1, _Mid2 = _F2;
                return (noconv); }
        virtual int do_length(_St& _State, const _E *_F1,
                const _E *_L1, size_t _N2) const throw ()
                {return (_N2 < _L1 - _F1 ? _N2 : _L1 - _F1); }
private:
        _Locinfo::_Cvtvec _Cvt;
        };
template<class _E, class _To, class _St>
        locale::id codecvt<_E, _To, _St>::id;
                
class __declspec(dllimport) codecvt<wchar_t, char, mbstate_t> : public codecvt_base {
public:
        typedef wchar_t _E;
        typedef char _To;
        typedef mbstate_t _St;
        typedef _E from_type;
        typedef _To to_type;
        typedef _St state_type;
        result in(_St& _State,
                const _To *_F1, const _To *_L1, const _To *& _Mid1,
                _E *_F2, _E *_L2, _E *& _Mid2) const
                {return (do_in(_State,
                        _F1, _L1, _Mid1, _F2, _L2, _Mid2)); }
        result out(_St& _State,
                const _E *_F1, const _E *_L1, const _E *& _Mid1,
                _To *_F2, _To *_L2, _To *& _Mid2) const
                {return (do_out(_State,
                        _F1, _L1, _Mid1, _F2, _L2, _Mid2)); }
        int length(_St& _State, const _E *_F1,
                const _E *_L1, size_t _N2) const throw ()
                {return (do_length(_State, _F1, _L1, _N2)); }
        static locale::id id;
        explicit codecvt(size_t _R = 0)
                : codecvt_base(_R) {_Init(_Locinfo()); }
        codecvt(const _Locinfo& _Lobj, size_t _R = 0)
                : codecvt_base(_R) {_Init(_Lobj); }
        static size_t __cdecl _Getcat()
                {return (2); }
public:
        virtual ~codecvt()
                {};
protected:
        void _Init(const _Locinfo& _Lobj)
                {_Cvt = _Lobj._Getcvt(); }
        virtual result do_in(_St& _State,
                const _To *_F1, const _To *_L1, const _To *& _Mid1,
                _E *_F2, _E *_L2, _E *& _Mid2) const
                {_Mid1 = _F1, _Mid2 = _F2;
                result _Ans = _Mid1 == _L1 ? ok : partial;
                int _N;
                while (_Mid1 != _L1 && _Mid2 != _L2)
                        switch (_N =
                                _Mbrtowc(_Mid2, _Mid1, _L1 - _Mid1,
                                        &_State, &_Cvt))
                        {case -2:
                                _Mid1 = _L1;
                                return (_Ans);
                        case -1:
                                return (error);
                        case 0:
                                _N = strlen(_Mid1) + 1;
                        default:        
                                _Mid1 += _N, ++_Mid2, _Ans = ok; }
                return (_Ans); }
        virtual result do_out(_St& _State,
                const _E *_F1, const _E *_L1, const _E *& _Mid1,
                _To *_F2, _To *_L2, _To *& _Mid2) const
                {_Mid1 = _F1, _Mid2 = _F2;
                result _Ans = _Mid1 == _L1 ? ok : partial;
                int _N;
                while (_Mid1 != _L1 && _Mid2 != _L2)
                        if ((*__p___mb_cur_max()) <= _L2 - _Mid2)
                                if ((_N =
                                        _Wcrtomb(_Mid2, *_Mid1, &_State,
                                                &_Cvt)) <= 0)
                                        return (error);
                                else
                                        ++_Mid1, _Mid2 += _N, _Ans = ok;
                        else
                                {_To _Buf[2];
                                _St _Stsave = _State;
                                if ((_N =
                                        _Wcrtomb(_Buf, *_Mid1, &_State,
                                                &_Cvt)) <= 0)
                                        return (error);
                                else if (_L2 - _Mid2 < _N)
                                        {_State = _Stsave;
                                        return (_Ans); }
                                else
                                        {memcpy(_Mid2, _Buf, _N);
                                        ++_Mid1, _Mid2 += _N, _Ans = ok; }}
                return (_Ans); }
        virtual int do_length(_St& _State, const _E *_F1,
                const _E *_L1, size_t _N2) const throw ()
                {const _E *_Mid1;
                _To _Buf[2];
                int _N;
                if (0) ; else for (_Mid1 = _F1; _Mid1 != _L1 && 0 < _N2;
                        ++_Mid1, _N2 -= _N)
                        if ((_N =
                                _Wcrtomb(_Buf, *_Mid1, &_State, &_Cvt)) <= 0
                                        || _N2 < _N)
                                break;
                return (_Mid1 - _F1); }
        virtual bool do_always_noconv() const throw ()
                {return (false); }
        virtual int do_max_length() const throw ()
                {return (2); }
        virtual int do_encoding() const throw ()
                {return (0); }
private:
        _Locinfo::_Cvtvec _Cvt;
        };
                
template<class _E, class _To, class _St>
        class codecvt_byname : public codecvt<_E, _To, _St> {
public:
        explicit codecvt_byname(const char *_S, size_t _R = 0)
                : codecvt<_E, _To, _St>(_Locinfo(_S), _R) {}
public:
        virtual ~codecvt_byname()
                {}
        };
                
struct __declspec(dllimport) ctype_base : public locale::facet {
        enum _Mask {alnum = 0x4|0x2|0x1|0x100, alpha = 0x2|0x1|0x100,
                cntrl = 0x20, digit = 0x4, graph = 0x4|0x2|0x10|0x1|0x100,
                lower = 0x2, print = 0x4|0x2|0x10|0x40|0x1|0x100|0x80,
                punct = 0x10, space = 0x8|0x40|0x000, upper = 0x1,
                xdigit = 0x80};

        typedef short mask;     
        ctype_base(size_t _R = 0)
                : locale::facet(_R) {}
        };
                
template<class _E>
        class ctype : public ctype_base {
public:
        typedef _E char_type;
        bool is(mask _M, _E _C) const
                {return (do_is(_M, _C)); }
        const _E *is(const _E *_F, const _E *_L, mask *_V) const
                {return (do_is(_F, _L, _V)); }
        const _E *scan_is(mask _M, const _E *_F,
                const _E *_L) const
                {return (do_scan_is(_M, _F, _L)); }
        const _E *scan_not(mask _M, const _E *_F,
                const _E *_L) const
                {return (do_scan_not(_M, _F, _L)); }
        _E tolower(_E _C) const
                {return (do_tolower(_C)); }
        const _E *tolower(_E *_F, const _E *_L) const
                {return (do_tolower(_F, _L)); }
        _E toupper(_E _C) const
                {return (do_toupper(_C)); }
        const _E *toupper(_E *_F, const _E *_L) const
                {return (do_toupper(_F, _L)); }
        _E widen(char _X) const
                {return (do_widen(_X)); }
        const char *widen(const char *_F, const char *_L,
                _E *_V) const
                {return (do_widen(_F, _L, _V)); }
        char narrow(_E _C, char _D = '\0') const
                {return (do_narrow(_C, _D)); }
        const _E *narrow(const _E *_F, const _E *_L, char _D,
                char *_V) const
                {return (do_narrow(_F, _L, _D, _V)); }
        static locale::id id;
        explicit ctype(size_t _R = 0)
                : ctype_base(_R) {_Init(_Locinfo()); }
        ctype(const _Locinfo& _Lobj, size_t _R = 0)
                : ctype_base(_R) {_Init(_Lobj); }
        static size_t __cdecl _Getcat()
                {return (2); }
public:
        virtual ~ctype()
                {if (_Ctype._Delfl)
                        free((void *)_Ctype._Table); }
protected:
        void _Init(const _Locinfo& _Lobj)
                {_Ctype = _Lobj._Getctype(); }
        virtual bool do_is(mask _M, _E _C) const
                {return ((_Ctype._Table[narrow(_C)] & _M) != 0); }
        virtual const _E *do_is(const _E *_F, const _E *_L,
                mask *_V) const
                {if (0) ; else for (; _F != _L; ++_F, ++_V)
                        *_V = _Ctype._Table[narrow(*_F)];
                return (_F); }
        virtual const _E *do_scan_is(mask _M, const _E *_F,
                const _E *_L) const
                {if (0) ; else for (; _F != _L && !is(_M, *_F); ++_F)
                        ;
                return (_F); }
        virtual const _E *do_scan_not(mask _M, const _E *_F,
                const _E *_L) const
                {if (0) ; else for (; _F != _L && is(_M, *_F); ++_F)
                        ;
                return (_F); }
        virtual _E do_tolower(_E _C) const
                {return (widen(_Tolower(narrow(_C), &_Ctype))); }
        virtual const _E *do_tolower(_E *_F, const _E *_L) const
                {if (0) ; else for (; _F != _L; ++_F)
                        *_F = _Tolower(*_F, &_Ctype);
                return ((const _E *)_F); }
        virtual _E do_toupper(_E _C) const
                {return (widen(_Toupper(narrow(_C), &_Ctype))); }
        virtual const _E *do_toupper(_E *_F, const _E *_L) const
                {if (0) ; else for (; _F != _L; ++_F)
                        *_F = _Toupper(*_F, &_Ctype);
                return ((const _E *)_F); }
        virtual _E do_widen(char _X) const
                {return (_Widen(_X, (_E *)0)); }
        virtual const char *do_widen(const char *_F, const char *_L,
                _E *_V) const
                {if (0) ; else for (; _F != _L; ++_F, ++_V)
                        *_V = _Widen(*_F, (_E *)0);
                return (_F); }
        virtual char do_narrow(_E _C, char) const
                {return (_Narrow((_E)(_C))); }
        virtual const _E *do_narrow(const _E *_F, const _E *_L,
                char, char *_V) const
                {if (0) ; else for (; _F != _L; ++_F, ++_V)
                        *_V = _Narrow((_E)(*_F));
                return (_F); }
private:
        _Locinfo::_Ctypevec _Ctype;
        };
template<class _E>
        locale::id ctype<_E>::id;
                
class __declspec(dllimport) ctype<char> : public ctype_base {
public:
        typedef char _E;
        typedef _E char_type;
        bool is(mask _M, _E _C) const
                {return ((_Ctype._Table[(unsigned char)_C] & _M) != 0); }
        const _E *is(const _E *_F, const _E *_L, mask *_V) const
                {if (0) ; else for (; _F != _L; ++_F, ++_V)
                        *_V = _Ctype._Table[(unsigned char)*_F];
                return (_F); }
        const _E *scan_is(mask _M, const _E *_F,
                const _E *_L) const
                {if (0) ; else for (; _F != _L && !is(_M, *_F); ++_F)
                        ;
                return (_F); }
        const _E *scan_not(mask _M, const _E *_F,
                const _E *_L) const
                {if (0) ; else for (; _F != _L && is(_M, *_F); ++_F)
                        ;
                return (_F); }
        _E tolower(_E _C) const
                {return (do_tolower(_C)); }
        const _E *tolower(_E *_F, const _E *_L) const
                {return (do_tolower(_F, _L)); }
        _E toupper(_E _C) const
                {return (do_toupper(_C)); }
        const _E *toupper(_E *_F, const _E *_L) const
                {return (do_toupper(_F, _L)); }
        _E widen(char _X) const
                {return (_X); }
        const _E *widen(const char *_F, const char *_L, _E *_V) const
                {memcpy(_V, _F, _L - _F);
                return (_L); }
        _E narrow(_E _C, char _D = '\0') const
                {return (_C); }
        const _E *narrow(const _E *_F, const _E *_L, char _D,
                char *_V) const
                {memcpy(_V, _F, _L - _F);
                return (_L); }
        static locale::id id;
        explicit ctype(const mask *_Tab = 0, bool _Df = false,
                size_t _R = 0)
                : ctype_base(_R)
                {_Lockit Lk;
                _Init(_Locinfo());
                if (_Ctype._Delfl)
                        free((void *)_Ctype._Table), _Ctype._Delfl = false;
                if (_Tab == 0)
                        _Ctype._Table = _Cltab;
                else
                        _Ctype._Table = _Tab, _Ctype._Delfl = _Df; }
        ctype(const _Locinfo& _Lobj, size_t _R = 0)
                : ctype_base(_R) {_Init(_Lobj); }
        static size_t __cdecl _Getcat()
                {return (2); }
        static const size_t table_size;
public:
        virtual ~ctype()
                {if (_Ctype._Delfl)
                        free((void *)_Ctype._Table); }
protected:
        static void __cdecl _Term(void)
                {free((void *)_Cltab); }
        void _Init(const _Locinfo& _Lobj)
                {_Lockit Lk;
                _Ctype = _Lobj._Getctype();
                if (_Cltab == 0)
                        {_Cltab = _Ctype._Table;
                        atexit(_Term);
                        _Ctype._Delfl = false; }}
        virtual _E do_tolower(_E _C) const
                {return (_Tolower((unsigned char)_C, &_Ctype)); }
        virtual const _E *do_tolower(_E *_F, const _E *_L) const
                {if (0) ; else for (; _F != _L; ++_F)
                        *_F = _Tolower(*_F, &_Ctype);
                return ((const _E *)_F); }
        virtual _E do_toupper(_E _C) const
                {return (_Toupper((unsigned char)_C, &_Ctype)); }
        virtual const _E *do_toupper(_E *_F, const _E *_L) const
                {if (0) ; else for (; _F != _L; ++_F)
                        *_F = _Toupper(*_F, &_Ctype);
                return ((const _E *)_F); }
        const mask *table() const throw ()
                {return (_Ctype._Table); }
        static const mask * __cdecl classic_table() throw ()
                {_Lockit Lk;
                if (_Cltab == 0)
                        locale::classic();      
                return (_Cltab); }
private:
        _Locinfo::_Ctypevec _Ctype;
        static const mask *_Cltab;
        };
                
template<class _E>
        class ctype_byname : public ctype<_E> {
public:
        explicit ctype_byname(const char *_S, size_t _R = 0)
                : ctype<_E>(_Locinfo(_S), _R) {}
public:
        virtual ~ctype_byname()
                {}
        };


#pragma warning(disable:4231) 

extern template class __declspec(dllimport) codecvt<char, char, int>;
extern template class __declspec(dllimport) ctype<wchar_t>;

#pragma warning(default:4231) 
#line 624 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"

};

#pragma pack(pop)
#line 629 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"

#line 631 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"
namespace std {
                
class __declspec(dllimport) ios_base {
public:
                        
        class failure : public runtime_error {
        public:
                explicit failure(const string &_S)
                        : runtime_error(_S) {}
                virtual ~failure()
                        {}
        protected:
                virtual void _Doraise() const
                        {throw (*this); }
        };
        enum _Fmtflags {skipws = 0x0001, unitbuf = 0x0002,
                uppercase = 0x0004, showbase = 0x0008,
                showpoint = 0x0010, showpos = 0x0020,
                left = 0x0040, right = 0x0080, internal = 0x0100,
                dec = 0x0200, oct = 0x0400, hex = 0x0800,
                scientific = 0x1000, fixed = 0x2000, boolalpha = 0x4000,
                adjustfield = 0x01c0, basefield = 0x0e00,
                floatfield = 0x3000, _Fmtmask = 0x7fff, _Fmtzero = 0};
        enum _Iostate {goodbit = 0x0, eofbit = 0x1,
                failbit = 0x2, badbit = 0x4, _Statmask = 0x7};
        enum _Openmode {in = 0x01, out = 0x02, ate = 0x04,
                app = 0x08, trunc = 0x10, binary = 0x20};
        enum seekdir {beg = 0, cur = 1, end = 2};
        enum event {erase_event, imbue_event, copyfmt_event};
        typedef void (__cdecl *event_callback)(event, ios_base&, int);
        typedef int fmtflags;
        typedef int iostate;
        typedef int openmode;
        typedef short io_state, open_mode, seek_dir;
                        
        class __declspec(dllimport) Init {
        public:
                Init();
                ~Init();
        private:
                static int _Init_cnt;
                };
        ios_base& operator=(const ios_base& _R)
                {if (this != &_R)
                        {_State = _R._State;
                        copyfmt(_R); }
                return (*this); }
        operator void *() const
                {return (fail() ? 0 : (void *)this); }
        bool operator!() const
                {return (fail()); }
        void clear(iostate = goodbit, bool = false);
        void clear(io_state _St)
                {clear((iostate)_St); }
        iostate rdstate() const
                {return (_State); }
        void setstate(iostate _St, bool _Ex = false)
                {if (_St != goodbit)
                        clear((iostate)((int)rdstate() | (int)_St), _Ex); }
        void setstate(io_state _St)
                {setstate((iostate)_St); }
        bool good() const
                {return (rdstate() == goodbit); }
        bool eof() const
                {return ((int)rdstate() & (int)eofbit); }
        bool fail() const
                {return (((int)rdstate() & ((int)badbit | (int)failbit)) != 0); }
        bool bad() const
                {return (((int)rdstate() & (int)badbit) != 0); }
        iostate exceptions() const
                {return (_Except); }
        void exceptions(iostate _Ne)
                {_Except = _Ne & _Statmask;
                clear(_State); }
        void exceptions(io_state _St)
                {exceptions((iostate)_St); }
        fmtflags flags() const
                {return (_Fmtfl); }
        fmtflags flags(fmtflags _Nf)
                {fmtflags _Of = _Fmtfl;
                _Fmtfl = _Nf & _Fmtmask;
                return (_Of); }
        fmtflags setf(fmtflags _Nf)
                {ios_base::fmtflags _Of = _Fmtfl;
                _Fmtfl |= _Nf & _Fmtmask;
                return (_Of); }
        fmtflags setf(fmtflags _Nf, fmtflags _M)
                {ios_base::fmtflags _Of = _Fmtfl;
                _Fmtfl = (_Fmtfl & ~_M) | (_Nf & _M & _Fmtmask);
                return (_Of); }
        void unsetf(fmtflags _M)
                {_Fmtfl &= ~_M; }
        streamsize precision() const
                {return (_Prec); }
        streamsize precision(int _Np)
                {streamsize _Op = _Prec;
                _Prec = _Np;
                return (_Op); }
        streamsize width() const
                {return (_Wide); }
        streamsize width(streamsize _Nw)
                {streamsize _Ow = _Wide;
                _Wide = _Nw;
                return (_Ow); }
        locale getloc() const
                {return (_Loc); }
        locale imbue(const locale&);
        static int __cdecl xalloc()
                {_Lockit _Lk;
                return (_Index++); }
        long& iword(int _Idx)
                {return (_Findarr(_Idx)._Lo); }
        void *& pword(int _Idx)
                {return (_Findarr(_Idx)._Vp); }
        void register_callback(event_callback, int);
        ios_base& copyfmt(const ios_base&);
        virtual ~ios_base();
        static bool __cdecl sync_with_stdio(bool _Sfl = true)
                {_Lockit _Lk;
                const bool _Osfl = _Sync;
                _Sync = _Sfl;
                return (_Osfl); }
protected:
        ios_base()
                : _Loc(_Noinit), _Stdstr(0) {}
        void _Addstd();
        void _Init();
private:
                        
        struct _Iosarray {
        public:
                _Iosarray(int _Idx, _Iosarray *_Link)
                        : _Next(_Link), _Index(_Idx), _Lo(0), _Vp(0) {}
                _Iosarray *_Next;
                int _Index;
                long _Lo;
                void *_Vp;
                };
                        
        struct _Fnarray {
                _Fnarray(int _Idx, event_callback _P, _Fnarray *_Link)
                        : _Next(_Link), _Index(_Idx), _Pfn(_P) {}
                _Fnarray *_Next;
                int _Index;
                event_callback _Pfn;
                };
        void _Callfns(event);
        _Iosarray& _Findarr(int);
        void _Tidy();
        iostate _State, _Except;
        fmtflags _Fmtfl;
        int _Prec, _Wide;
        _Iosarray *_Arr;
        _Fnarray *_Calls;
        locale _Loc;
        size_t _Stdstr;
        static int _Index;
        static bool _Sync;
        };



};

#pragma pack(pop)
#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"

#line 177 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"





#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"


#pragma pack(push,8)
#line 13 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
namespace std {
		
template<class _E>
	class numpunct : public locale::facet {
public:
	typedef basic_string<_E, char_traits<_E>, allocator<_E> >
		string_type;
	typedef _E char_type;
	static locale::id id;
	_E decimal_point() const
		{return (do_decimal_point()); }
	_E thousands_sep() const
		{return (do_thousands_sep()); }
	string grouping() const
		{return (do_grouping()); }
	string_type falsename() const
		{return (do_falsename()); }
	string_type truename() const
		{return (do_truename()); }
	explicit numpunct(size_t _R = 0)
		: locale::facet(_R) {_Init(_Locinfo()); }
	numpunct(const _Locinfo& _Lobj, size_t _R = 0)
		: locale::facet(_R) {_Init(_Lobj); }
	static size_t __cdecl _Getcat()
		{return (4); }
public:
	virtual ~numpunct()
		{delete[] _Gr;
		delete[] _Nf;
		delete[] _Nt; }
protected:
	void _Init(const _Locinfo& _Lobj)
		{const lconv *_P = _Lobj._Getlconv();
		_Dp = _Widen(_P->decimal_point[0], (_E *)0);
		_Ks = _Widen(_P->thousands_sep[0], (_E *)0);
		_Gr = _Maklocstr(_P->grouping, (char *)0);
		_Nf = _Maklocstr(_Lobj._Getfalse(), (_E *)0);
		_Nt = _Maklocstr(_Lobj._Gettrue(), (_E *)0); }
	virtual _E do_decimal_point() const
		{return (_Dp); }
	virtual _E do_thousands_sep() const
		{return (_Ks); }
	virtual string do_grouping() const
		{return (string(_Gr)); }
	virtual string_type do_falsename() const
		{return (string_type(_Nf)); }
	virtual string_type do_truename() const
		{return (string_type(_Nt)); }
private:
	char *_Gr;
	_E _Dp, _Ks, *_Nf, *_Nt;
	};
typedef numpunct<char> _Npc;
typedef numpunct<wchar_t> _Npwc;
		
template<class _E>
	class numpunct_byname : public numpunct<_E> {
public:
	explicit numpunct_byname(const char *_S, size_t _R = 0)
		: numpunct<_E>(_Locinfo(_S), _R) {}
public:
	virtual ~numpunct_byname()
		{}
	};
template<class _E>
	locale::id numpunct<_E>::id;
 
		
template<class _E,
	class _II = istreambuf_iterator<_E, char_traits<_E> > >
	class num_get : public locale::facet {
public:
	typedef numpunct<_E> _Mypunct;
	typedef basic_string<_E, char_traits<_E>, allocator<_E> >
		_Mystr;
	static size_t __cdecl _Getcat()
		{return (4); }
	static locale::id id;
public:
	virtual ~num_get()
		{}
protected:
	void _Init(const _Locinfo& _Lobj)
		{}
public:
	explicit num_get(size_t _R = 0)
		: locale::facet(_R) {_Init(_Locinfo()); }
	num_get(const _Locinfo& _Lobj, size_t _R = 0)
		: locale::facet(_R) {_Init(_Lobj); }
	typedef _E char_type;
	typedef _II iter_type;
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		_Bool& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		unsigned short& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		unsigned int& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		long& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		unsigned long& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		float& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		double& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		long double& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
	_II get(_II _F, _II _L, ios_base& _X, ios_base::iostate& _St,
		void *& _V) const
		{return (do_get(_F, _L, _X, _St, _V)); }
protected:
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, _Bool& _V) const
		{int _Ans = -1;
		if (_X.flags() & ios_base::boolalpha)
			{const _Mypunct& _Fac = use_facet(_X.getloc(), (_Mypunct *)0, true);
			_Mystr _Str(1, (_E)0);
			_Str += _Fac.falsename();
			_Str += (_E)0;
			_Str += _Fac.truename();
			_Ans = _Getloctxt(_F, _L, (size_t)2, _Str.c_str()); }
		else
			{char _Ac[32], *_Ep;
			(*_errno()) = 0;
			const unsigned long _Ulo = strtoul(_Ac, &_Ep,
				_Getifld(_Ac, _F, _L, _X.flags(), _X.getloc()));
			if (_Ep != _Ac && (*_errno()) == 0 && _Ulo <= 1)
				_Ans = _Ulo; }
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ans < 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans != 0;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, unsigned short& _V) const
		{char _Ac[32], *_Ep;
		(*_errno()) = 0;
		int _Base =
			_Getifld(_Ac, _F, _L, _X.flags(), _X.getloc());
		char *_S = _Ac[0] == '-' ? _Ac + 1 : _Ac;
		const unsigned long _Ans = strtoul(_S, &_Ep, _Base);
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _S || (*_errno()) != 0 || 0xffff < _Ans)
			_St |= ios_base::failbit;
		else
			_V = _Ac[0] == '-' ? -_Ans : _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, unsigned int& _V) const
		{char _Ac[32], *_Ep;
		(*_errno()) = 0;
		int _Base =
			_Getifld(_Ac, _F, _L, _X.flags(), _X.getloc());
		char *_S = _Ac[0] == '-' ? _Ac + 1 : _Ac;
		const unsigned long _Ans = strtoul(_S, &_Ep, _Base);
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _S || (*_errno()) != 0 || 0xffffffff < _Ans)
			_St |= ios_base::failbit;
		else
			_V = _Ac[0] == '-' ? -_Ans : _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, long& _V) const
		{char _Ac[32], *_Ep;
		(*_errno()) = 0;
		const long _Ans = strtol(_Ac, &_Ep,
			_Getifld(_Ac, _F, _L, _X.flags(), _X.getloc()));
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _Ac || (*_errno()) != 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, unsigned long& _V) const
		{char _Ac[32], *_Ep;
		(*_errno()) = 0;
		const unsigned long _Ans = strtoul(_Ac, &_Ep,
			_Getifld(_Ac, _F, _L, _X.flags(), _X.getloc()));
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _Ac || (*_errno()) != 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, float& _V) const
		{char _Ac[8 + 36 + 16], *_Ep;
		(*_errno()) = 0;
		const float _Ans = _Stof(_Ac, &_Ep,
			_Getffld(_Ac, _F, _L, _X.getloc()));
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _Ac || (*_errno()) != 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, double& _V) const
		{char _Ac[8 + 36 + 16], *_Ep;
		(*_errno()) = 0;
		const double _Ans = _Stod(_Ac, &_Ep,
			_Getffld(_Ac, _F, _L, _X.getloc()));
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _Ac || (*_errno()) != 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, long double& _V) const
		{char _Ac[8 + 36 + 16], *_Ep;
		(*_errno()) = 0;
		const long double _Ans = _Stold(_Ac, &_Ep,
			_Getffld(_Ac, _F, _L, _X.getloc()));
		if (_F == _L)
			_St |= ios_base::eofbit;
		if (_Ep == _Ac || (*_errno()) != 0)
			_St |= ios_base::failbit;
		else
			_V = _Ans;
		return (_F); }
	virtual _II do_get(_II _F, _II _L, ios_base& _X,
		ios_base::iostate& _St, void *& _V) const
		{union _Pvlo {
			void *_Pv;
			unsigned long _Lo[1 +
				(sizeof (void *) - 1) / sizeof (unsigned long)];
			} _U;
		const int _NL = sizeof (_U._Lo) / sizeof (unsigned long);
		if (0) ; else for (int _I = 0; ; ++_F)
			{char _Ac[32], *_Ep;
			(*_errno()) = 0;
			_U._Lo[_I] = strtoul(_Ac, &_Ep,
				_Getifld(_Ac, _F, _L,
					ios_base::hex, _X.getloc()));
			if (_F == _L)
				_St |= ios_base::eofbit;
			if (_Ep == _Ac || (*_errno()) != 0)
				{_St |= ios_base::failbit;
				break; }
			if (_NL <= ++_I)
				break;
			if (_F == _L || *_F != _Widen(':', (_E *)0))
				{_St |= ios_base::failbit;
				break; }}
		if (!(_St & ios_base::failbit))
			_V = _U._Pv;
		return (_F); }
private:
	static int __cdecl _Getifld(char *_Ac, _II& _F, _II& _L,
		ios_base::fmtflags _Bfl, const locale& _Loc)
		{const _E _E0 = _Widen('0', (_E *)0);
		const _Mypunct& _Fac = use_facet(_Loc, (_Mypunct *)0, true);
		const string _Gr = _Fac.grouping();
		const _E _Ks = _Fac.thousands_sep();
		char *_P = _Ac;
		if (_F == _L)
			;
		else if (*_F == _Widen('+', (_E *)0))
			*_P++ = '+', ++_F;
		else if (*_F == _Widen('-', (_E *)0))
			*_P++ = '-', ++_F;
		_Bfl &= ios_base::basefield;
		int _Base = _Bfl == ios_base::oct ? 8
			: _Bfl == ios_base::hex ? 16
			: _Bfl == ios_base::_Fmtzero ? 0 : 10;
		bool _Sd = false, _Snz = false;
		if (_F != _L && *_F == _E0)
			{_Sd = true, ++_F;
			if (_F != _L && (*_F == _Widen('x', (_E *)0)
					|| *_F == _Widen('X', (_E *)0))
				&& (_Base == 0 || _Base == 16))
				_Base = 16, _Sd = false, ++_F;
			else if (_Base == 0)
				_Base = 8; }
		int _Dlen = _Base == 0 || _Base == 10 ? 10
			: _Base == 8 ? 8 : 16 + 6;
		string _Grin(1, _Sd ? '\1' : '\0');
		size_t _I = 0;
		if (0) ; else for (char *const _Pe = &_Ac[32 - 1];
			_F != _L; ++_F)
			if (memchr("0123456789abcdefABCDEF",
				*_P = _Narrow((_E)(*_F)), _Dlen) != 0)
				{if ((_Snz || *_P != '0') && _P < _Pe)
					++_P, _Snz = true;
				_Sd = true;
				if (_Grin[_I] != 127)
					++_Grin[_I]; }
			else if (_Grin[_I] == '\0' || _Ks == (_E)0
				|| *_F != _Ks)
				break;
			else
				_Grin.append(1, '\0'), ++_I;
		if (_I == 0)
			;
		else if ('\0' < _Grin[_I])
			++_I;
		else
			_Sd = false;
		if (0) ; else for (const char *_Pg = _Gr.c_str(); _Sd && 0 < _I; --_I)
			if (*_Pg == 127)
				break;
			else if (0 < --_I && *_Pg != _Grin[_I]
				|| 0 == _I && *_Pg < _Grin[_I])
				_Sd = false;
			else if ('\0' < _Pg[1])
				++_Pg;
		if (_Sd && !_Snz)
			*_P++ = '0';
		else if (!_Sd)
			_P = _Ac;
		*_P = '\0';
		return (_Base);
		}
	static int __cdecl _Getffld(char *_Ac, _II& _F, _II &_L,
		const locale& _Loc)
		{const _E _E0 = _Widen('0', (_E *)0);
		const _Mypunct& _Fac = use_facet(_Loc, (_Mypunct *)0, true);
		char *_P = _Ac;
		if (_F == _L)
			;
		else if (*_F == _Widen('+', (_E *)0))
			*_P++ = '+', ++_F;
		else if (*_F == _Widen('-', (_E *)0))
			*_P++ = '-', ++_F;
		bool _Sd = false;
		if (0) ; else for (; _F != _L && *_F == _E0; _Sd = true, ++_F)
			;
		if (_Sd)
			*_P++ = '0';
		int _Ns = 0;
		int _Pten = 0;
		if (0) ; else for (; _F != _L && isdigit(*_P = _Narrow((_E)(*_F)));
			_Sd = true, ++_F)
			if (_Ns < 36)
				++_P, ++_Ns;
			else
				++_Pten;
		if (_F != _L && *_F == _Fac.decimal_point())
			*_P++ = localeconv()->decimal_point[0], ++_F;
		if (_Ns == 0)
			{if (0) ; else for (; _F != _L && *_F == _E0; _Sd = true, ++_F)
				--_Pten;
			if (_Pten < 0)
				*_P++ = '0', ++_Pten; }
		if (0) ; else for (; _F != _L && isdigit(*_P = _Narrow((_E)(*_F)));
			_Sd = true, ++_F)
			if (_Ns < 36)
				++_P, ++_Ns;
		if (_Sd && _F != 
_L
&& (*_F == _Widen('e', (_E *)0) || *_F == _Widen('E', (_E *)0)))
			{*_P++ = 'e', ++_F;
			_Sd = false, _Ns = 0;
			if (_F == _L)
				;
			else if (*_F == _Widen('+', (_E *)0))
				*_P++ = '+', ++_F;
			else if (*_F == _Widen('-', (_E *)0))
				*_P++ = '-', ++_F;
			if (0) ; else for (; _F != _L && *_F == _E0; _Sd = true, ++_F)
				;
			if (_Sd)
				*_P++ = '0';
			if (0) ; else for (; _F != _L && isdigit(*_P = _Narrow((_E)(*_F)));
				_Sd = true, ++_F)
				if (_Ns < 8)
					++_P, ++_Ns; }
		if (!_Sd)
			_P = _Ac;
		*_P = '\0';
		return (_Pten);
		};
	};
template<class _E, class _II>
	locale::id num_get<_E, _II>::id;
		
template<class _E,
	class _OI = ostreambuf_iterator<_E, char_traits<_E> > >
	class num_put : public locale::facet {
public:
	typedef numpunct<_E> _Mypunct;
	typedef basic_string<_E, char_traits<_E>, allocator<_E> >
		_Mystr;
	static size_t __cdecl _Getcat()
		{return (4); }
	static locale::id id;
public:
	virtual ~num_put()
		{}
protected:
	void _Init(const _Locinfo& _Lobj)
		{}
public:
	explicit num_put(size_t _R = 0)
		: locale::facet(_R) {_Init(_Locinfo()); }
	num_put(const _Locinfo& _Lobj, size_t _R = 0)
		: locale::facet(_R) {_Init(_Lobj); }
	typedef _E char_type;
	typedef _OI iter_type;
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		_Bool _V) const
		{return (do_put(_F, _X, _Fill, _V)); }
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		long _V) const
		{return (do_put(_F, _X, _Fill, _V)); }
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		unsigned long _V) const
		{return (do_put(_F, _X, _Fill, _V)); }
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		double _V) const
		{return (do_put(_F, _X, _Fill, _V)); }
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		long double _V) const
		{return (do_put(_F, _X, _Fill, _V)); }
	_OI put(_OI _F, ios_base& _X, _E _Fill,
		const void *_V) const
		{return (do_put(_F, _X, _Fill, _V)); }
protected:
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		_Bool _V) const
		{const _Mypunct& _Fac = use_facet(_X.getloc(), (_Mypunct *)0, true);
		_Mystr _Str;
		if (!(_X.flags() & ios_base::boolalpha))
			_Str.append(1, _Widen(_V ? '1' : '0', (_E *)0));
		else if (_V)
			_Str = _Fac.truename();
		else
			_Str = _Fac.falsename();
		size_t _M = _X.width() <= 0 || _X.width() <= _Str.size()
			? 0 : _X.width() - _Str.size();
		ios_base::fmtflags _Afl =
			_X.flags() & ios_base::adjustfield;
		if (_Afl != ios_base::left)
			_F = _Rep(_F, _Fill, _M), _M = 0;
		_F = _Put(_F, _Str.c_str(), _Str.size());
		_X.width(0);
		return (_Rep(_F, _Fill, _M)); }
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		long _V) const
		{char _Buf[2 * 32], _Fmt[6];
		return (_Iput(_F, _X, _Fill, _Buf,
			sprintf(_Buf, _Ifmt(_Fmt, 'd', _X.flags()), _V))); }
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		unsigned long _V) const
		{char _Buf[2 * 32], _Fmt[6];
		return (_Iput(_F, _X, _Fill, _Buf,
			sprintf(_Buf, _Ifmt(_Fmt, 'u', _X.flags()), _V))); }
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		double _V) const
		{char _Buf[8 + 36 + 16], _Fmt[8];
		streamsize _Prec = _X.precision() <= 0
			&& !(_X.flags() & ios_base::fixed) ? 6
			: _X.precision();
		int _Mpr = 36 < _Prec ? 36 : _Prec;
		return (_Fput(_F, _X, _Fill, _Buf, _Prec - _Mpr,
			sprintf(_Buf, _Ffmt(_Fmt, 0, _X.flags()),
				_Mpr, _V))); }
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		long double _V) const
		{char _Buf[8 + 36 + 16], _Fmt[8];
		streamsize _Prec = _X.precision() <= 0
			&& !(_X.flags() & ios_base::fixed) ? 6
			: _X.precision();
		int _Mpr = 36 < _Prec ? 36 : _Prec;
		return (_Fput(_F, _X, _Fill, _Buf, _Prec - _Mpr,
			sprintf(_Buf, _Ffmt(_Fmt, 'L', _X.flags()),
				_Mpr, _V))); }
	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
		const void *_V) const
		{
		const int _NL = 1
			+ (sizeof (void *) - 1) / sizeof (unsigned long);
		char _Buf[(_NL + 1) * (32 + 1)];
		int _N = sprintf(_Buf, "%p", _V);
		size_t _M = _X.width() <= 0 || _X.width() <= _N
			? 0 : _X.width() - _N;
		ios_base::fmtflags _Afl =
			_X.flags() & ios_base::adjustfield;
		if (_Afl != ios_base::left)
			_F = _Rep(_F, _Fill, _M), _M = 0;
		_F = _Putc(_F, _Buf, _N);
		_X.width(0);
		return (_Rep(_F, _Fill, _M)); }
	static char *_Ffmt(char *_Fmt, char _Spec,
		ios_base::fmtflags _Fl)
		{char *_S = _Fmt;
		*_S++ = '%';
		if (_Fl & ios_base::showpos)
			*_S++ = '+';
		if (_Fl & ios_base::showpoint)
			*_S++ = '#';
		*_S++ = '.';
		*_S++ = '*';
		if (_Spec != 0)
			*_S++ = _Spec;	
		ios_base::fmtflags _Ffl = _Fl & ios_base::floatfield;
		*_S++ = _Ffl == ios_base::fixed ? 'f'
			: _Ffl == ios_base::scientific ? 'e' : 'g';
		*_S = '\0';
		return (_Fmt); }
	static _OI __cdecl _Fput(_OI _F, ios_base& _X, _E _Fill,
		const char *_S, size_t _Nz, size_t _N)
		{size_t _M = _X.width() <= 0 || _X.width() <= _N  + _Nz
			? 0 : _X.width() - _N - _Nz;
		ios_base::fmtflags _Afl =
			_X.flags() & ios_base::adjustfield;
		if (_Afl != ios_base::left && _Afl != ios_base::internal)
			_F = _Rep(_F, _Fill, _M), _M = 0;
		else if (_Afl == ios_base::internal)
			{if (0 < _N && (*_S == '+' || *_S == '-'))
				_F = _Putc(_F, _S, 1), ++_S, --_N;
			_F = _Rep(_F, _Fill, _M), _M = 0; }
		const char *_P = (const char *)memchr(_S,
			localeconv()->decimal_point[0], _N);
		if (_P != 0)
			{const _Mypunct& _Fac = use_facet(_X.getloc(), (_Mypunct *)0, true);
			size_t _Nf = _P - _S + 1;
			_F = _Putc(_F, _S, _Nf - 1);
			_F = _Rep(_F, _Fac.decimal_point(), 1);
			_S += _Nf, _N -= _Nf; }
		if ((_P = (const char *)memchr(_S, 'e', _N)) != 0)
			{size_t _Nm = _P - _S + 1;
			_F = _Putc(_F, _S, _Nm - 1);
			_F = _Rep(_F, _Widen('0', (_E *)0), _Nz), _Nz = 0;
			_F = _Putc(_F, _X.flags() & ios_base::uppercase
				? "E" : "e", 1);
			_S += _Nm, _N -= _Nm; }
		_F = _Putc(_F, _S, _N);
		_F = _Rep(_F, _Widen('0', (_E *)0), _Nz);
		_X.width(0);
		return (_Rep(_F, _Fill, _M)); }
	static char *__cdecl _Ifmt(char *_Fmt, char _Spec,
		ios_base::fmtflags _Fl)
		{char *_S = _Fmt;
		*_S++ = '%';
		if (_Fl & ios_base::showpos)
			*_S++ = '+';
		if (_Fl & ios_base::showbase)
			*_S++ = '#';
		*_S++ = 'l';
		ios_base::fmtflags _Bfl = _Fl & ios_base::basefield;
		*_S++ = _Bfl == ios_base::oct ? 'o'
			: _Bfl != ios_base::hex ? _Spec	
			: _Fl & ios_base::uppercase ? 'X' : 'x';
		*_S = '\0';
		return (_Fmt); }
	static _OI __cdecl _Iput(_OI _F, ios_base& _X, _E _Fill,
		char *_S, size_t _N)
		{const size_t _Np = *_S == '+' || *_S == '-' ? 1
			: *_S == '0' && (_S[1] == 'x' || _S[1] == 'X') ? 2
			: 0;
		const _Mypunct& _Fac = use_facet(_X.getloc(), (_Mypunct *)0, true);
		const string _Gr = _Fac.grouping();
		const _E _Ks = _Fac.thousands_sep();
		bool _Grp = '\0' < *_Gr.c_str();
		if (_Grp)
			{const char *_Pg = _Gr.c_str();
			size_t _I = _N;
			if (0) ; else for (_Grp = false; *_Pg != 127 && '\0' < *_Pg
				&& *_Pg < _I - _Np; _Grp = true)
				{_I -= *_Pg;
				memmove(&_S[_I + 1], &_S[_I], _N + 1 - _I);
				_S[_I] = ',', ++_N;
				if ('\0' < _Pg[1])
					++_Pg; }}
		size_t _M = _X.width() <= 0 || _X.width() <= _N
			? 0 : _X.width() - _N;
		ios_base::fmtflags _Afl =
			_X.flags() & ios_base::adjustfield;
		if (_Afl != ios_base::left && _Afl != ios_base::internal)
			_F = _Rep(_F, _Fill, _M), _M = 0;
		else if (_Afl == ios_base::internal)
			{_F = _Putc(_F, _S, _Np), _S += _Np, _N -= _Np;
			_F = _Rep(_F, _Fill, _M), _M = 0; }
		if (!_Grp)
			_F = _Putc(_F, _S, _N);
		else
			if (0) ; else for (; ; ++_S, --_N)
				{size_t _Nd = strcspn(_S, ",");
				_F = _Putc(_F, _S, _Nd);
				_S += _Nd, _N -= _Nd;
				if (_N == 0)
					break;
				if (_Ks != (_E)0)
					_F = _Rep(_F, _Ks, 1); }
		_X.width(0);
		return (_Rep(_F, _Fill, _M)); }
	static _OI _Put(_OI _F, const _E *_S, size_t _N)
		{if (0) ; else for (; 0 < _N; --_N, ++_F, ++_S)
			*_F = *_S;
		return (_F); }
	static _OI _Putc(_OI _F, const char *_S, size_t _N)
		{if (0) ; else for (; 0 < _N; --_N, ++_F, ++_S)
			*_F = _Widen(*_S, (_E *)0);
		return (_F); }
	static _OI _Rep(_OI _F, _E _C, size_t _N)
		{if (0) ; else for (; 0 < _N; --_N, ++_F)
			*_F = _C;
		return (_F); }
	};
template<class _E, class _OI>
	locale::id num_put<_E, _OI>::id;


#pragma warning(disable:4231) 

extern template class __declspec(dllimport) numpunct<char>;
extern template class __declspec(dllimport) numpunct<wchar_t>;

#pragma warning(default:4231) 
#line 641 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"

};

#pragma pack(pop)
#line 646 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"

#line 648 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"
namespace std {
		
template<class _E, class _Tr = char_traits<_E> >
	class basic_streambuf {
	basic_streambuf(const basic_streambuf<_E, _Tr>&);	
	basic_streambuf<_E, _Tr>&
		operator=(const basic_streambuf<_E, _Tr>&);
protected:
	basic_streambuf()
		: _Loc() {_Init(); }
	basic_streambuf(_Uninitialized)
		: _Loc(_Noinit) {}
public:
	typedef basic_streambuf<_E, _Tr> _Myt;
	typedef _E char_type;
	typedef _Tr traits_type;
	virtual ~basic_streambuf()
		{}
	typedef _Tr::int_type int_type;
	typedef _Tr::pos_type pos_type;
	typedef _Tr::off_type off_type;
	pos_type pubseekoff(off_type _O, ios_base::seekdir _W,
		ios_base::openmode _M = ios_base::in | ios_base::out)
		{return (seekoff(_O, _W, _M)); }
	pos_type pubseekoff(off_type _O, ios_base::seek_dir _W,
		ios_base::open_mode _M)
		{return (pubseekoff(_O, (ios_base::seekdir)_W,
			(ios_base::openmode)_M)); }
	pos_type pubseekpos(pos_type _P,
		ios_base::openmode _M = ios_base::in | ios_base::out)
		{return (seekpos(_P, _M)); }
	pos_type pubseekpos(pos_type _P, ios_base::open_mode _M)
		{return (seekpos(_P, (ios_base::openmode)_M)); }
	_Myt *pubsetbuf(_E *_S, streamsize _N)
		{return (setbuf(_S, _N)); }
	locale pubimbue(const locale &_Ln)
		{locale _Lo = _Loc;
		imbue(_Ln);
		_Loc = _Ln;
		return (_Lo); }
	locale getloc()
		{return (_Loc); }
	streamsize in_avail()
		{return (gptr() != 0 && gptr() < egptr()
			? egptr() - gptr() : showmanyc()); }
	int pubsync()
		{return (sync()); }
	int_type sbumpc()
		{return (gptr() != 0 && gptr() < egptr()
			? _Tr::to_int_type(*_Gninc()) : uflow()); }
	int_type sgetc()
		{return (gptr() != 0 && gptr() < egptr()
			? _Tr::to_int_type(*gptr()) : underflow()); }
	streamsize sgetn(_E *_S, streamsize _N)
		{return (xsgetn(_S, _N)); }
	int_type snextc()
		{return (_Tr::eq_int_type(_Tr::eof(), sbumpc())
			? _Tr::eof() : sgetc()); }
	int_type sputbackc(_E _C)
		{return (gptr() != 0 && eback() < gptr()
			&& _Tr::eq(_C, gptr()[-1])
			? _Tr::to_int_type(*_Gndec())
			: pbackfail(_Tr::to_int_type(_C))); }
	void stossc()
		{if (gptr() != 0 && gptr() < egptr())
			_Gninc();
		else
			uflow(); }
	int_type sungetc()
		{return (gptr() != 0 && eback() < gptr()
			? _Tr::to_int_type(*_Gndec()) : pbackfail()); }
	int_type sputc(_E _C)
		{return (pptr() != 0 && pptr() < epptr()
			? _Tr::to_int_type(*_Pninc() = _C)
			: overflow(_Tr::to_int_type(_C))); }
	streamsize sputn(const _E *_S, streamsize _N)
		{return (xsputn(_S, _N)); }
protected:
	_E *eback() const
		{return (*_IGbeg); }
	_E *gptr() const
		{return (*_IGnext); }
	_E *pbase() const
		{return (*_IPbeg); }
	_E *pptr() const
		{return (*_IPnext); }
	_E *egptr() const
		{return (*_IGnext + *_IGcnt); }
	void gbump(int _N)
		{*_IGcnt -= _N;
		*_IGnext += _N; }
	void setg(_E *_B, _E *_N, _E *_L)
		{*_IGbeg = _B, *_IGnext = _N, *_IGcnt = _L - _N; }
	_E *epptr() const
		{return (*_IPnext + *_IPcnt); }
	_E *_Gndec()
		{++*_IGcnt;
		return (--*_IGnext); }
	_E *_Gninc()
		{--*_IGcnt;
		return ((*_IGnext)++); }
	void pbump(int _N)
		{*_IPcnt -= _N;
		*_IPnext += _N; }
	void setp(_E *_B, _E *_L)
		{*_IPbeg = _B, *_IPnext = _B, *_IPcnt = _L - _B; }
	void setp(_E *_B, _E *_N, _E *_L)
		{*_IPbeg = _B, *_IPnext = _N, *_IPcnt = _L - _N; }
	_E *_Pninc()
		{--*_IPcnt;
		return ((*_IPnext)++); }
	void _Init()
		{_IGbeg = &_Gbeg, _IPbeg = &_Pbeg;
		_IGnext = &_Gnext, _IPnext = &_Pnext;
		_IGcnt = &_Gcnt, _IPcnt = &_Pcnt;
		setp(0, 0), setg(0, 0, 0); }
	void _Init(_E **_Gb, _E **_Gn, int *_Gc,
		_E **_Pb, _E **_Pn, int *_Pc)
		{_IGbeg = _Gb, _IPbeg = _Pb;
		_IGnext = _Gn, _IPnext = _Pn;
		_IGcnt = _Gc, _IPcnt = _Pc; }
	virtual int_type overflow(int_type = _Tr::eof())
		{return (_Tr::eof()); }
	virtual int_type pbackfail(int_type = _Tr::eof())
		{return (_Tr::eof()); }
	virtual int showmanyc()
		{return (0); }
	virtual int_type underflow()
		{return (_Tr::eof()); }
	virtual int_type uflow()
		{return (_Tr::eq_int_type(_Tr::eof(), underflow())
			? _Tr::eof() : _Tr::to_int_type(*_Gninc())); }
	virtual streamsize xsgetn(_E * _S, streamsize _N)
		{int_type _C;
		streamsize _M, _Ns;
		if (0) ; else for (_Ns = 0; 0 < _N; )
			if (gptr() != 0 && 0 < (_M = egptr() - gptr()))
				{if (_N < _M)
					_M = _N;
				_Tr::copy(_S, gptr(), _M);
				_S += _M, _Ns += _M, _N -= _M, gbump(_M); }
			else if (_Tr::eq_int_type(_Tr::eof(), _C = uflow()))
				break;
			else
				*_S++ = _Tr::to_char_type(_C), ++_Ns, --_N;
		return (_Ns); }
	virtual streamsize xsputn(const _E *_S, streamsize _N)
		{streamsize _M, _Ns;
		if (0) ; else for (_Ns = 0; 0 < _N; )
			if (pptr() != 0 && 0 < (_M = epptr() - pptr()))
				{if (_N < _M)
					_M = _N;
				_Tr::copy(pptr(), _S, _M);
				_S += _M, _Ns += _M, _N -= _M, pbump(_M); }
			else if (_Tr::eq_int_type(_Tr::eof(),
				overflow(_Tr::to_int_type(*_S))))
				break;
			else
				++_S, ++_Ns, --_N;
		return (_Ns); }
	virtual pos_type seekoff(off_type, ios_base::seekdir,
		ios_base::openmode = ios_base::in | ios_base::out)
		{return (streampos(_BADOFF)); }
	virtual pos_type seekpos(pos_type,
		ios_base::openmode = ios_base::in | ios_base::out)
		{return (streampos(_BADOFF)); }
	virtual _Myt *setbuf(_E *, streamsize)
		{return (this); }
	virtual int sync()
		{return (0); }
	virtual void imbue(const locale& _Loc)
		{}
private:
	_E *_Gbeg, *_Pbeg;
	_E **_IGbeg, **_IPbeg;
	_E *_Gnext, *_Pnext;
	_E **_IGnext, **_IPnext;
	int _Gcnt, _Pcnt;
	int *_IGcnt, *_IPcnt;
	locale _Loc;
	};
};

#pragma pack(pop)
#line 194 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"

#line 196 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"
namespace std {
		
template<class _E, class _Tr = char_traits<_E> >
	class basic_ios : public ios_base {
public:
	typedef basic_ios<_E, _Tr> _Myt;
	typedef basic_ostream<_E, _Tr> _Myos;
	typedef basic_streambuf<_E, _Tr> _Mysb;
	typedef ctype<_E> _Ctype;
	explicit basic_ios(_Mysb *_S)
		{init(_S); }
	basic_ios(const _Myt& _R)
		{init(0), *this = _R; }
	virtual ~basic_ios()
		{}
	typedef _E char_type;
	typedef _Tr traits_type;
	typedef _Tr::int_type int_type;
	typedef _Tr::pos_type pos_type;
	typedef _Tr::off_type off_type;
	void clear(iostate _St = goodbit, bool _Ex = false)
		{ios_base::clear(_Sb == 0 ? (int)_St | (int)badbit
			: (int)_St, _Ex); }
	void clear(io_state _St)
		{clear((iostate)_St); }
	void setstate(iostate _St, bool _Ex = false)
		{if (_St != goodbit)
			clear((iostate)((int)rdstate() | (int)_St), _Ex); }
	void setstate(io_state _St)
		{setstate((iostate)_St); }
	_Myt& copyfmt(const _Myt& _R)
		{_Tiestr = _R.tie();
		_Fillch = _R.fill();
		ios_base::copyfmt(_R);
		return (*this); }
	_Myos *tie() const
		{return (_Tiestr); }
	_Myos *tie(_Myos *_N)
		{_Myos *_O = _Tiestr;
		_Tiestr = _N;
		return (_O); }
	_Mysb *rdbuf() const
		{return (_Sb); }
	_Mysb *rdbuf(_Mysb *_N)
		{_Mysb *_O = _Sb;
		_Sb = _N;
		clear();
		return (_O); }
	locale imbue(const locale& _Ln)
		{if (rdbuf() != 0)
			rdbuf()->pubimbue(_Ln);
		return (ios_base::imbue(_Ln)); }
	_E fill() const
		{return (_Fillch); }
	_E fill(_E _Nf)
		{_E _Of = _Fillch;
		_Fillch = _Nf;
		return (_Of); }
	char narrow(_E _C, char _D = '\0') const
		{const _Ctype& _Fac = use_facet(getloc(), (_Ctype *)0, true);
		return (_Fac.narrow(_C, _D)); }
	_E widen(char _C) const
		{const _Ctype& _Fac = use_facet(getloc(), (_Ctype *)0, true);
		return (_Fac.widen(_C)); }
protected:
	void init(_Mysb *_S = 0,
		bool _Isstd = false)
		{_Sb = _S;
		_Tiestr = 0;
		_Fillch = _Widen(' ', (_E *)0);
		_Init();
		if (_Sb == 0)
			setstate(badbit);
		if (_Isstd)
			_Addstd(); }
	basic_ios()
		{}
private:
	_Mysb *_Sb;
	_Myos *_Tiestr;
	_E _Fillch;
	};
		
inline ios_base& __cdecl boolalpha(ios_base& _I)
	{_I.setf(ios_base::boolalpha);
	return (_I); }
inline ios_base& __cdecl dec(ios_base& _I)
	{_I.setf(ios_base::dec, ios_base::basefield);
	return (_I); }
inline ios_base& __cdecl fixed(ios_base& _I)
	{_I.setf(ios_base::fixed, ios_base::floatfield);
	return (_I); }
inline ios_base& __cdecl hex(ios_base& _I)
	{_I.setf(ios_base::hex, ios_base::basefield);
	return (_I); }
inline ios_base& __cdecl internal(ios_base& _I)
	{_I.setf(ios_base::internal, ios_base::adjustfield);
	return (_I); }
inline ios_base& __cdecl left(ios_base& _I)
	{_I.setf(ios_base::left, ios_base::adjustfield);
	return (_I); }
inline ios_base& __cdecl noboolalpha(ios_base& _I)
	{_I.unsetf(ios_base::boolalpha);
	return (_I); }
inline ios_base& __cdecl noshowbase(ios_base& _I)
	{_I.unsetf(ios_base::showbase);
	return (_I); }
inline ios_base& __cdecl noshowpoint(ios_base& _I)
	{_I.unsetf(ios_base::showpoint);
	return (_I); }
inline ios_base& __cdecl noshowpos(ios_base& _I)
	{_I.unsetf(ios_base::showpos);
	return (_I); }
inline ios_base& __cdecl noskipws(ios_base& _I)
	{_I.unsetf(ios_base::skipws);
	return (_I); }
inline ios_base& __cdecl nounitbuf(ios_base& _I)
	{_I.unsetf(ios_base::unitbuf);
	return (_I); }
inline ios_base& __cdecl nouppercase(ios_base& _I)
	{_I.unsetf(ios_base::uppercase);
	return (_I); }
inline ios_base& __cdecl oct(ios_base& _I)
	{_I.setf(ios_base::oct, ios_base::basefield);
	return (_I); }
inline ios_base& __cdecl right(ios_base& _I)
	{_I.setf(ios_base::right, ios_base::adjustfield);
	return (_I); }
inline ios_base& __cdecl scientific(ios_base& _I)
	{_I.setf(ios_base::scientific, ios_base::floatfield);
	return (_I); }
inline ios_base& __cdecl showbase(ios_base& _I)
	{_I.setf(ios_base::showbase);
	return (_I); }
inline ios_base& __cdecl showpoint(ios_base& _I)
	{_I.setf(ios_base::showpoint);
	return (_I); }
inline ios_base& __cdecl showpos(ios_base& _I)
	{_I.setf(ios_base::showpos);
	return (_I); }
inline ios_base& __cdecl skipws(ios_base& _I)
	{_I.setf(ios_base::skipws);
	return (_I); }
inline ios_base& __cdecl unitbuf(ios_base& _I)
	{_I.setf(ios_base::unitbuf);
	return (_I); }
inline ios_base& __cdecl uppercase(ios_base& _I)
	{_I.setf(ios_base::uppercase);
	return (_I); }
};

#pragma pack(pop)
#line 162 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"

#line 164 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"
		
 
 

 

namespace std {
		
template<class _E, class _Tr = char_traits<_E> >
	class basic_ostream : virtual public basic_ios<_E, _Tr> {
public:
	typedef basic_ostream<_E, _Tr> _Myt;
	typedef basic_ios<_E, _Tr> _Myios;
	typedef basic_streambuf<_E, _Tr> _Mysb;
	typedef ostreambuf_iterator<_E, _Tr> _Iter;
	typedef num_put<_E, _Iter> _Nput;
	explicit basic_ostream(basic_streambuf<_E, _Tr> *_S,
		bool _Isstd = false, bool _Doinit = true)
		{if (_Doinit)
		    init(_S, _Isstd); }
	basic_ostream(_Uninitialized)
		{_Addstd(); }
	virtual ~basic_ostream()
		{}
	class sentry {
	public:
		explicit sentry(_Myt& _Os)
			: _Ok(_Os.opfx()), _Ostr(_Os) {}
		~sentry()
			{if (!uncaught_exception())
				_Ostr.osfx(); }
		operator bool() const
			{return (_Ok); }
	private:
		bool _Ok;
		_Myt& _Ostr;
		};
	bool opfx()
		{if (good() && tie() != 0)
			tie()->flush();
		return (good()); }
	void osfx()
		{if (flags() & unitbuf)
			flush(); }
	_Myt& operator<<(_Myt& (__cdecl *_F)(_Myt&))
		{return ((*_F)(*this)); }
	_Myt& operator<<(_Myios& (__cdecl *_F)(_Myios&))
		{(*_F)(*(_Myios *)this);
		return (*this); }
	_Myt& operator<<(ios_base& (__cdecl *_F)(ios_base&))
		{(*_F)(*(ios_base *)this);
		return (*this); }
	_Myt& operator<<(_Bool _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(short _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			fmtflags _Bfl = flags() & basefield;
			long _Y = (_Bfl == oct || _Bfl == hex)
				? (long)(unsigned short)_X : (long)_X;
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _Y).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(unsigned short _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), (unsigned long)_X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(int _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			fmtflags _Bfl = flags() & basefield;
			long _Y = (_Bfl == oct || _Bfl == hex)
				? (long)(unsigned int)_X : (long)_X;
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _Y).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(unsigned int _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			unsigned long _Y = _X & 0xffffffff;
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), (unsigned long)_X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(long _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(unsigned long _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(float _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), (double)_X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(double _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(long double _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(const void *_X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nput& _Fac = use_facet(getloc(), (_Nput *)0, true);
			try {
			if (_Fac.put(_Iter(rdbuf()), *this,
				fill(), _X).failed())
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator<<(_Mysb *_Pb)
		{iostate _St = goodbit;
		bool _Copied = false;
		const sentry _Ok(*this);
		if (_Ok && _Pb != 0)
			if (0) ; else for (int_type _C = _Tr::eof(); ; _Copied = true)
				{try {
				_C = _Tr::eq_int_type(_Tr::eof(), _C)
					? _Pb->sgetc() : _Pb->snextc();
				} catch (...) {
					setstate(failbit);
					throw;
				}
				if (_Tr::eq_int_type(_Tr::eof(),_C))
					break;
				try {
					if (_Tr::eq_int_type(_Tr::eof(),
						rdbuf()->sputc(_Tr::to_char_type(_C))))
						{_St |= badbit;
						break; }
				} catch (...) { setstate(ios_base::badbit, true); } }
		width(0);
		setstate(!_Copied ? _St | failbit : _St);
		return (*this); }
	_Myt& put(_E _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (!_Ok)
			_St |= badbit;
		else
			{try {
			 if (_Tr::eq_int_type(_Tr::eof(),
				rdbuf()->sputc(_X)))
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& write(const _E *_S, streamsize _N)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (!_Ok)
			_St |= badbit;
		else
			{try {
			if (rdbuf()->sputn(_S, _N) != _N)
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& flush()
		{iostate _St = goodbit;
		if (!fail() && rdbuf()->pubsync() == -1)
			_St |= badbit;
		setstate(_St);
		return (*this); }
	_Myt& seekp(pos_type _P)
		{if (!fail())
			rdbuf()->pubseekpos(_P, out);
		return (*this); }
	_Myt& seekp(off_type _O, ios_base::seekdir _W)
		{if (!fail())
			rdbuf()->pubseekoff(_O, _W, out);
		return (*this); }
	pos_type tellp()
		{if (!fail())
			return (rdbuf()->pubseekoff(0, cur, out));
		else
			return (streampos(_BADOFF)); }
	};
		
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const _E *_X)
	{typedef basic_ostream<_E, _Tr> _Myos;
	ios_base::iostate _St = ios_base::goodbit;
	size_t _N = _Tr::length(_X);
	size_t _M = _O.width() <= 0 || _O.width() <= _N
		? 0 : _O.width() - _N;
	const _Myos::sentry _Ok(_O);
	if (!_Ok)
		_St |= ios_base::badbit;
	else
		{try {
		if ((_O.flags() & ios_base::adjustfield)
			!= ios_base::left)
			if (0) ; else for (; 0 < _M; --_M)
				if (_Tr::eq_int_type(_Tr::eof(),
					_O.rdbuf()->sputc(_O.fill())))
					{_St |= ios_base::badbit;
					break; }
		if (_St == ios_base::goodbit
			&& _O.rdbuf()->sputn(_X, _N) != _N)
			_St |= ios_base::badbit;
		if (_St == ios_base::goodbit)
			if (0) ; else for (; 0 < _M; --_M)
				if (_Tr::eq_int_type(_Tr::eof(),
					_O.rdbuf()->sputc(_O.fill())))
					{_St |= ios_base::badbit;
					break; }
		_O.width(0);
		} catch (...) { (_O).setstate(ios_base::badbit, true); } }
	_O.setstate(_St);
	return (_O); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, _E _C)
	{typedef basic_ostream<_E, _Tr> _Myos;
	ios_base::iostate _St = ios_base::goodbit;
	const _Myos::sentry _Ok(_O);
	if (_Ok)
		{size_t _M = _O.width() <= 1 ? 0 : _O.width() - 1;
		try {
		if ((_O.flags() & ios_base::adjustfield)
			!= ios_base::left)
			if (0) ; else for (; _St == ios_base::goodbit && 0 < _M; --_M)
				if (_Tr::eq_int_type(_Tr::eof(),
					_O.rdbuf()->sputc(_O.fill())))
					_St |= ios_base::badbit;
		if (_St == ios_base::goodbit
			&& _Tr::eq_int_type(_Tr::eof(),
				_O.rdbuf()->sputc(_C)))
			_St |= ios_base::badbit;
		if (0) ; else for (; _St == ios_base::goodbit && 0 < _M; --_M)
			if (_Tr::eq_int_type(_Tr::eof(),
				_O.rdbuf()->sputc(_O.fill())))
				_St |= ios_base::badbit;
		} catch (...) { (_O).setstate(ios_base::badbit, true); } }
	_O.width(0);
	_O.setstate(_St);
	return (_O); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const signed char *_X)
	{return (_O << (const char *)_X); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const signed char _C)
	{return (_O << (char)_C); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const unsigned char *_X)
	{return (_O << (const char *)_X); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const unsigned char _C)
	{return (_O << (char)_C); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>& __cdecl operator<<(
		basic_ostream<_E, _Tr>& _O, const signed short *_X)
	{return (_O << (const wchar_t *)_X); }
		
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>&
		__cdecl endl(basic_ostream<_E, _Tr>& _O)
	{_O.put(_O.widen('\n'));
	_O.flush();
	return (_O); }
__declspec(dllimport) inline basic_ostream<char, char_traits<char> >&
	__cdecl endl(basic_ostream<char, char_traits<char> >& _O)
	{_O.put('\n');
	_O.flush();
	return (_O); }
__declspec(dllimport) inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl endl(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _O)
	{_O.put('\n');
	_O.flush();
	return (_O); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>&
		__cdecl ends(basic_ostream<_E, _Tr>& _O)
	{_O.put(_E('\0'));
	return (_O); }
__declspec(dllimport) inline basic_ostream<char, char_traits<char> >&
	__cdecl ends(basic_ostream<char, char_traits<char> >& _O)
	{_O.put('\0');
	return (_O); }
__declspec(dllimport) inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl ends(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _O)
	{_O.put('\0');
	return (_O); }
template<class _E, class _Tr> inline
	basic_ostream<_E, _Tr>&
		__cdecl flush(basic_ostream<_E, _Tr>& _O)
	{_O.flush();
	return (_O); }
__declspec(dllimport) inline basic_ostream<char, char_traits<char> >&
	__cdecl flush(basic_ostream<char, char_traits<char> >& _O)
	{_O.flush();
	return (_O); }
__declspec(dllimport) inline basic_ostream<wchar_t, char_traits<wchar_t> >&
	__cdecl flush(basic_ostream<wchar_t,
		char_traits<wchar_t> >& _O)
	{_O.flush();
	return (_O); }
};

#pragma pack(pop)
#line 400 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"

#line 402 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"
namespace std {
		
template<class _E, class _Tr = char_traits<_E> >
	class basic_istream : virtual public basic_ios<_E, _Tr> {
public:
	typedef basic_istream<_E, _Tr> _Myt;
	typedef basic_ios<_E, _Tr> _Myios;
	typedef basic_streambuf<_E, _Tr> _Mysb;
	typedef istreambuf_iterator<_E, _Tr> _Iter;
	typedef ctype<_E> _Ctype;
	typedef num_get<_E, _Iter> _Nget;
	explicit basic_istream(_Mysb *_S, bool _Isstd = false)
		: _Chcount(0) {init(_S, _Isstd); }
	basic_istream(_Uninitialized)
		{_Addstd(); }
	virtual ~basic_istream()
		{}
	class sentry {
	public:
		explicit sentry(_Myt& _Is, bool _Noskip = false)
			: _Ok(_Is.ipfx(_Noskip)) {}
		operator bool() const
			{return (_Ok); }
	private:
		bool _Ok;
		};
	bool ipfx(bool _Noskip = false)
		{if (good())
			{if (tie() != 0)
				tie()->flush();
			if (!_Noskip && flags() & skipws)
				{const _Ctype& _Fac = use_facet(getloc(), (_Ctype *)0, true);
				try {
				int_type _C = rdbuf()->sgetc();
				while (!_Tr::eq_int_type(_Tr::eof(), _C)
					&& _Fac.is(_Ctype::space,
						_Tr::to_char_type(_C)))
					_C = rdbuf()->snextc();
				} catch (...) { setstate(ios_base::badbit, true); } }
			if (good())
				return (true); }
		setstate(failbit);
		return (false); }
	void isfx()
		{}
	_Myt& operator>>(_Myt& (__cdecl *_F)(_Myt&))
		{return ((*_F)(*this)); }
	_Myt& operator>>(_Myios& (__cdecl *_F)(_Myios&))
		{(*_F)(*(_Myios *)this);
		return (*this); }
	_Myt& operator>>(ios_base& (__cdecl *_F)(ios_base&))
		{(*_F)(*(ios_base *)this);
		return (*this); }
	_Myt& operator>>(_Bool& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(short& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{long _Y;
			const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _Y);
			} catch (...) { setstate(ios_base::badbit, true); }
			if (_St & failbit || _Y < (-32768) || 32767 < _Y)
				_St |= failbit;
			else
				_X = _Y; }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(unsigned short& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(int& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{long _Y;
			const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _Y);
			} catch (...) { setstate(ios_base::badbit, true); }
			if (_St & failbit || _Y < (-2147483647 - 1) || 2147483647 < _Y)
				_St |= failbit;
			else
				_X = _Y; }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(unsigned int& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(long& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(unsigned long& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(float& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(double& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(long double& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(void *& _X)
		{iostate _St = goodbit;
		const sentry _Ok(*this);
		if (_Ok)
			{const _Nget& _Fac = use_facet(getloc(), (_Nget *)0, true);
			try {
			_Fac.get(_Iter(rdbuf()), _Iter(0), *this, _St, _X);
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& operator>>(_Mysb *_Pb)
		{iostate _St = goodbit;
		bool _Copied = false;
		const sentry _Ok(*this);
		if (_Ok && _Pb != 0)
			{try {
			int_type _C = rdbuf()->sgetc();
			if (0) ; else for (; ; _C = rdbuf()->snextc())
				if (_Tr::eq_int_type(_Tr::eof(), _C))
					{_St |= eofbit;
					break; }
				else
					{try {
						if (_Tr::eq_int_type(_Tr::eof(),
							_Pb->sputc(_Tr::to_char_type(_C))))
							break;
					} catch (...) {
						break;
					}
					_Copied = true; }
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(!_Copied ? _St | failbit : _St);
		return (*this); }
	int_type get()
		{int_type _C;
		iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (!_Ok)
			_C = _Tr::eof();
		else
			{try {
			_C = rdbuf()->sbumpc();
			if (_Tr::eq_int_type(_Tr::eof(), _C))
				_St |= eofbit | failbit;
			else
				++_Chcount;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (_C); }
	_Myt& get(_E *_S, streamsize _N)
		{return (get(_S, _N, widen('\n'))); }
	_Myt& get(_E *_S, streamsize _N, _E _D)
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok && 0 < _N)
			{try {
			int_type _C = rdbuf()->sgetc();
			if (0) ; else for (; 0 < --_N; _C = rdbuf()->snextc())
				if (_Tr::eq_int_type(_Tr::eof(), _C))
					{_St |= eofbit;
					break; }
				else if (_Tr::to_char_type(_C) == _D)
					break;
				else
					*_S++ = _Tr::to_char_type(_C), ++_Chcount;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_Chcount == 0 ? _St | failbit : _St);
		*_S = _E(0);
		return (*this); }
	_Myt& get(_E& _X)
		{int_type _C = get();
		if (!_Tr::eq_int_type(_Tr::eof(), _C))
			_X = _Tr::to_char_type(_C);
		return (*this); }
	_Myt& get(_Mysb& _Sb)
		{return (get(_Sb, widen('\n'))); }
	_Myt& get(_Mysb& _Sb, _E _D)
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok)
			{try {
			int_type _C = rdbuf()->sgetc();
			if (0) ; else for (; ; _C = rdbuf()->snextc())
				if (_Tr::eq_int_type(_Tr::eof(), _C))
					{_St |= eofbit;
					break; }
				else
					{try {
						_E _Ch = _Tr::to_char_type(_C);
						if (_Ch == _D
							|| _Tr::eq_int_type(_Tr::eof(),
								_Sb.sputc(_Ch)))
							break;
					} catch (...) {
						break;
					}
					++_Chcount; }
			} catch (...) { setstate(ios_base::badbit, true); } }
		if (_Chcount == 0)
			_St |= failbit;
		setstate(_St);
		return (*this); }
	_Myt& getline(_E *_S, streamsize _N)
		{return (getline(_S, _N, widen('\n'))); }
	_Myt& getline(_E *_S, streamsize _N, _E _D)
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok && 0 < _N)
			{int_type _Di = _Tr::to_int_type(_D);
			try {
			int_type _C = rdbuf()->sgetc();
			if (0) ; else for (; ; _C = rdbuf()->snextc())
				if (_Tr::eq_int_type(_Tr::eof(), _C))
					{_St |= eofbit;
					break; }
				else if (_C == _Di)
					{++_Chcount;
					rdbuf()->snextc();
					break; }
				else if (--_N <= 0)
					{_St |= failbit;
					break; }
				else
					{++_Chcount;
					*_S++ = _Tr::to_char_type(_C); }
			} catch (...) { setstate(ios_base::badbit, true); } }
		*_S = _E(0);
		setstate(_Chcount == 0 ? _St | failbit : _St);
		return (*this); }
	_Myt& ignore(streamsize _N = 1, int_type _Di = _Tr::eof())
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok && 0 < _N)
			{try {
			if (0) ; else for (; ; )
				{int_type _C;
				if (_N != 2147483647 && --_N < 0)
					break;
				else if (_Tr::eq_int_type(_Tr::eof(),
					_C = rdbuf()->sbumpc()))
					{_St |= eofbit;
					break; }
				else
					{++_Chcount;
					if (_C == _Di)
						break; }}
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& read(_E *_S, streamsize _N)
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok)
			{try {
			const streamsize _M = rdbuf()->sgetn(_S, _N);
			_Chcount += _M;
			if (_M != _N)
				_St |= eofbit | failbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	streamsize readsome(_E *_S, streamsize _N)
		{iostate _St = goodbit;
		_Chcount = 0;
		int _M;
		if (rdbuf() == 0)
			_St |= failbit;
		else if ((_M = rdbuf()->in_avail()) < 0)
			_St |= eofbit;
		else if (0 < _M)
			read(_S, _M < _N ? _M : _N);
		setstate(_St);
		return (gcount()); }
	int_type peek()
		{iostate _St = goodbit;
		_Chcount = 0;
		int_type _C;
		const sentry _Ok(*this, true);
		if (!_Ok)
			_C = _Tr::eof();
		else
			{try {
			if (_Tr::eq_int_type(_Tr::eof(),
				_C = rdbuf()->sgetc()))
				_St |= eofbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (_C); }
	_Myt& putback(_E _X)
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok)
			{try {
			if (_Tr::eq_int_type(_Tr::eof(),
				rdbuf()->sputbackc(_X)))
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	_Myt& unget()
		{iostate _St = goodbit;
		_Chcount = 0;
		const sentry _Ok(*this, true);
		if (_Ok)
			{try {
			if (_Tr::eq_int_type(_Tr::eof(),
				rdbuf()->sungetc()))
				_St |= badbit;
			} catch (...) { setstate(ios_base::badbit, true); } }
		setstate(_St);
		return (*this); }
	streamsize gcount() const
		{return (_Chcount); }
	int sync()
		{iostate _St = goodbit;
		int _Ans;
		if (rdbuf() == 0)
			_Ans = -1;
		else if (rdbuf()->pubsync() == -1)
			_St |= badbit, _Ans = -1;
		else
			_Ans = 0;
		setstate(_St);
		return (_Ans); }
	_Myt& seekg(pos_type _P)
		{if (!fail())
			rdbuf()->pubseekpos(_P, in);
		return (*this); }
	_Myt& seekg(off_type _O, ios_base::seekdir _W)
		{if (!fail())
			rdbuf()->pubseekoff(_O, _W, in);
		return (*this); }
	pos_type tellg()
		{if (!fail())
			return (rdbuf()->pubseekoff(0, cur, in));
		else
			return (streampos(_BADOFF)); }
private:
	streamsize _Chcount;
	};
		
template<class _E, class _Tr = char_traits<_E> >
	class basic_iostream : public basic_istream<_E, _Tr>,
		public basic_ostream<_E, _Tr> {
public:
	explicit basic_iostream(basic_streambuf<_E, _Tr> *_S)
		: basic_istream<_E, _Tr>(_S), basic_ostream<_E, _Tr>(_S, false, false)
		{}
	virtual ~basic_iostream()
		{}
	};
		
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, _E *_X)
	{typedef basic_istream<_E, _Tr> _Myis;
	typedef ctype<_E> _Ctype;
	ios_base::iostate _St = ios_base::goodbit;
	_E *_S = _X;
	const _Myis::sentry _Ok(_I);
	if (_Ok)
		{const _Ctype& _Fac = use_facet(_I.getloc(), (_Ctype *)0, true);
		try {
		int _N = 0 < _I.width() ? _I.width() : 2147483647;
		_Myis::int_type _C = _I.rdbuf()->sgetc();
		if (0) ; else for (; 0 < --_N; _C = _I.rdbuf()->snextc())
			if (_Tr::eq_int_type(_Tr::eof(), _C))
				{_St |= ios_base::eofbit;
				break; }
			else if (_Fac.is(_Ctype::space,
				_Tr::to_char_type(_C)))
				break;
			else
				*_S++ = _Tr::to_char_type(_C);
		} catch (...) { (_I).setstate(ios_base::badbit, true); } }
	*_S = _E(0);
	_I.width(0);
	_I.setstate(_S == _X ? _St | ios_base::failbit : _St);
	return (_I); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, _E& _X)
	{typedef basic_istream<_E, _Tr> _Myis;
	_Myis::int_type _C;
	ios_base::iostate _St = ios_base::goodbit;
	const _Myis::sentry _Ok(_I);
	if (_Ok)
		{try {
		_C = _I.rdbuf()->sbumpc();
		if (_Tr::eq_int_type(_Tr::eof(), _C))
			_St |= ios_base::eofbit | ios_base::failbit;
		else
			_X = _Tr::to_char_type(_C);
		} catch (...) { (_I).setstate(ios_base::badbit, true); } }
	_I.setstate(_St);
	return (_I); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, signed char *_X)
	{return (_I >> (char *)_X); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, signed char& _C)
	{return (_I >> (char&)_C); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, unsigned char *_X)
	{return (_I >> (char *)_X); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, unsigned char& _C)
	{return (_I >> (char&)_C); }
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl operator>>(
		basic_istream<_E, _Tr>& _I, signed short * _X)
	{return (_I >> (wchar_t *)_X); }
		
template<class _E, class _Tr> inline
	basic_istream<_E, _Tr>& __cdecl ws(basic_istream<_E, _Tr>& _I)
	{typedef basic_istream<_E, _Tr> _Myis;
	typedef ctype<_E> _Ctype;
	ios_base::iostate _St = ios_base::goodbit;
	const _Myis::sentry _Ok(_I, true);
	if (_Ok)
		{const _Ctype& _Fac = use_facet(_I.getloc(), (_Ctype *)0, true);
		try {
		if (0) ; else for (_Tr::int_type _C = _I.rdbuf()->sgetc(); ;
			_C = _I.rdbuf()->snextc())
			if (_Tr::eq_int_type(_Tr::eof(), _C))
				{_St |= ios_base::eofbit;
				break; }
			else if (!_Fac.is(_Ctype::space,
				_Tr::to_char_type(_C)))
				break;
		} catch (...) { (_I).setstate(ios_base::badbit, true); } }
	_I.setstate(_St);
	return (_I); }
__declspec(dllimport) inline basic_istream<char, char_traits<char> >&
	__cdecl ws(basic_istream<char, char_traits<char> >& _I)
	{typedef char _E;
	typedef char_traits<_E> _Tr;
	ios_base::iostate _St = ios_base::goodbit;
	const basic_istream<_E, _Tr>::sentry _Ok(_I, true);
	if (_Ok)
		{const ctype<_E>& _Fac = use_facet(_I.getloc(), (ctype<_E> *)0, true);
		try {
		if (0) ; else for (_Tr::int_type _C = _I.rdbuf()->sgetc(); ;
			_C = _I.rdbuf()->snextc())
			if (_Tr::eq_int_type(_Tr::eof(), _C))
				{_St |= ios_base::eofbit;
				break; }
			else if (!_Fac.is(ctype<_E>::space,
				_Tr::to_char_type(_C)))
				break;
		} catch (...) { (_I).setstate(ios_base::badbit, true); } }
	_I.setstate(_St);
	return (_I); }
__declspec(dllimport) inline basic_istream<wchar_t, char_traits<wchar_t> >&
	__cdecl ws(basic_istream<wchar_t, char_traits<wchar_t> >& _I)
	{typedef wchar_t _E;
	typedef char_traits<_E> _Tr;
	ios_base::iostate _St = ios_base::goodbit;
	const basic_istream<_E, _Tr>::sentry _Ok(_I, true);
	if (_Ok)
		{const ctype<_E>& _Fac = use_facet(_I.getloc(), (ctype<_E> *)0, true);
		try {
		if (0) ; else for (_Tr::int_type _C = _I.rdbuf()->sgetc(); ;
			_C = _I.rdbuf()->snextc())
			if (_Tr::eq_int_type(_Tr::eof(), _C))
				{_St |= ios_base::eofbit;
				break; }
			else if (_Fac.is(ctype<_E>::space,
				_Tr::to_char_type(_C)))
				break;
		} catch (...) { (_I).setstate(ios_base::badbit, true); } }
	_I.setstate(_St);
	return (_I); }
};

#pragma pack(pop)
#line 560 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"

#line 562 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"
namespace std {
		
static ios_base::Init _Ios_init;
extern __declspec(dllimport) istream cin;
extern __declspec(dllimport) ostream cout;
extern __declspec(dllimport) ostream cerr, clog;
		
class __declspec(dllimport) _Winit {
public:
	_Winit();
	~_Winit();
private:
	static int _Init_cnt;
	};
		
static _Winit _Wios_init;
extern __declspec(dllimport) wistream wcin;
extern __declspec(dllimport) wostream wcout, wcerr, wclog;
};

#pragma pack(pop)
#line 31 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"

#line 33 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"





#line 44 "../../../src\\Pegasus/Common/Char16.h"
#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 45 "../../../src\\Pegasus/Common/Char16.h"

namespace Pegasus {

class __declspec(dllexport) Char16 
{
public:

    Char16() : _code(0) { }

    Char16(Uint16 x) : _code(x) { }

    Char16(const Char16& x) : _code(x._code) { }

    Char16& operator=(Uint16 x) { _code = x; return *this; }

    Char16& operator=(const Char16& x) {_code = x._code; return *this;}

    operator Uint16() const { return _code; }

    Uint16 getCode() const { return _code; }

private:

    Uint16 _code;
};

inline Boolean operator==(const Char16& x, const Char16& y)
{
    return x.getCode() == y.getCode();
}

inline Boolean operator==(const Char16& x, char y)
{
    return x.getCode() == y;
}

inline Boolean operator==(char x, const Char16& y)
{
    return x == y.getCode();
}

__declspec(dllexport) std::ostream& operator<<(
    std::ostream& os, const Char16& x);

}

#line 92 "../../../src\\Pegasus/Common/Char16.h"
#line 45 "Array.h"
#line 1 "../../../src\\Pegasus/Common/Memory.h"















































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 49 "../../../src\\Pegasus/Common/Memory.h"
#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 50 "../../../src\\Pegasus/Common/Memory.h"
#line 1 "../../../src\\Pegasus/Common/Type.h"


















































































































































#line 51 "../../../src\\Pegasus/Common/Memory.h"
#line 1 "../../../src\\Pegasus/Common/Char16.h"



























































































#line 52 "../../../src\\Pegasus/Common/Memory.h"

namespace Pegasus {

template<class T>
inline void Zeros(T* items, Uint32 size)
{
    memset(items, 0, sizeof(T) * size);
}

template<class T>
inline void Destroy(T* items, Uint32 size)
{
    while (size--)
	items++->~T();
}

inline void Destroy(Boolean* items, Uint32 size) { }
inline void Destroy(Uint8* items, Uint32 size) { }
inline void Destroy(Sint8* items, Uint32 size) { }
inline void Destroy(Uint16* items, Uint32 size) { }
inline void Destroy(Sint16* items, Uint32 size) { }
inline void Destroy(Uint32* items, Uint32 size) { }
inline void Destroy(Sint32* items, Uint32 size) { }
inline void Destroy(Uint64* items, Uint32 size) { }
inline void Destroy(Sint64* items, Uint32 size) { }
inline void Destroy(Real32* items, Uint32 size) { }
inline void Destroy(Real64* items, Uint32 size) { }
inline void Destroy(Char16* items, Uint32 size) { }

template<class T, class U>
inline void CopyToRaw(T* to, const U* from, Uint32 size)
{
    while (size--)
	new(to++) T(*from++);
}

inline void CopyToRaw(Boolean* to, const Boolean* from, Uint32 size)
{
    memcpy(to, from, sizeof(Boolean) * size);
}

inline void CopyToRaw(Uint8* to, const Uint8* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint8) * size);
}

inline void CopyToRaw(Sint8* to, const Sint8* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint8) * size);
}

inline void CopyToRaw(Uint16* to, const Uint16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint16) * size);
}

inline void CopyToRaw(Sint16* to, const Sint16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint16) * size);
}

inline void CopyToRaw(Uint32* to, const Uint32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint32) * size);
}

inline void CopyToRaw(Sint32* to, const Sint32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint32) * size);
}

inline void CopyToRaw(Uint64* to, const Uint64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Uint64) * size);
}

inline void CopyToRaw(Sint64* to, const Sint64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Sint64) * size);
}

inline void CopyToRaw(Real32* to, const Real32* from, Uint32 size)
{
    memcpy(to, from, sizeof(Real32) * size);
}

inline void CopyToRaw(Real64* to, const Real64* from, Uint32 size)
{
    memcpy(to, from, sizeof(Real64) * size);
}

inline void CopyToRaw(Char16* to, const Char16* from, Uint32 size)
{
    memcpy(to, from, sizeof(Char16) * size);
}

template<class T>
inline void InitializeRaw(T* items, Uint32 size)
{
    while (size--)
	items++->~T();
}

inline void InitializeRaw(Boolean* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint8* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint8* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint16* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint16* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Uint64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Sint64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Real32* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Real64* items, Uint32 size) { Zeros(items, size); }
inline void InitializeRaw(Char16* items, Uint32 size) { Zeros(items, size); }

}

#line 171 "../../../src\\Pegasus/Common/Memory.h"
#line 46 "Array.h"

namespace Pegasus {












template<class T>
struct ArrayRep
{
    Uint32 size;
    Uint32 capacity;

    
    
    
    
    

    union
    {
	Uint32 ref;
	Uint64 alignment;
    };

    T* data() { return (T*)(this + 1); }

    const T* data() const { return (const T*)(this + 1); }

    ArrayRep* clone() const;

    
    
    
    

    static ArrayRep* create(Uint32 size);

    static void inc(const ArrayRep* rep_);

    static void dec(const ArrayRep* rep_);

    static ArrayRep* getNullRep();
};

template<class T>
ArrayRep<T>* ArrayRep<T>::clone() const
{
    ArrayRep<T>* rep = ArrayRep<T>::create(capacity);
    rep->size = size;
    CopyToRaw(rep->data(), data(), size);
    return rep;
}

template<class T>
ArrayRep<T>* ArrayRep<T>::create(Uint32 size)
{
    

    Uint32 capacity = 8;

    while (capacity < size)
	capacity <<= 1;

    

    ArrayRep* rep = 
	(ArrayRep*)operator new(sizeof(ArrayRep) + sizeof(T) * capacity);

    rep->size = size;
    rep->capacity = capacity;
    rep->ref = 1;

    return rep;
}

template<class T>
void ArrayRep<T>::inc(const ArrayRep<T>* rep)
{
    if (rep)
	((ArrayRep*)rep)->ref++;
}

template<class T>
void ArrayRep<T>::dec(const ArrayRep<T>* rep_)
{
    ArrayRep* rep = (ArrayRep*)rep_;

    if (rep && --rep->ref == 0)
    {
	Destroy(rep->data(), rep->size);
	operator delete(rep);
    }
}

template<class T>
ArrayRep<T>* ArrayRep<T>::getNullRep()
{
    static ArrayRep<T>* nullRep = 0;

    if (!nullRep)
    {
	nullRep = ArrayRep::create(0);

	
	
	ArrayRep<T>::inc(nullRep);
    }

    
    ArrayRep<T>::inc(nullRep);
    return nullRep;
}







class Value;

__declspec(dllexport) void ThrowOutOfBounds();

class Value;

template<class T>
class Array
{
public:

    Array();

    Array(const Array<T>& x);

    Array(Uint32 size);

    Array(Uint32 size, const T& x);

    Array(const T* items, Uint32 size);

    Array(ArrayRep<T>* rep)
    {
	Rep::inc(_rep = rep);
    }

    ~Array();

    Array<T>& operator=(const Array<T>& x);

    void clear();

    void reserve(Uint32 capacity)
    {
	if (capacity > _rep->capacity)
	    _reserveAux(capacity);
    }

    void grow(Uint32 size, const T& x);

    void swap(Array<T>& x);

    Uint32 getSize() const { return _rep->size; }

    const T* getData() const { return _rep->data(); }

    T& operator[](Uint32 pos);

    const T& operator[](Uint32 pos) const;

    void append(const T& x);

    void append(const T* x, Uint32 size);

    void appendArray(const Array<T>& x)
    {
	append(x.getData(), x.getSize());
    }

    void prepend(const T& x);

    void prepend(const T* x, Uint32 size);

    void insert(Uint32 pos, const T& x);

    void insert(Uint32 pos, const T* x, Uint32 size);

    void remove(Uint32 pos);

    void remove(Uint32 pos, Uint32 size);

private:

    void set(ArrayRep<T>* rep)
    {
	if (_rep != rep)
	{
	    Rep::dec(_rep);
	    Rep::inc(_rep = rep);
	}
    }

    void _reserveAux(Uint32 capacity);

    T* _data() const { return _rep->data(); }

    typedef ArrayRep<T> Rep;

    void _copyOnWriteAux();

    void _copyOnWrite()
    {
	if (_rep->ref != 1)
	    _copyOnWriteAux();
    }

    Rep* _rep;

    friend Value;
};

template<class T>
Array<T>::Array()
{
    _rep = Rep::getNullRep();
}

template<class T>
Array<T>::Array(const Array<T>& x)
{
    Rep::inc(_rep = x._rep);
}

template<class T>
Array<T>::Array(Uint32 size)
{
    _rep = Rep::create(size);
    InitializeRaw(_rep->data(), size);
}

template<class T>
Array<T>::Array(Uint32 size, const T& x)
{
    _rep = Rep::create(size);

    T* data = _rep->data();
    
    while (size--)
	new(data++) T(x);
}

template<class T>
Array<T>::Array(const T* items, Uint32 size)
{
    _rep = Rep::create(size);
    CopyToRaw(_rep->data(), items, size);
}

template<class T>
Array<T>::~Array()
{
    Rep::dec(_rep);
}

template<class T>
Array<T>& Array<T>::operator=(const Array<T>& x)
{
    if (x._rep != _rep)
    {
	Rep::dec(_rep);
	Rep::inc(_rep = x._rep);
    }
    return *this;
}

template<class T>
void Array<T>::clear()
{
    Rep::dec(_rep);
    _rep = Rep::getNullRep();
}

template<class T>
void Array<T>::_reserveAux(Uint32 capacity)
{
    Uint32 size = getSize();
    Rep* rep = Rep::create(capacity);
    rep->size = size;
    CopyToRaw(rep->data(), _rep->data(), size);
    Rep::dec(_rep);
    _rep = rep;
}

template<class T>
void Array<T>::grow(Uint32 size, const T& x)
{
    Uint32 oldSize = _rep->size;
    reserve(oldSize + size);
    _copyOnWrite();

    T* p = _rep->data() + oldSize;
    Uint32 n = size;

    while (n--)
	new(p) T(x);

    _rep->size += size;
}

template<class T>
void Array<T>::swap(Array<T>& x)
{
    Rep* tmp = _rep;
    _rep = x._rep;
    x._rep = tmp;
}

template<class T>
inline T& Array<T>::operator[](Uint32 pos) 
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    return _rep->data()[pos];
}

template<class T>
inline const T& Array<T>::operator[](Uint32 pos) const
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    return _rep->data()[pos];
}

template<class T>
void Array<T>::append(const T& x)
{
    reserve(getSize() + 1);
    _copyOnWrite();
    new (_data() + getSize()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::append(const T* x, Uint32 size)
{
    reserve(getSize() + size);
    _copyOnWrite();

    
    CopyToRaw(_data() + getSize(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::prepend(const T& x)
{
    reserve(getSize() + 1);
    _copyOnWrite();
    memmove(_data() + 1, _data(), sizeof(T) * getSize());
    new(_data()) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::prepend(const T* x, Uint32 size)
{
    reserve(getSize() + size);
    _copyOnWrite();
    memmove(_data() + size, _data(), sizeof(T) * getSize());
    CopyToRaw(_data(), x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T& x)
{
    if (pos > getSize())
	ThrowOutOfBounds();

    reserve(getSize() + 1);
    _copyOnWrite();

    Uint32 n = getSize() - pos;

    if (n)
	memmove(_data() + pos + 1, _data() + pos, sizeof(T) * n);

    new(_data() + pos) T(x);
    _rep->size++;
}

template<class T>
void Array<T>::insert(Uint32 pos, const T* x, Uint32 size)
{
    if (pos + size > getSize())
	ThrowOutOfBounds();

    reserve(getSize() + size);
    _copyOnWrite();

    Uint32 n = getSize() - pos;

    if (n)
	memmove(_data() + pos + size, _data() + pos, sizeof(T) * n);

    CopyToRaw(_data() + pos, x, size);
    _rep->size += size;
}

template<class T>
void Array<T>::remove(Uint32 pos)
{
    if (pos >= getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    (_data() + pos)->~T();

    Uint32 rem = getSize() - pos - 1;

    if (rem)
	memmove(_data() + pos, _data() + pos + 1, sizeof(T) * rem);

    _rep->size--;
}

template<class T>
void Array<T>::remove(Uint32 pos, Uint32 size)
{
    if (pos + size > getSize())
	ThrowOutOfBounds();

    _copyOnWrite();

    Destroy(_data() + pos, size);

    Uint32 rem = getSize() - (pos + size);

    if (rem)
	memmove(_data() + pos, _data() + pos + size, sizeof(T) * rem);

    _rep->size -= size;
}

template<class T>
void Array<T>::_copyOnWriteAux()
{
    if (_rep->ref != 1)
    {
	Rep* rep = _rep->clone();
	Rep::dec(_rep);
	_rep = rep;
    }
}

template<class T>
Boolean operator==(const Array<T>& x, const Array<T>& y)
{
    if (x.getSize() != y.getSize())
	return false;

    if (0) ; else for (Uint32 i = 0, n = x.getSize(); i < n; i++)
    {
	if (!(x[i] == y[i]))
	    return false;
    }

    return true;
}

template<class T>
void BubbleSort(Array<T>& x)
{
    Uint32 n = x.getSize();

    if (n < 2)
	return;

    if (0) ; else for (Uint32 i = 0; i < n - 1; i++)
    {
	if (0) ; else for (Uint32 j = 0; j < n - 1; j++)
	{
	    if (x[j] > x[j+1])
	    {
		T t = x[j];
		x[j] = x[j+1];
		x[j+1] = t;
	    }
	}
    }
}

template<class T>
void Print(Array<T>& x)
{
    if (0) ; else for (Uint32 i = 0, n = x.getSize(); i < n; i++)
	cout << x[i] << endl;
}

}

#line 560 "Array.h"
#line 30 "Headers.cpp"
#line 1 "Char16.h"



























































































#line 31 "Headers.cpp"
#line 1 "DateTime.h"
























































































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 90 "DateTime.h"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"





































#line 91 "DateTime.h"

namespace Pegasus {

class DateTime;
__declspec(dllexport) Boolean operator==(const DateTime& x, const DateTime& y);

class __declspec(dllexport) DateTime
{
public:

    enum { FORMAT_LENGTH = 25 };

    DateTime();

    

    DateTime(const char* str);

    DateTime(const DateTime& x);

    DateTime& operator=(const DateTime& x);

    

    Boolean isNull() const;

    const char* getString() const;

    

    void set(const char* str);

    void clear();

    __declspec(dllexport) friend Boolean operator==(
	const DateTime& x, 
	const DateTime& y);

private:

    Boolean _set(const char* str);

    char _rep[FORMAT_LENGTH + 1];
};

__declspec(dllexport) Boolean operator==(const DateTime& x, const DateTime& y);

__declspec(dllexport) std::ostream& operator<<(std::ostream& os, const DateTime& x);

}

#line 143 "DateTime.h"
#line 32 "Headers.cpp"
#line 1 "Exception.h"







































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 41 "Exception.h"
#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 42 "Exception.h"
#line 1 "../../../src\\Pegasus/Common/String.h"







































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"





































#line 41 "../../../src\\Pegasus/Common/String.h"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 42 "../../../src\\Pegasus/Common/String.h"
#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 43 "../../../src\\Pegasus/Common/String.h"
#line 1 "../../../src\\Pegasus/Common/Char16.h"



























































































#line 44 "../../../src\\Pegasus/Common/String.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 45 "../../../src\\Pegasus/Common/String.h"

namespace Pegasus {

__declspec(dllexport) void ThrowNullPointer();

__declspec(dllexport) Uint32 StrLen(const Char16* str);

inline Uint32 StrLen(const char* str)
{
    if (!str)
	ThrowNullPointer();

    return strlen(str);
}

class __declspec(dllexport) String
{
public:

    String();

    String(const String& x);

    String(const String& x, Uint32 n);

    String(const Char16* x);

    String(const Char16* x, Uint32 n);

    String(const char* x);

    String(const char* x, Uint32 n);

    ~String() { }

    String& operator=(const String& x) { _rep = x._rep; return *this; }

    String& operator=(const Char16* x) { assign(x); return *this; }

    String& assign(const String& x) { _rep = x._rep; return *this; }

    String& assign(const Char16* x);

    String& assign(const Char16* x, Uint32 n);

    String& assign(const char* x);

    String& assign(const char* x, Uint32 n);

    void clear() { _rep.clear(); _rep.append('\0'); }

    void reserve(Uint32 capacity) { _rep.reserve(capacity + 1); }

    Uint32 getLength() const { return _rep.getSize() - 1; }

    const Char16* getData() const { return _rep.getData(); }

    
    
    
    

    char* allocateCString(Uint32 extraBytes = 0, Boolean noThrow = false) const;

    
    
    
    
    

    void appendToCString(
	char* str, 
	Uint32 length = Uint32(-1),
	Boolean noThrow = false) const;

    Char16& operator[](Uint32 i);

    const Char16 operator[](Uint32 i) const;

    String& append(const Char16& c) 
    { 
	_rep.insert(_rep.getSize() - 1, c); 
	return *this;
    }

    String& append(const Char16* str, Uint32 n);

    String& append(const String& str) 
    {
	return append(str.getData(), str.getLength());
    }

    String& operator+=(const String& x)
    {
	return append(x);
    }

    String& operator+=(Char16 c)
    {
	return append(c);
    }

    String& operator+=(char c)
    {
	return append(Char16(c));
    }

    void remove(Uint32 pos, Uint32 size = Uint32(-1));

    String subString(Uint32 pos, Uint32 length = Uint32(-1)) const;

    Uint32 find(Char16 c) const;

    static int compare(const Char16* s1, const Char16* s2, Uint32 n);

    static int compare(const Char16* s1, const Char16* s2);

    static const String EMPTY;

private:

    static Uint32 _min(Uint32 x, Uint32 y) { return x < y ? x : y; }

    Array<Char16> _rep;	
};

__declspec(dllexport) Boolean operator==(const String& x, const String& y);

__declspec(dllexport) Boolean operator==(const String& x, const char* y);

__declspec(dllexport) Boolean operator==(const char* x, const String& y);

inline Boolean operator!=(const String& x, const String& y)
{
    return !operator==(x, y);
}

__declspec(dllexport) std::ostream& operator<<(std::ostream& os, const String& x);

inline String operator+(const String& x, const String& y)
{
    return String(x).append(y);
}

inline Boolean operator<(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) < 0;
}

inline Boolean operator<=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) <= 0;
}

inline Boolean operator>(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) > 0;
}

inline Boolean operator>=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) >= 0;
}

}

#line 212 "../../../src\\Pegasus/Common/String.h"
#line 43 "Exception.h"

namespace Pegasus {

class __declspec(dllexport) Exception
{
public:

    Exception(const String& message);

    Exception(const char* message);

    ~Exception();

    const String& getMessage() const { return _message; }

protected:

    String _message;
};

class __declspec(dllexport) AssertionFailureException : public Exception
{
public:

    AssertionFailureException(
	const char* file, 
	size_t line,
	const String& message);
};










class __declspec(dllexport) BadReference : public Exception
{
public:

    BadReference(const String& message) : Exception(message) { }
};

class __declspec(dllexport) OutOfBounds : public Exception
{
public:

    static const char MSG[];

    OutOfBounds() : Exception(MSG) { }
};

class __declspec(dllexport) AlreadyExists : public Exception
{
public:

    static const char MSG[];

    AlreadyExists(const String& x = String()) : Exception(MSG + x) { }
};

class __declspec(dllexport) NullPointer : public Exception
{
public:

    static const char MSG[];

    NullPointer() : Exception(MSG) { }
};

class __declspec(dllexport) IllegalName : public Exception
{
public:

    static const char MSG[];

    IllegalName() : Exception(MSG) { }
};

class __declspec(dllexport) UnitializedHandle : public Exception
{
public:

    static const char MSG[];

    UnitializedHandle() : Exception(MSG) { }
};

class __declspec(dllexport) NoSuchSuperClass : public Exception
{
public:

    static const char MSG[];

    NoSuchSuperClass(const String& className) : Exception(MSG + className) { }
};

class __declspec(dllexport) InvalidPropertyOverride : public Exception
{
public:

    static const char MSG[];

    InvalidPropertyOverride(const String& message)
	: Exception(MSG + message) { }
};

class __declspec(dllexport) InvalidMethodOverride : public Exception
{
public:

    static const char MSG[];

    InvalidMethodOverride(const String& message)
	: Exception(MSG + message) { }
};

class __declspec(dllexport) UndeclaredQualifier : public Exception
{
public:

    static const char MSG[];

    UndeclaredQualifier(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

class __declspec(dllexport) BadQualifierType : public Exception
{
public:

    static const char MSG[];

    BadQualifierType(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

class __declspec(dllexport) BadQualifierScope : public Exception
{
public:

    static const char MSG[];

    BadQualifierScope(const String& qualifierName) 
	: Exception(MSG + qualifierName) { }
};

class __declspec(dllexport) BadQualifierOverride : public Exception
{
public:

    static const char MSG[];

    BadQualifierOverride(const String& qualifierName) 
	: Exception(MSG + qualifierName) { }
};

class __declspec(dllexport) NullType : public Exception
{
public:

    static const char MSG[];

    NullType() : Exception(MSG) { }
};

class __declspec(dllexport) AddedReferenceToClass : public Exception
{
public:

    static const char MSG[];

    AddedReferenceToClass(const String& className) 
	: Exception(MSG + className) { }
};

class __declspec(dllexport) ClassAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    ClassAlreadyResolved(const String& className) 
	: Exception(MSG + className) { }
};

class __declspec(dllexport) ClassNotResolved : public Exception
{
public:

    static const char MSG[];

    ClassNotResolved(const String& className) 
	: Exception(MSG + className) { }
};

class __declspec(dllexport) InstanceAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    InstanceAlreadyResolved() : Exception(MSG) { }
};

class __declspec(dllexport) InstantiatedAbstractClass : public Exception
{
public:

    static const char MSG[];

    InstantiatedAbstractClass() : Exception(MSG) { }
};

class __declspec(dllexport) NoSuchProperty : public Exception
{
public:

    static const char MSG[];

    NoSuchProperty(const String& propertyName) 
	: Exception(MSG + propertyName) { }
};

class __declspec(dllexport) TruncatedCharacter : public Exception
{
public:

    static const char MSG[];

    TruncatedCharacter() : Exception(MSG) { }
};

class __declspec(dllexport) ExpectedReferenceValue : public Exception
{
public:

    static const char MSG[];

    ExpectedReferenceValue() : Exception(MSG) { }
};

class __declspec(dllexport) MissingReferenceClassName : public Exception
{
public:

    static const char MSG[];

    MissingReferenceClassName() : Exception(MSG) { }
};

class __declspec(dllexport) IllegalTypeTag : public Exception
{
public:

    static const char MSG[];

    IllegalTypeTag() : Exception(MSG) { }
};

class __declspec(dllexport) TypeMismatch : public Exception
{
public:

    static const char MSG[];

    TypeMismatch() : Exception(MSG) { }
};

class __declspec(dllexport) NoSuchFile : public Exception
{
public:

    static const char MSG[];

    NoSuchFile(const String& fileName) : Exception(MSG + fileName) { }
};

class __declspec(dllexport) FailedToRemoveDirectory : public Exception
{
public:

    static const char MSG[];

    FailedToRemoveDirectory(const String& path) : Exception(MSG + path) { }
};

class __declspec(dllexport) FailedToRemoveFile : public Exception
{
public:

    static const char MSG[];

    FailedToRemoveFile(const String& path) : Exception(MSG + path) { }
};

class __declspec(dllexport) NoSuchDirectory : public Exception
{
public:

    static const char MSG[];

    NoSuchDirectory(const String& directoryName) 
	: Exception(MSG + directoryName) { }
};

class __declspec(dllexport) ChangeDirectoryFailed : public Exception
{
public:

    static const char MSG[];

    ChangeDirectoryFailed(const String& directoryName) 
	: Exception(MSG + directoryName) { }
};

class __declspec(dllexport) CannotCreateDirectory : public Exception
{
public:

    static const char MSG[];

    CannotCreateDirectory(const String& path) 
	: Exception(MSG + path) { }
};

class __declspec(dllexport) NoSuchNameSpace : public Exception
{
public:

    static const char MSG[];

    NoSuchNameSpace(const String& directoryName) 
	: Exception(MSG + directoryName) { }
};

class __declspec(dllexport) CannotOpenFile : public Exception
{
public:

    static const char MSG[];

    CannotOpenFile(const String& path) 
	: Exception(MSG + path) { }
};

class __declspec(dllexport) NotImplemented : public Exception
{
public:

    static const char MSG[];

    NotImplemented(const String& method) : Exception(MSG + method) { }
};

class __declspec(dllexport) CimException : public Exception
{
public:

    enum Code
    {
	SUCCESS = 0,

	
	

	FAILED = 1,

	

	ACCESS_DENIED = 2,

	

	INVALID_NAMESPACE = 3,

	

	INVALID_PARAMETER = 4,

	

	INVALID_CLASS = 5,

	

	NOT_FOUND = 6,

	

	NOT_SUPPORTED = 7,

	
	

	CLASS_HAS_CHILDREN = 8,

	
	

	CLASS_HAS_INSTANCES = 9,

	
	

	INVALID_SUPERCLASS = 10,

	

	ALREADY_EXISTS = 11,

	

	NO_SUCH_PROPERTY = 12,

	

	TYPE_MISMATCH = 13,

	

	QUERY_LANGUAGE_NOT_SUPPORTED = 14,

	

	INVALID_QUERY = 15,

	

	METHOD_NOT_AVAILABLE = 16,

	

	METHOD_NOT_FOUND = 17
    };

    CimException(Code code);

    CimException::Code getCode() const { return _code; }

    static const char* codeToString(Code code);

private:

    Code _code;
};

class __declspec(dllexport) StackUnderflow : public Exception
{
public:

    static const char MSG[];

    StackUnderflow() : Exception(MSG) { }
};

class __declspec(dllexport) BadFormat : public Exception
{
public:

    static const char MSG[];

    BadFormat() : Exception(MSG) { }
};

class __declspec(dllexport) BadDateTimeFormat : public Exception
{
public:

    static const char MSG[];

    BadDateTimeFormat() : Exception(MSG) { }
};

class __declspec(dllexport) IncompatibleTypes : public Exception
{
public:

    static const char MSG[];

    IncompatibleTypes() : Exception(MSG) { }
};

class __declspec(dllexport) BadlyFormedCGIQueryString : public Exception
{
public:

    static const char MSG[];

    BadlyFormedCGIQueryString() : Exception(MSG) { }
};

}

#line 541 "Exception.h"
#line 33 "Headers.cpp"
#line 1 "Indentor.h"












































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 46 "Indentor.h"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"





































#line 47 "Indentor.h"

namespace Pegasus {


class Indentor;
__declspec(dllexport) std::ostream& operator<<(std::ostream& os, const Indentor& x);

class __declspec(dllexport) Indentor
{
public:

    Indentor(Uint32 level) : _level(level) { }

    __declspec(dllexport) friend std::ostream& operator<<(
	std::ostream& os, const Indentor& x);

    Uint32 getLevel() const { return _level; }

private:

    Uint32 _level;
};

__declspec(dllexport) std::ostream& operator<<(std::ostream& os, const Indentor& x);

}

#line 75 "Indentor.h"
#line 34 "Headers.cpp"
#line 1 "Memory.h"










































































































































































#line 35 "Headers.cpp"
#line 1 "Name.h"


















































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 52 "Name.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 53 "Name.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 54 "Name.h"

namespace Pegasus {

class __declspec(dllexport) Name
{
public:

    
    

    static Boolean legal(const Char16* name);

    static Boolean legal(const String& name) 
    { 
	return legal(name.getData()); 
    }

    
    

    static Boolean equal(const String& name1, const String& name2);

private:

    Name() { }
};

}

#line 84 "Name.h"
#line 36 "Headers.cpp"
#line 1 "Scope.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "Scope.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 42 "Scope.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "Scope.h"

namespace Pegasus {

namespace Scope
{
    __declspec(dllexport) extern const Uint32 NONE;
    __declspec(dllexport) extern const Uint32 CLASS;
    __declspec(dllexport) extern const Uint32 ASSOCIATION;
    __declspec(dllexport) extern const Uint32 INDICATION;
    __declspec(dllexport) extern const Uint32 PROPERTY;
    __declspec(dllexport) extern const Uint32 REFERENCE;
    __declspec(dllexport) extern const Uint32 METHOD;
    __declspec(dllexport) extern const Uint32 PARAMETER;
    __declspec(dllexport) extern const Uint32 ANY;
}

__declspec(dllexport) String ScopeToString(Uint32 scope);

__declspec(dllexport) void ScopeToXml(Array<Sint8>& out, Uint32 scope);

}

#line 66 "Scope.h"
#line 37 "Headers.cpp"
#line 1 "String.h"



















































































































































































































#line 38 "Headers.cpp"
#line 1 "String.h"



















































































































































































































#line 39 "Headers.cpp"
#line 1 "String.h"



















































































































































































































#line 40 "Headers.cpp"
#line 1 "Type.h"


















































































































































#line 41 "Headers.cpp"
#line 1 "Union.h"










































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 44 "Union.h"
#line 1 "../../../src\\Pegasus/Common/Type.h"


















































































































































#line 45 "Union.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 46 "Union.h"
#line 1 "../../../src\\Pegasus/Common/DateTime.h"














































































































































#line 47 "Union.h"
#line 1 "../../../src\\Pegasus/Common/Reference.h"































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 33 "../../../src\\Pegasus/Common/Reference.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 34 "../../../src\\Pegasus/Common/Reference.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 35 "../../../src\\Pegasus/Common/Reference.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 36 "../../../src\\Pegasus/Common/Reference.h"

namespace Pegasus {







class __declspec(dllexport) KeyBinding
{
public:

    enum Type { BOOLEAN, STRING, NUMERIC };

    KeyBinding();

    KeyBinding(const KeyBinding& x);

    KeyBinding(const String& name, const String& value, Type type);

    ~KeyBinding();

    KeyBinding& operator=(const KeyBinding& x);

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name) 
    { 
	_name = name; 
    }

    const String& getValue() const 
    { 
	return _value; 
    }

    void setValue(const String& value) 
    { 
	_value = value; 
    }

    Type getType() const 
    { 
	return _type; 
    }

    void setType(Type type) 
    { 
	_type = type; 
    }

    static const char* typeToString(Type type);

private:

    String _name;
    String _value;
    Type _type;

    friend Boolean operator==(const KeyBinding& x, const KeyBinding& y);
};

inline Boolean operator==(const KeyBinding& x, const KeyBinding& y)
{
    return x._name == y._name && x._value == y._value && x._type == y._type;
}







class __declspec(dllexport) Reference 
{
public:

    Reference();

    Reference(const Reference& x);

    Reference(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = Array<KeyBinding>());

    virtual ~Reference();

    Reference& operator=(const Reference& x);

    

    static Array<KeyBinding> _getDefaultArray()
    {
	return Array<KeyBinding>();
    }

    void set(
	const String& host,
	const String& nameSpace,
	const String& className, 
	const Array<KeyBinding>& keyBindings = _getDefaultArray());

    const String& getHost() const 
    {
	return _host; 
    }

    void setHost(const String& host)
    {
	_host = host;
    }

    const String& getNameSpace() const 
    {
	return _nameSpace; 
    }

    void setNameSpace(const String& nameSpace);

    const String& getClassName() const 
    { 
	return _className; 
    }

    void setClassName(const String& className);

    const Array<KeyBinding>& getKeyBindings() const 
    { 
	return _keyBindings; 
    }

    void setKeyBindings(const Array<KeyBinding>& keyBindings);

    Boolean identical(const Reference& x) const;

    void toXml(Array<Sint8>& out) const;

    void print() const;

    void nameSpaceToXml(Array<Sint8>& out) const;

    void localNameSpaceToXml(Array<Sint8>& out) const;

    void instanceNameToXml(Array<Sint8>& out) const;

    void classNameToXml(Array<Sint8>& out) const;

private:

    String _host;
    String _nameSpace;
    String _className;
    Array<KeyBinding> _keyBindings;
};

inline Boolean operator==(const Reference& x, const Reference& y)
{
    return x.identical(y);
}

}

#line 205 "../../../src\\Pegasus/Common/Reference.h"
#line 48 "Union.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 49 "Union.h"

namespace Pegasus {

union Union
{
    Uint8 _booleanValue;
    Uint8 _uint8Value;
    Sint8 _sint8Value;
    Uint16 _uint16Value;
    Sint16 _sint16Value;
    Uint32 _uint32Value;
    Sint32 _sint32Value;
    Uint64 _uint64Value;
    Sint64 _sint64Value;
    Real32 _real32Value;
    Real64 _real64Value;
    Uint16 _char16Value;
    String* _stringValue;
    DateTime* _dateTimeValue;
    Reference* _referenceValue;

    ArrayRep<Boolean>* _booleanArray;
    ArrayRep<Uint8>* _uint8Array;
    ArrayRep<Sint8>* _sint8Array;
    ArrayRep<Uint16>* _uint16Array;
    ArrayRep<Sint16>* _sint16Array;
    ArrayRep<Uint32>* _uint32Array;
    ArrayRep<Sint32>* _sint32Array;
    ArrayRep<Uint64>* _uint64Array;
    ArrayRep<Sint64>* _sint64Array;
    ArrayRep<Real32>* _real32Array;
    ArrayRep<Real64>* _real64Array;
    ArrayRep<Char16>* _char16Array;
    ArrayRep<String>* _stringArray;
    ArrayRep<DateTime>* _dateTimeArray;

    void* _voidPtr;
};

}

#line 91 "Union.h"
#line 42 "Headers.cpp"
#line 1 "Value.h"










































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 44 "Value.h"
#line 1 "../../../src\\Pegasus/Common/Type.h"


















































































































































#line 45 "Value.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 46 "Value.h"
#line 1 "../../../src\\Pegasus/Common/DateTime.h"














































































































































#line 47 "Value.h"
#line 1 "../../../src\\Pegasus/Common/Union.h"


























































































#line 48 "Value.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 49 "Value.h"

namespace Pegasus {


class Value;
__declspec(dllexport) Boolean operator==(const Value& x, const Value& y);

class __declspec(dllexport) Value
{
public:

    Value();

    Value(Boolean x) { _init(); set(x); }

    Value(Uint8 x) { _init(); set(x); }

    Value(Sint8 x) { _init(); set(x); }

    Value(Uint16 x) { _init(); set(x); }

    Value(Sint16 x) { _init(); set(x); }

    Value(Uint32 x) { _init(); set(x); }

    Value(Sint32 x) { _init(); set(x); }

    Value(Uint64 x) { _init(); set(x); }

    Value(Sint64 x) { _init(); set(x); }

    Value(Real32 x) { _init(); set(x); }

    Value(Real64 x) { _init(); set(x); }

    Value(const Char16& x) { _init(); set(x); }

    Value(const String& x) { _init(); set(x); }

    Value(const char* x) { _init(); set(x); }

    Value(const DateTime& x) { _init(); set(x); }

    Value(const Reference& x) { _init(); set(x); }

    Value(const Array<Boolean>& x) { _init(); set(x); }

    Value(const Array<Uint8>& x) { _init(); set(x); }

    Value(const Array<Sint8>& x) { _init(); set(x); }

    Value(const Array<Uint16>& x) { _init(); set(x); }

    Value(const Array<Sint16>& x) { _init(); set(x); }

    Value(const Array<Uint32>& x) { _init(); set(x); }

    Value(const Array<Sint32>& x) { _init(); set(x); }

    Value(const Array<Uint64>& x) { _init(); set(x); }

    Value(const Array<Sint64>& x) { _init(); set(x); }

    Value(const Array<Real32>& x) { _init(); set(x); }

    Value(const Array<Real64>& x) { _init(); set(x); }

    Value(const Array<Char16>& x) { _init(); set(x); }

    Value(const Array<String>& x) { _init(); set(x); }

    Value(const Array<DateTime>& x) { _init(); set(x); }

    Value(const Value& x);

    ~Value();

    Value& operator=(const Value& x) 
    { 
	assign(x); return *this; 
    }

    void assign(const Value& x);

    void clear();

    Boolean typeCompatible(const Value& x) const
    {
	return _type == x._type && _isArray == x._isArray;
    }

    Boolean isArray() const { return _isArray; }

    Uint32 getArraySize() const;

    Type getType() const 
    { 
	return Type(_type); 
    }

    void setNullValue(Type type, Boolean isArray, Uint32 arraySize = 0);

    void set(Boolean x);

    void set(Uint8 x);

    void set(Sint8 x);

    void set(Uint16 x);

    void set(Sint16 x);

    void set(Uint32 x);

    void set(Sint32 x);

    void set(Uint64 x);

    void set(Sint64 x);

    void set(Real32 x);

    void set(Real64 x);

    void set(const Char16& x);

    void set(const String& x);

    void set(const char* x);

    void set(const DateTime& x);

    void set(const Reference& x);

    void set(const Array<Boolean>& x);

    void set(const Array<Uint8>& x);

    void set(const Array<Sint8>& x);

    void set(const Array<Uint16>& x);

    void set(const Array<Sint16>& x);

    void set(const Array<Uint32>& x);

    void set(const Array<Sint32>& x);

    void set(const Array<Uint64>& x);

    void set(const Array<Sint64>& x);

    void set(const Array<Real32>& x);

    void set(const Array<Real64>& x);

    void set(const Array<Char16>& x);

    void set(const Array<String>& x);

    void set(const Array<DateTime>& x);

    void get(Boolean& x) const;

    void get(Uint8& x) const;

    void get(Sint8& x) const;

    void get(Uint16& x) const;

    void get(Sint16& x) const;

    void get(Uint32& x) const;

    void get(Sint32& x) const;

    void get(Uint64& x) const;

    void get(Sint64& x) const;

    void get(Real32& x) const;

    void get(Real64& x) const;

    void get(Char16& x) const;

    void get(String& x) const;

    void get(DateTime& x) const;

    void get(Reference& x) const;

    void get(Array<Boolean>& x) const;

    void get(Array<Uint8>& x) const;

    void get(Array<Sint8>& x) const;

    void get(Array<Uint16>& x) const;

    void get(Array<Sint16>& x) const;

    void get(Array<Uint32>& x) const;

    void get(Array<Sint32>& x) const;

    void get(Array<Uint64>& x) const;

    void get(Array<Sint64>& x) const;

    void get(Array<Real32>& x) const;

    void get(Array<Real64>& x) const;

    void get(Array<Char16>& x) const;

    void get(Array<String>& x) const;

    void get(Array<DateTime>& x) const; 

    void toXml(Array<Sint8>& out) const;

    void print() const;

    String toString() const;

private:

    void _init();

    Type _type;
    Boolean _isArray;
    Union _u;

    friend class MethodRep;
    friend class ParameterRep;
    friend class PropertyRep;
    friend class QualifierRep;
    friend class QualifierDeclRep;
    __declspec(dllexport) friend Boolean operator==(
	const Value& x, 
	const Value& y);
};

__declspec(dllexport) Boolean operator==(const Value& x, const Value& y);

inline Boolean operator!=(const Value& x, const Value& y)
{
    return !operator==(x, y);
}

}

#line 303 "Value.h"
#line 43 "Headers.cpp"
#line 1 "Sharable.h"













































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 47 "Sharable.h"

namespace Pegasus {

class __declspec(dllexport) Sharable
{
public:

    Sharable() : _ref(1) { }

    virtual ~Sharable();

    Uint32 getRef() const { return _ref; }

    friend void Inc(const Sharable* sharable);

    friend void Dec(const Sharable* sharable);

private:

    Uint32 _ref;
};

inline void Inc(const Sharable* x)
{
    if (x)
	((Sharable*)x)->_ref++;
}

inline void Dec(const Sharable* x)
{
    if (x && --((Sharable*)x)->_ref == 0)
	delete (Sharable*)x;
}

}

#line 84 "Sharable.h"
#line 44 "Headers.cpp"
#line 1 "Qualifier.h"








































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 42 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Value.h"














































































































































































































































































































#line 44 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 45 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 46 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Pair.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "../../../src\\Pegasus/Common/Pair.h"

namespace Pegasus {

template<class T, class U>
struct Pair
{
    T first;
    U second;

    Pair() : first(), second()
    { 

    }

    Pair(const Pair& x) : first(x.first), second(x.second)
    { 

    }

    Pair(const T& first_, const U& second_) : first(first_), second(second_)
    {

    }

    Pair& operator=(const Pair& x)
    {
	first = x.first;
	second = x.second;
	return *this;
    }
};

}

#line 76 "../../../src\\Pegasus/Common/Pair.h"
#line 47 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 48 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 49 "Qualifier.h"
#line 1 "../../../src\\Pegasus/Common/Flavor.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "../../../src\\Pegasus/Common/Flavor.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 42 "../../../src\\Pegasus/Common/Flavor.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "../../../src\\Pegasus/Common/Flavor.h"

namespace Pegasus {

struct __declspec(dllexport) Flavor
{
    static const Uint32 NONE;
    static const Uint32 OVERRIDABLE;
    static const Uint32 TOSUBCLASS;
    static const Uint32 TOINSTANCE;
    static const Uint32 TRANSLATABLE;

    
    static const Uint32 DEFAULTS;
};

__declspec(dllexport) String FlavorToString(Uint32 scope);

__declspec(dllexport) void FlavorToXml(
    Array<Sint8>& out, 
    Uint32 flavor);

}

#line 67 "../../../src\\Pegasus/Common/Flavor.h"
#line 50 "Qualifier.h"

namespace Pegasus {







class ConstQualifier;
class Qualifier;

class __declspec(dllexport) QualifierRep : public Sharable
{
public:

    QualifierRep(
	const String& name, 
	const Value& value, 
	Uint32 flavor,
	Boolean propagated);

    virtual ~QualifierRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Type getType() const 
    { 
	return _value.getType(); 
    }

    const Value& getValue() const 
    { 
	return _value; 
    }

    void setValue(const Value& value) 
    {
	_value = value; 

	if (_value.getType() == Type::NONE)
	    throw NullType();
    }

    Uint32 getFlavor() const 
    {
	return _flavor; 
    }

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    {
	_propagated = propagated; 
    }

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const QualifierRep* x) const;

private:

    QualifierRep();

    QualifierRep(const QualifierRep& x);

    QualifierRep& operator=(const QualifierRep& x);

    String _name;
    Value _value;
    Uint32 _flavor;
    Boolean _propagated;
};







class ConstQualifier;
class ClassDeclRep;

class __declspec(dllexport) Qualifier
{
public:

    Qualifier() : _rep(0)
    {

    }

    Qualifier(const Qualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    

    Qualifier(
	const String& name, 
	const Value& value, 
	Uint32 flavor = Flavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new QualifierRep(name, value, flavor, propagated);
    }

    ~Qualifier()
    {
	Dec(_rep);
    }

    Qualifier& operator=(const Qualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    

    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    void setValue(const Value& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }

    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    const Uint32 getPropagated() const 
    {
	_checkRep();
	return _rep->getPropagated(); 
    }

    void setPropagated(Boolean propagated) 
    {
	_checkRep();
	_rep->setPropagated(propagated); 
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstQualifier& x) const;

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierRep* _rep;
    friend class ConstQualifier;
    friend class ClassDeclRep;
};







class __declspec(dllexport) ConstQualifier
{
public:

    ConstQualifier() : _rep(0)
    {

    }

    ConstQualifier(const ConstQualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    ConstQualifier(const Qualifier& x) 
    {
	Inc(_rep = x._rep); 
    }

    

    ConstQualifier(
	const String& name, 
	const Value& value, 
	Uint32 flavor = Flavor::DEFAULTS,
	Boolean propagated = false)
    {
	_rep = new QualifierRep(name, value, flavor, propagated);
    }

    ~ConstQualifier()
    {
	Dec(_rep);
    }

    ConstQualifier& operator=(const ConstQualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    ConstQualifier& operator=(const Qualifier& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    const Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    const Uint32 getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated(); 
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstQualifier& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierRep* _rep;
    friend class Qualifier;
};

}

#line 392 "Qualifier.h"
#line 45 "Headers.cpp"
#line 1 "Method.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "Method.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 42 "Method.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "Method.h"
#line 1 "../../../src\\Pegasus/Common/Qualifier.h"







































































































































































































































































































































































































#line 44 "Method.h"
#line 1 "../../../src\\Pegasus/Common/QualifierList.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "../../../src\\Pegasus/Common/QualifierList.h"
#line 1 "../../../src\\Pegasus/Common/Qualifier.h"







































































































































































































































































































































































































#line 42 "../../../src\\Pegasus/Common/QualifierList.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 43 "../../../src\\Pegasus/Common/QualifierList.h"
#line 1 "../../../src\\Pegasus/Common/Pair.h"











































































#line 44 "../../../src\\Pegasus/Common/QualifierList.h"

namespace Pegasus {

class DeclContext;

class __declspec(dllexport) QualifierList
{
public:

    QualifierList& add(const Qualifier& qualifier);

    Uint32 getCount() const { return _qualifiers.getSize(); }

    Qualifier getQualifier(Uint32 pos) { return _qualifiers[pos]; }

    ConstQualifier getQualifier(Uint32 pos) const { return _qualifiers[pos]; }

    Uint32 find(const String& name) const;

    void resolve(
	DeclContext* declContext,
	const String& nameSpace,
	Uint32 scope,
	Boolean isInstancePart,
	QualifierList& inheritedQualifiers);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const QualifierList& x) const;

private:

    Array<Qualifier> _qualifiers;
};

}

#line 84 "../../../src\\Pegasus/Common/QualifierList.h"
#line 45 "Method.h"
#line 1 "../../../src\\Pegasus/Common/Parameter.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "../../../src\\Pegasus/Common/Parameter.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 42 "../../../src\\Pegasus/Common/Parameter.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "../../../src\\Pegasus/Common/Parameter.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 44 "../../../src\\Pegasus/Common/Parameter.h"
#line 1 "../../../src\\Pegasus/Common/Qualifier.h"







































































































































































































































































































































































































#line 45 "../../../src\\Pegasus/Common/Parameter.h"
#line 1 "../../../src\\Pegasus/Common/QualifierList.h"



















































































#line 46 "../../../src\\Pegasus/Common/Parameter.h"

namespace Pegasus {







class DeclContext;
class ConstParameter;
class Parameter;

class __declspec(dllexport) ParameterRep : public Sharable
{
public:

    ParameterRep(
	const String& name, 
	Type type,
	Boolean isArray,
	Uint32 arraySize,
	const String& referenceClassName);

    ~ParameterRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Boolean isArray() const
    {
	return _isArray;
    }

    Uint32 getAraySize() const
    {
	return _arraySize;
    }

    const String& getReferenceClassName() const 
    {
	return _referenceClassName; 
    }

    const Type getType() const 
    { 
	return _type; 
    }

    void setType(Type type);

    void addQualifier(const Qualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(DeclContext* declContext, const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const ParameterRep* x) const;

private:

    ParameterRep();

    ParameterRep(const ParameterRep& x);

    ParameterRep& operator=(const ParameterRep& x);

    String _name;
    Type _type;
    Boolean _isArray;
    Uint32 _arraySize;
    String _referenceClassName;
    QualifierList _qualifiers;
};







class ConstParameter;

class __declspec(dllexport) Parameter
{
public:

    Parameter() : _rep(0)
    {

    }

    Parameter(const Parameter& x)
    {
	Inc(_rep = x._rep);
    }

    Parameter& operator=(const Parameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    

    Parameter(
	const String& name, 
	Type type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new ParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~Parameter()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    

    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    Boolean isArray() const
    {
	_checkRep();
	return _rep->isArray();
    }

    Uint32 getAraySize() const
    {
	_checkRep();
	return _rep->getAraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    void setType(Type type);

    

    Parameter& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    void resolve(DeclContext* declContext, const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstParameter& x) const;

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ParameterRep* _rep;
    friend class ConstParameter;
};







class __declspec(dllexport) ConstParameter
{
public:

    ConstParameter() : _rep(0)
    {

    }

    ConstParameter(const ConstParameter& x)
    {
	Inc(_rep = x._rep);
    }

    ConstParameter(const Parameter& x)
    {
	Inc(_rep = x._rep);
    }

    ConstParameter& operator=(const ConstParameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstParameter& operator=(const Parameter& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    

    ConstParameter(
	const String& name, 
	Type type,
	Boolean isArray = false,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY)
    {
	_rep = new ParameterRep(
	    name, type, isArray, arraySize, referenceClassName);
    }

    ~ConstParameter()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Boolean isArray() const
    {
	_checkRep();
	return _rep->isArray();
    }

    Uint32 getAraySize() const
    {
	_checkRep();
	return _rep->getAraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstParameter& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    ParameterRep* _rep;
    friend class Parameter;
};

}

#line 460 "../../../src\\Pegasus/Common/Parameter.h"
#line 46 "Method.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 47 "Method.h"
#line 1 "../../../src\\Pegasus/Common/Pair.h"











































































#line 48 "Method.h"

namespace Pegasus {







class Method;
class ConstMethod;
class DeclContext;

class __declspec(dllexport) MethodRep : public Sharable
{
public:

    MethodRep(
	const String& name, 
	Type type,
	const String& classOrigin,
	Boolean propagated);

    ~MethodRep();

    virtual const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Type getType() const 
    {
	return _type; 
    }

    void setType(Type type)
    {
	_type = type; 

	if (type == Type::NONE)
	    throw NullType();
    }

    const String& getClassOrigin() const
    {
	return _classOrigin;
    }

    void setClassOrigin(const String& classOrigin);

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    { 
	_propagated = propagated; 
    }

    void addQualifier(const Qualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void addParameter(const Parameter& x);

    Uint32 findParameter(const String& name);

    Uint32 findParameter(const String& name) const
    {
	return ((MethodRep*)this)->findParameter(name);
    }

    Parameter getParameter(Uint32 pos);

    ConstParameter getParameter(Uint32 pos) const
    {
	return ((MethodRep*)this)->getParameter(pos);
    }

    Uint32 getParameterCount() const;

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	const ConstMethod& method);

    void resolve(
	DeclContext* declContext,
	const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    virtual void print() const;

    Boolean identical(const MethodRep* x) const;

private:

    MethodRep();

    MethodRep(const MethodRep& x);

    MethodRep& operator=(const MethodRep& x);

    String _name;
    Type _type;
    String _classOrigin;
    Boolean _propagated;
    QualifierList _qualifiers;
    Array<Parameter> _parameters;

    friend class ClassDeclRep;
};







class ConstMethod;

class __declspec(dllexport) Method
{
public:

    Method() : _rep(0)
    {

    }

    Method(const Method& x)
    {
	Inc(_rep = x._rep);
    }

    Method& operator=(const Method& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    

    Method(
	const String& name, 
	Type type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new MethodRep(name, type, classOrigin, propagated);
    }

    ~Method()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    

    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    Type getType() const 
    {
	_checkRep();
	return _rep->getType();
    }

    void setType(Type type)
    { 
	_checkRep();
	_rep->setType(type);
    }

    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    void setPropagated(Boolean propagated) 
    { 
	_checkRep();
	_rep->setPropagated(propagated);
    }

    

    Method& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Method& addParameter(const Parameter& x)
    {
	_checkRep();
	_rep->addParameter(x);
	return *this;
    }

    Uint32 findParameter(const String& name)
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    Parameter getParameter(Uint32 pos)
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    ConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	const ConstMethod& method)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, method);
    }

    void resolve(
	DeclContext* declContext,
	const String& nameSpace)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstMethod& x) const;

private:

    explicit Method(const ConstMethod& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    MethodRep* _rep;
    friend class ConstMethod;
    friend class ClassDeclRep;
};







class __declspec(dllexport) ConstMethod
{
public:

    ConstMethod() : _rep(0)
    {

    }

    ConstMethod(const ConstMethod& x)
    {
	Inc(_rep = x._rep);
    }

    ConstMethod(const Method& x)
    {
	Inc(_rep = x._rep);
    }

    ConstMethod& operator=(const ConstMethod& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstMethod& operator=(const Method& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    

    ConstMethod(
	const String& name, 
	Type type,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new MethodRep(name, type, classOrigin, propagated);
    }

    ~ConstMethod()
    {
	Dec(_rep);
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType();
    }

    const String& getClassOrigin() const
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    Uint32 findParameter(const String& name) const
    {
	_checkRep();
	return _rep->findParameter(name);
    }

    ConstParameter getParameter(Uint32 pos) const
    {
	_checkRep();
	return _rep->getParameter(pos);
    }

    Uint32 getParameterCount() const
    {
	_checkRep();
	return _rep->getParameterCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstMethod& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    MethodRep* _rep;

    friend class Method;
    friend class MethodRep;
};

}

#line 567 "Method.h"
#line 46 "Headers.cpp"
#line 1 "Parameter.h"











































































































































































































































































































































































































































































#line 47 "Headers.cpp"
#line 1 "Property.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "Property.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 42 "Property.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 43 "Property.h"
#line 1 "../../../src\\Pegasus/Common/Value.h"














































































































































































































































































































#line 44 "Property.h"
#line 1 "../../../src\\Pegasus/Common/Qualifier.h"







































































































































































































































































































































































































#line 45 "Property.h"
#line 1 "../../../src\\Pegasus/Common/QualifierList.h"



















































































#line 46 "Property.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 47 "Property.h"
#line 1 "../../../src\\Pegasus/Common/Pair.h"











































































#line 48 "Property.h"

namespace Pegasus {







class ClassDeclRep;
class Property;
class ConstProperty;
class DeclContext;

class __declspec(dllexport) PropertyRep : public Sharable
{
public:

    PropertyRep(
	const String& name, 
	const Value& value,
	Uint32 arraySize,
	const String& referenceClassName,
	const String& classOrigin,
	Boolean propagated);

    ~PropertyRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    const Value& getValue() const 
    { 
	return _value; 
    }

    void setValue(const Value& value);

    Uint32 getArraySize() const
    {
	return _arraySize;
    }

    const String& getReferenceClassName() const 
    {
	return _referenceClassName; 
    }

    const String& getClassOrigin() const 
    { 
	return _classOrigin; 
    }

    void setClassOrigin(const String& classOrigin);

    Boolean getPropagated() const 
    { 
	return _propagated; 
    }

    void setPropagated(Boolean propagated) 
    { 
	_propagated = propagated; 
    }

    void addQualifier(const Qualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart,
	const ConstProperty& property);

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart);

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const PropertyRep* x) const;

private:

    PropertyRep();

    PropertyRep(const PropertyRep& x);

    PropertyRep& operator=(const PropertyRep& x);

    String _name;
    Value _value;
    Uint32 _arraySize;
    String _referenceClassName;
    String _classOrigin;
    Boolean _propagated;
    QualifierList _qualifiers;

    friend class ClassDeclRep;
};







class ConstProperty;
class InstanceDeclRep;

class __declspec(dllexport) Property
{
public:

    Property() : _rep(0)
    {

    }

    Property(const Property& x)
    {
	Inc(_rep = x._rep);
    }

    

    Property(
	const String& name, 
	const Value& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new PropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~Property()
    {
	Dec(_rep);
    }

    Property& operator=(const Property& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    

    void setName(const String& name)
    {
	_checkRep();
	_rep->setName(name);
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue();
    }

    void setValue(const Value& value) 
    {
	_checkRep();
	_rep->setValue(value);
    }

    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    const String& getClassOrigin() const 
    { 
	_checkRep();
	return _rep->getClassOrigin();
    }

    void setClassOrigin(const String& classOrigin)
    {
	_checkRep();
	_rep->setClassOrigin(classOrigin);
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    void setPropagated(Boolean propagated) 
    { 
	_checkRep();
	_rep->setPropagated(propagated);
    }

    

    Property& addQualifier(const Qualifier& x)
    {
	_checkRep();
	_rep->addQualifier(x);
	return *this;
    }

    Uint32 findQualifier(const String& name)
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    Qualifier getQualifier(Uint32 pos)
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart,
	const ConstProperty& property)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart, property);
    }

    void resolve(
	DeclContext* declContext, 
	const String& nameSpace,
	Boolean isInstancePart)
    {
	_checkRep();
	_rep->resolve(declContext, nameSpace, isInstancePart);
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstProperty& x) const;

private:

    
    

    explicit Property(const ConstProperty& x);

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    PropertyRep* _rep;
    friend class ConstProperty;
    friend class ClassDeclRep;
    friend class InstanceDeclRep;
};







class __declspec(dllexport) ConstProperty
{
public:

    ConstProperty() : _rep(0)
    {

    }

    ConstProperty(const ConstProperty& x)
    {
	Inc(_rep = x._rep);
    }

    ConstProperty(const Property& x)
    {
	Inc(_rep = x._rep);
    }

    

    ConstProperty(
	const String& name, 
	const Value& value,
	Uint32 arraySize = 0,
	const String& referenceClassName = String::EMPTY,
	const String& classOrigin = String(),
	Boolean propagated = false)
    {
	_rep = new PropertyRep(name, value,
	    arraySize, referenceClassName, classOrigin, propagated);
    }

    ~ConstProperty()
    {
	Dec(_rep);
    }

    ConstProperty& operator=(const ConstProperty& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    ConstProperty& operator=(const Property& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}
	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    const Value& getValue() const 
    {
	_checkRep();
	return _rep->getValue();
    }

    Uint32 getArraySize() const
    {
	_checkRep();
	return _rep->getArraySize();
    }

    const String& getReferenceClassName() const 
    {
	_checkRep();
	return _rep->getReferenceClassName(); 
    }

    const String& getClassOrigin() const 
    {
	_checkRep();
	return _rep->getClassOrigin();
    }

    Boolean getPropagated() const 
    { 
	_checkRep();
	return _rep->getPropagated();
    }

    Uint32 findQualifier(const String& name) const
    {
	_checkRep();
	return _rep->findQualifier(name);
    }

    ConstQualifier getQualifier(Uint32 pos) const
    {
	_checkRep();
	return _rep->getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	_checkRep();
	return _rep->getQualifierCount();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstProperty& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    PropertyRep* _rep;

    friend class Property;
    friend class PropertyRep;
};

}

#line 541 "Property.h"
#line 48 "Headers.cpp"
#line 1 "Reference.h"












































































































































































































#line 49 "Headers.cpp"
#line 1 "QualifierDecl.h"







































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 41 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/String.h"



















































































































































































































#line 42 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Value.h"














































































































































































































































































































#line 43 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 44 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Array.h"















































































































































































































































































































































































































































































































































































#line 45 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Pair.h"











































































#line 46 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Sharable.h"



















































































#line 47 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Exception.h"




























































































































































































































































































































































































































































































































































#line 48 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Flavor.h"


































































#line 49 "QualifierDecl.h"
#line 1 "../../../src\\Pegasus/Common/Scope.h"

































































#line 50 "QualifierDecl.h"

namespace Pegasus {







class ConstQualifierDecl;
class QualifierDecl;

class __declspec(dllexport) QualifierDeclRep : public Sharable
{
public:

    QualifierDeclRep(
	const String& name, 
	const Value& value, 
	Uint32 scope,
	Uint32 flavor,
	Uint32 arraySize);

    virtual ~QualifierDeclRep();

    const String& getName() const 
    { 
	return _name; 
    }

    void setName(const String& name);

    Type getType() const 
    { 
	return _value.getType(); 
    }

    const Value& getValue() const 
    { 
	return _value; 
    }

    void setValue(const Value& value) 
    {
	_value = value; 

	if (_value.getType() == Type::NONE)
	    throw NullType();
    }

    Uint32 getScope() const 
    {
	return _scope; 
    }

    Uint32 getFlavor() const 
    {
	return _flavor; 
    }

    Uint32 getArraySize() const 
    {
	return _arraySize; 
    }

    void toXml(Array<Sint8>& out) const;

    void print() const;

    Boolean identical(const QualifierDeclRep* x) const;

private:

    QualifierDeclRep();

    QualifierDeclRep(const QualifierDeclRep& x);

    QualifierDeclRep& operator=(const QualifierDeclRep& x);

    String _name;
    Value _value;
    Uint32 _scope;
    Uint32 _flavor;
    Uint32 _arraySize;
};







class ConstQualifierDecl;
class ClassDeclRep;

class __declspec(dllexport) QualifierDecl
{
public:

    QualifierDecl() : _rep(0)
    {

    }

    QualifierDecl(const QualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

    

    QualifierDecl(
	const String& name, 
	const Value& value, 
	Uint32 scope,
	Uint32 flavor = Flavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new QualifierDeclRep(name, value, scope, flavor, arraySize);
    }

    ~QualifierDecl()
    {
	Dec(_rep);
    }

    QualifierDecl& operator=(const QualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    

    void setName(const String& name) 
    { 
	_checkRep();
	_rep->setName(name); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    void setValue(const Value& value) 
    { 
	_checkRep();
	_rep->setValue(value); 
    }

    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }

    Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstQualifierDecl& x) const;

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierDeclRep* _rep;
    friend class ConstQualifierDecl;
    friend class ClassDeclRep;
};







class __declspec(dllexport) ConstQualifierDecl
{
public:

    ConstQualifierDecl() : _rep(0)
    {

    }

    ConstQualifierDecl(const ConstQualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

    ConstQualifierDecl(const QualifierDecl& x) 
    {
	Inc(_rep = x._rep); 
    }

    

    ConstQualifierDecl(
	const String& name, 
	const Value& value, 
	Uint32 scope,
	Uint32 flavor = Flavor::DEFAULTS,
	Uint32 arraySize = 0)
    {
	_rep = new QualifierDeclRep(name, value, scope, flavor, arraySize);
    }

    ~ConstQualifierDecl()
    {
	Dec(_rep);
    }

    ConstQualifierDecl& operator=(const ConstQualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    ConstQualifierDecl& operator=(const QualifierDecl& x)
    {
	if (x._rep != _rep)
	{
	    Dec(_rep);
	    Inc(_rep = x._rep);
	}

	return *this;
    }

    const String& getName() const 
    { 
	_checkRep();
	return _rep->getName(); 
    }

    Type getType() const 
    { 
	_checkRep();
	return _rep->getType(); 
    }

    const Value& getValue() const 
    { 
	_checkRep();
	return _rep->getValue(); 
    }

    Uint32 getScope() const 
    {
	_checkRep();
	return _rep->getScope();
    }

    const Uint32 getFlavor() const 
    {
	_checkRep();
	return _rep->getFlavor();
    }

    Uint32 getArraySize() const 
    {
	_checkRep();
	return _rep->getArraySize();
    }

    operator int() const { return _rep != 0; }

    void toXml(Array<Sint8>& out) const
    {
	_checkRep();
	_rep->toXml(out);
    }

    void print() const
    {
	_checkRep();
	_rep->print();
    }

    Boolean identical(const ConstQualifierDecl& x) const
    {
	x._checkRep();
	_checkRep();
	return _rep->identical(x._rep);
    }

private:

    void _checkRep() const
    {
	if (!_rep)
	    throw UnitializedHandle();
    }

    QualifierDeclRep* _rep;
    friend class QualifierDecl;
};

}

#line 402 "QualifierDecl.h"
#line 50 "Headers.cpp"
