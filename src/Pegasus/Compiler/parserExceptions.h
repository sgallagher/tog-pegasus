// Exceptions that can occur in processing the cimmof compiler command line
//

#ifndef _PARSEREXCEPTION_H_
#define _PARSEREXCEPTION_H_

// NOTE:  This exception does not use the Exception class from
// Pegasus/Common because it has to do with the base class parser,
// which should be reusable outside Pegasus.

#include <Pegasus/Common/String.h>

namespace ParserExceptions {

class PEGASUS_COMPILER_LINKAGE ParserException {
 private:
  const String _msg;
 public:
  ParserException(const char *msg) : _msg(msg) {} ;
  ParserException(const String &msg) :  _msg(msg) {};
  const String &getMessage() const { return _msg; };
};

class PEGASUS_COMPILER_LINKAGE ParserLexException : public ParserException  {
 public:
  ParserLexException(const char *msg) : ParserException(msg) {};
  ParserLexException(const String &msg) : ParserException(msg) {};
  ~ParserLexException() {};
};

}

#endif
