// 
// interface definition for the cimmofRepository class, a specialiazation
// of the Pegasus Repository class with error handling.
//

#ifndef _CIMMOFREPOSITORY_H_
#define _CIMMOFREPOSITORY_H_

#include <Pegasus/Repository/Repository.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/cimmofParser.h>

using namespace Pegasus;

class PEGASUS_COMPILER_LINKAGE cimmofRepository : public Repository {
 public:
  cimmofRepository(const String &path);
  ~cimmofRepository();

  // Add some methods for use at the compiler level
  virtual int addClass(CIMClass *classdecl);
  virtual int addInstance(CIMInstance *instance);
  virtual int addQualifier(CIMQualifierDecl *qualifier);

  virtual CIMQualifierDecl getQualifierDecl(const String &name);

};

#endif
