#ifndef _Pegasus_StringInline_h
#define _Pegasus_StringInline_h

#include <Pegasus/Common/Atomic.h>

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Compile-time macros:
//
//     PEGASUS_STRING_ENABLE_ICU -- enables use of ICU package.
//
//     PEGASUS_STRING_NO_THROW -- don't throw exceptions caused by bounds 
//     errors and null-pointers.
//      
//     PEGASUS_STRING_NO_UTF8 -- don't generate slower UTF8 code.
//
//     PEGASUS_STRING_INLINE -- either "inline" or empty.
//
//==============================================================================

PEGASUS_COMMON_LINKAGE void String_throw_out_of_bounds();

#ifdef PEGASUS_STRING_NO_THROW
# define _check_bounds(ARG1, ARG2) /* empty */
#else
inline void _check_bounds(size_t index, size_t size)
{
    if (index > size)
	String_throw_out_of_bounds();
}
#endif

struct StringRep
{
    StringRep();

    ~StringRep();

    static StringRep* alloc(size_t cap);

    static void free(StringRep* rep);

    static StringRep* create(const Uint16* data, size_t size);

    static StringRep* create(const char* data, size_t size);

    static StringRep* copy_on_write(StringRep* rep);

    static Uint32 length(const Uint16* str);

    static void ref(const StringRep* rep);

    static void unref(const StringRep* rep);

    static StringRep _empty_rep;

    size_t size;
    size_t cap;
    Atomic refs;
    Uint16 data[1];
};

inline void StringRep::free(StringRep* rep)
{
    Atomic_destroy(&rep->refs);
    ::operator delete(rep);
}

inline StringRep::StringRep() : size(0), cap(0)
{
    // Only called on _empty_rep.
    Atomic_create(&refs, 99);
    data[0] = 0;
}

inline StringRep::~StringRep()
{
    // Only called on _empty_rep.
    Atomic_destroy(&refs);
}

PEGASUS_STRING_INLINE void StringRep::ref(const StringRep* rep)
{
    if (rep != &StringRep::_empty_rep)
	Atomic_inc(&((StringRep*)rep)->refs);
}

PEGASUS_STRING_INLINE void StringRep::unref(const StringRep* rep)
{
    if (rep != &StringRep::_empty_rep && 
	Atomic_dec_and_test(&((StringRep*)rep)->refs))
	StringRep::free((StringRep*)rep);
}

PEGASUS_STRING_INLINE CString::CString() : _rep(0)
{
}

PEGASUS_STRING_INLINE CString::CString(char* cstr) : _rep(cstr)
{
}

PEGASUS_STRING_INLINE CString::~CString()
{
    operator delete(_rep);
}

PEGASUS_STRING_INLINE CString::operator const char*() const
{
    return _rep;
}

PEGASUS_STRING_INLINE String::String()
{
    _rep = &StringRep::_empty_rep;
}

PEGASUS_STRING_INLINE String::String(const String& str)
{
    StringRep::ref(_rep = str._rep);
} 

PEGASUS_STRING_INLINE String::~String()
{
    StringRep::unref(_rep);
}

PEGASUS_STRING_INLINE Uint32 String::size() const 
{ 
    return _rep->size; 
}

PEGASUS_STRING_INLINE const Char16* String::getChar16Data() const 
{ 
    return (Char16*)_rep->data; 
}

PEGASUS_STRING_INLINE Char16& String::operator[](Uint32 i) 
{
    _check_bounds(i, _rep->size);

    if (Atomic_get(&_rep->refs) != 1)
	_rep = StringRep::copy_on_write(_rep);

    return (Char16&)_rep->data[i]; 
}

PEGASUS_STRING_INLINE const Char16 String::operator[](Uint32 i) const 
{
    _check_bounds(i, _rep->size);
    return (Char16&)_rep->data[i]; 
}

PEGASUS_STRING_INLINE String& String::operator=(const String& str)
{
    return assign(str);
}

PEGASUS_STRING_INLINE String& String::operator=(const char* str)
{
    return assign(str);
}

PEGASUS_STRING_INLINE String& String::assign(const Char16* str)
{
    return assign(str, StringRep::length((Uint16*)str));
    return *this;
}

PEGASUS_STRING_INLINE String& String::assign(const char* str)
{
    return assign(str, strlen(str));
}

PEGASUS_STRING_INLINE Uint32 String::find(const String& s) const
{
    return _find_aux((Char16*)s._rep->data, s._rep->size);
}

PEGASUS_STRING_INLINE String& String::append(const Char16& c)
{
    if (_rep->size == _rep->cap || Atomic_get(&_rep->refs) != 1)
	_append_char_aux();

    _rep->data[_rep->size++] = c;
    _rep->data[_rep->size] = 0;
    return *this;
}

PEGASUS_STRING_INLINE Boolean String::equalNoCase(
    const String& s1, const String& s2)
{
    if (s1._rep->size == s2._rep->size)
	return equalNoCase_aux(s1, s2);

    return false;
}

PEGASUS_STRING_INLINE String& String::append(const char* str)
{
    append(str, strlen(str));
    return *this;
}

PEGASUS_STRING_INLINE String& String::append(char c) 
{
    return append(Char16(c)); 
}

PEGASUS_STRING_INLINE Uint32 String::find(char c) const 
{
    return find(Char16(c)); 
}

PEGASUS_STRING_INLINE Boolean operator==(const String& s1, const String& s2)
{
    return String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator==(const String& s1, const char* s2)
{
    return String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator==(const char* s1, const String& s2)
{
    return String::equal(s2, s1);
}

PEGASUS_STRING_INLINE Boolean operator!=(const String& s1, const String& s2)
{
    return !String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator!=(const String& s1, const char* s2)
{
    return !String::equal(s1, s2);
}

PEGASUS_STRING_INLINE Boolean operator!=(const char* s1, const String& s2)
{
    return !String::equal(s2, s1);
}

PEGASUS_STRING_INLINE Boolean operator<(const String& s1, const String& s2)
{
    return String::compare(s1, s2) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<(const String& s1, const char* s2)
{
    return String::compare(s1, s2) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<(const char* s1, const String& s2)
{
    return String::compare(s2, s1) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const String& s1, const String& s2)
{
    return String::compare(s1, s2) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const String& s1, const char* s2)
{
    return String::compare(s1, s2) > 0;
}

PEGASUS_STRING_INLINE Boolean operator>(const char* s1, const String& s2)
{
    return String::compare(s2, s1) < 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const String& s1, const String& s2)
{
    return String::compare(s1, s2) <= 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const String& s1, const char* s2)
{
    return String::compare(s1, s2) <= 0;
}

PEGASUS_STRING_INLINE Boolean operator<=(const char* s1, const String& s2)
{
    return String::compare(s2, s1) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const String& s1, const String& s2)
{
    return String::compare(s1, s2) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const String& s1, const char* s2)
{
    return String::compare(s1, s2) >= 0;
}

PEGASUS_STRING_INLINE Boolean operator>=(const char* s1, const String& s2)
{
    return String::compare(s2, s1) <= 0;
}

PEGASUS_STRING_INLINE String operator+(const String& s1, const String& s2)
{
    return String(s1, s2);
}

PEGASUS_STRING_INLINE String operator+(const String& s1, const char* s2)
{
    return String(s1, s2);
}

PEGASUS_STRING_INLINE String operator+(const char* s1, const String& s2)
{
    return String(s1, s2);
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_StringInline_h */
