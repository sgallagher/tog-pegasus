// Exceptions that can occur in processing the cimmof compiler command line
//

#ifndef _CMDLINEEXCEPTION_H_
#define _CMDLINEEXCEPTION_H_

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CMDLINE_LINKAGE ArgumentErrorsException : public Exception {
 public:
  static const String MSG;
  ArgumentErrorsException(const String &msg) : Exception(msg) {};
};

class PEGASUS_CMDLINE_LINKAGE CmdlineNoRepository : public Exception {
 public:
  static const String MSG;
  CmdlineNoRepository(const String &msg) : Exception(msg) {};
};

PEGASUS_NAMESPACE_END
#endif
