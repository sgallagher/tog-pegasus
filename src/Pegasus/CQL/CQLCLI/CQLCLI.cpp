
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>
#include <stdio.h>
#include <Pegasus/Common/String.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
FILE *CQL_in;
int CQL_parse();

int main(int argc, char ** argv)
{
	Boolean readFile = false;
	if(argc > 1){
		FILE *file;
                file = fopen(argv[1],"r");
                if(!file){
                        printf("could not open %s\n",argv[1]);
                        exit(1);
                }
                CQL_in = file;
		printf("Opened %s for reading...\n",argv[1]);
		readFile = true;
	}
        printf("Starting parser...\n");
	//if(readFile){
	 //  int good = 0;
	  /// while(good == 0){
	//	good = CQL_parse();
	  // }	
	//}else{
		while(1){
			CQL_parse();
		}
        	
	//}
    	return 0;                                                                                                              
}

