#include <cstdlib>
#include <iostream>
#include <cassert>
                                                                                                                                       
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/CQL/CIMOMHandleQueryContext.h>
#include <Pegasus/CQL/RepositoryQueryContext.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Repository/CIMRepository.h>
                                                                                                                     
PEGASUS_USING_PEGASUS;
                                                                                                                                       
PEGASUS_USING_STD;

void drive_CIMOMHandleQueryContext(){
	CIMNamespaceName _ns("root/cimv2");
	CIMOMHandle _ch;
	CIMOMHandleQueryContext _query(_ns,_ch);
	cout << "Host name = " << _query.getHost(false) << endl;
	Array<String> alias;
	Array<CQLIdentifier> classes;
	alias.append("A");
	alias.append("B");
	alias.append("C");
	classes.append(CQLIdentifier("APPLE"));
	classes.append(CQLIdentifier("BONGO"));
	classes.append(CQLIdentifier("CLAVE"));
	for(Uint32 i = 0; i < alias.size(); i++){
		_query.insertClass(classes[i],alias[i]);
	}

	Array<CQLIdentifier> fromList = _query.getFromList();
	for(Uint32 i = 0; i < fromList.size(); i++)
		cout << fromList[i].toString() << endl;

	cout << "CIMOMHandleQueryContext::GetClass(CIM_Process)..." << endl;
	CIMClass _class = _query.getClass(CIMName("CIM_Process"));
	CIMName _name = _class.getClassName();
	cout << "_name.getString() = " << _name.getString() << endl;
	
}

void drive_RepositoryQueryContext(){
	const char* env = getenv("PEGASUS_HOME");
	String repositoryDir(env);
	repositoryDir.append("/repository");
	cout << "using rep dir = " << repositoryDir << endl;	
	CIMNamespaceName _ns("root/cimv2");
	CIMRepository *_rep = new CIMRepository(repositoryDir);
	RepositoryQueryContext _query(_ns, _rep);
	cout << "Host name = " << _query.getHost(false) << endl;
        Array<String> alias;
        Array<CQLIdentifier> classes;
        alias.append("A");
        alias.append("B");
        alias.append("C");
        classes.append(CQLIdentifier("APPLE"));
        classes.append(CQLIdentifier("BONGO"));
        classes.append(CQLIdentifier("CLAVE"));
        for(Uint32 i = 0; i < alias.size(); i++){
                _query.insertClass(classes[i],alias[i]);
        }
        
	CQLIdentifier _id_Alias = _query.findClass(String("A"));
	CQLIdentifier _id_Class = _query.findClass(String("APPLE"));
	CQLIdentifier _id_Error = _query.findClass(String("AP"));
	cout << "_id_Alias = " << _id_Alias.getName().getString() << endl;
	cout << "_id_Class = " << _id_Class.getName().getString() << endl;
	cout << "_id_Error = " << _id_Error.getName().getString() << endl;
                                                                                                                               
        Array<CQLIdentifier> fromList = _query.getFromList();
        for(Uint32 i = 0; i < fromList.size(); i++)
                cout << fromList[i].toString() << endl;

	cout << "RepositoryQueryContext::GetClass(CIM_Process)..." << endl;
        CIMClass _class = _query.getClass(CIMName("CIM_ComputerSystem"));
        CIMName _name = _class.getClassName();
        cout << "_name.getString() = " << _name.getString() << endl;
}

int main( int argc, char *argv[] ){

        //BEGIN TESTS....

	drive_CIMOMHandleQueryContext();
	drive_RepositoryQueryContext();

	//END TESTS....
	                                                                                                                   
        cout << argv[0] << " +++++ passed all tests" << endl;
                                                                                                                                       
        return 0;
}

