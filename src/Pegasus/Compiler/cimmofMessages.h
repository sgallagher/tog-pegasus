//
// cimmofMessages:  an interface that hides the underlying
// internationalization implementation for the CIM MOF compiler.
// Any strings that the compiler needs come out of this interface.
// When we settle on an MSB internationalization method, the
// strings will be translated for the locale.

#include <Pegasus/Common/String.h>
#include <vector>

PEGASUS_NAMESPACE_BEGIN
using namespace std;

class cimmofMessages {
 public:
  enum MsgCode {
    SUCCESS = 0,
    PARSER_SYNTAX_ERROR,
    APPLY_PARAMETER_ERROR,
    NEW_PARAMETER_ERROR,
    UNINITIALIZED_PARAMETER_ERROR,
    METHOD_ALREADY_EXISTS_WARNING,
    APPLY_METHOD_ERROR,
    NEW_METHOD_ERROR,
    ADD_QUALIFIER,
    TRACE,
    ADD_QUALIFIER_DECL_ERROR,
    TAB_OK,
    NEW_QUALIFIER_ERROR,
    ADD_QUALIFIER_ERROR,
    NEW_QUALIFIER_DECLARATION_ERROR,
    ADD_INSTANCE,
    UNINITIALIZED_PROPERTY_ERROR,
    PROPERTY_ALREADY_EXISTS_WARNING,
    APPLYING_PROPERTY_ERROR,
    NEW_PROPERTY_ERROR,
    NEW_CLASS_ERROR,
    ADD_CLASS,
    CLASS_EXISTS_WARNING,
    ADD_CLASS_ERROR,
    SETREPOSITORY_NO_COMPILER_OPTIONS,
    SETREPOSITORY_BLANK_NAME,
    NAMESPACE_CREATE_ERROR,
    REPOSITORY_CREATE_ERROR,
    NEW_REFERENCE_ERROR,
    FIND_CLASS_OF_INSTANCE_ERROR,
    FIND_PROPERTY_VALUE_ERROR,
    CLONING_PROPERTY_ERROR,
    APPLY_INSTANCE_PROPERTY_ERROR,
    GET_INSTANCE_PROPERTY_ERROR,
    GET_CLASS_PROPERTY_ERROR,
    GET_PROPERTY_VALUE_ERROR,
    END
  };
  typedef vector<String> arglist;
  static const arglist EMPTYLIST;
  static const String &msgCodeToString(MsgCode code);
  static void          getMessage(String &out, MsgCode code, 
				  const arglist &al = 
				  cimmofMessages::EMPTYLIST);
};

PEGASUS_NAMESPACE_END
