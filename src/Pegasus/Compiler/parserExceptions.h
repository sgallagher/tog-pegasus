#ifndef _ParserExceptions_h
#define _ParserExceptions_h

#include <cstring>
#include <cstdlib>

namespace ParserExceptions
{

class ParserLexException
{
public:

    ParserLexException(const char* message)
    {
	_message = strcpy(new char[strlen(message) + 1], message);
    }

    ~ParserLexException()
    {
	delete [] _message;
    }

    const char* getMessage() const { return _message; }

private:
    char* _message;
};

}

#endif /* _ParserExceptions_h */
