#ifndef Pegasus_QueryContext_h
#define Pegasus_QueryContext_h
                                                                                                                           
                                                                                                                           
#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<String, CQLIdentifier, EqualFunc<String>, HashFunc<String> > HT_Alias_Class;

class PEGASUS_CQL_LINKAGE QueryContext
{
   public:

        QueryContext(CIMNamespaceName inNS);

	virtual ~QueryContext(){}

        virtual CIMClass getClass(CIMName inClassName) = 0;

        String getHost(Boolean fullyQualified = true);

        CIMNamespaceName getNamespace()const;

        void insertClass(const CQLIdentifier& inIdentifier, String inAlias = String::EMPTY);

        const CQLIdentifier findClass(String inAlias);

        Array<CQLIdentifier> getFromList()const;

   protected:
	 QueryContext(){}
          
   private: 

        // members
        CIMNamespaceName _NS;
	HT_Alias_Class _AliasClassTable;
	Array<CQLIdentifier> _fromList;
};

PEGASUS_NAMESPACE_END
#endif
