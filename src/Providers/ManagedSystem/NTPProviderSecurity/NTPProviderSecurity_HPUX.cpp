//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//Pegasus includes
#include "NTPProviderSecurity.h"

// Security includes
#include <sys/getaccess.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

//------------------------------------------------------------------------------ 
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//==============================================================================
//
// Class [Security] methods
//
//==============================================================================

//------------------------------------------------------------------------------
// Constructor to set context
//------------------------------------------------------------------------------
SecurityProvider::SecurityProvider(const OperationContext & context)
{
    IdentityContainer container(context.get(IdentityContainer::NAME));
    secUsername.assign(container.getUserName());
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
SecurityProvider::~SecurityProvider(void)
{
	delete this;    
}    

//------------------------------------------------------------------------------
// FUNCTION: checkAccess
//
// REMARKS: Status of context user
//
// PARAMETERS:    [IN]  username  -> user to retrieve information
//                [IN]  filename  -> file name to verify access    
//                [IN]  chkoper   -> valid options: SEC_OPT_READ,
//											  		SEC_OPT_WRITE,
//											  		SEC_OPT_READ_WRITE or
//											  		SEC_OPT_EXECUTE
//
// RETURN: TRUE, if user have privileges, otherwise FALSE
//------------------------------------------------------------------------------
Boolean
SecurityProvider::checkAccess(const String username, 
                              const String filename,
                              const String chkoper) 
{
    FILE *fp;
    struct passwd *pwd;
    struct group *grp;
    struct stat st;
    int ps,	opt, i, j,
    	ct = 0, ngr = 0;
    ushort rt, gr, ot,
    	   trt, tgr;
    Boolean ok = false,
    		isRoot = false,
   		    okUser = (username.size() > 0);
    char buffer[500];
    char *member;
    gid_t grps[100];
    // store user id
	uid_t user_id; 
	// store group id  - is there only one group id?
	gid_t group_id;
    int accessrights;
    String strTmp;
    String path;
    String strValue;
    Array<String> strCmd;
    Array<String> strMembers;
    
    if(okUser) {
	    // Retrieve uid from user
	    strValue.clear();
	    if(String::equalNoCase(username, "root"))
	        isRoot = true;
	    else
	    {
            // Go through password entries and find the entry that matches "username"
		    pwd = getpwent();
		    if(pwd != NULL) {
		    	strValue.assign(pwd->pw_name);
		    	while(!String::equalNoCase(strValue, username)) {
		            pwd = getpwent();
		            if(pwd == NULL)
		                break;
		            strValue.assign(pwd->pw_name);
		        }
		    }
			// indicate that the processing of the password database is complete
	        endpwent();

	        // If we didn't find the entry - just return
	        if(strValue.size() == 0 || !String::equalNoCase(strValue, username))
	            return ok;

			// DLH set the group and user id
			user_id = pwd->pw_uid;
			group_id = pwd->pw_gid;
	    }
    
	    // Find the groups to which this user belongs and store the list in "member"
		strValue.clear(); 
	    memset(&grps, 0, sizeof(grps));
	    // Return a pointer to the first group structure in the group database
	    grp = getgrent();
	    while(grp != NULL) {
			i = 0;
	    	strMembers.clear();
			member = grp->gr_mem[i++];
			while (member) {
	        	strMembers.append(member);
	        	member = grp->gr_mem[i++];
	        }
	        for(i=0; i < strMembers.size(); i++) {
	        	strValue.assign(strMembers[i]);
	        	ps = strValue.find(username);
	        	if(ps >= 0) {
	            	grps[ngr++] = grp->gr_gid;
	            	break;
	            }
	        }
            // Get the next group structure
	        grp = getgrent();
	    }
        // Indicate that the processing of the group database is complete
	    endgrent();
    }
    
	// Build the command with path of file
    strCmd.clear();
    ps = filename.reverseFind('/');
    if(ps > 0) {
	    path.assign(filename.subString(0, ps));
	    strCmd.append(path);
    }
    
    // Build the command to retrieve user informations
    strCmd.append(filename);

    //
    // Identify the type test
    //    
    opt = 0;
    if(String::equalNoCase(chkoper, SEC_OPT_READ) ||
       String::equalNoCase(chkoper, SEC_OPT_READ_WRITE))
        opt = 1;
    else if(String::equalNoCase(chkoper, SEC_OPT_WRITE) ||
            String::equalNoCase(chkoper, SEC_OPT_READ_WRITE)) 
        opt = 2;
    else if(String::equalNoCase(chkoper, SEC_OPT_EXECUTE) ||
           String::equalNoCase(chkoper, SEC_OPT_ALL))
        opt = 3;
    
    // Verify permissions from directory and file name
    for(int i=0; i<strCmd.size(); i++) {
    	ok = false;
    	strTmp.assign(strCmd[i]);

       	// The stat call gets information about the file access permissions
       	if(stat(strTmp.getCString(), &st) == -1)
        	return ok;

		// Return ok, if is invalid user_id and other permission or is root
		if(!okUser && st.st_basemode & 0x04 || isRoot)
            ok = true;
		else if(user_id > 0) 
        {            
			// Use getaccess to check permission instead of stat so that we get consistent response from OS
			accessrights = getaccess( strTmp.getCString(), user_id, ngr, grps,(void *) 0,(void *) 0);
        	if ( accessrights == -1)
			// if error - just return with ok set to false
				return ok;
        
	        // Verify status by type test
	        switch(opt) {
	            case 1:
	                if(accessrights & R_OK)
	                	ok = true;
	            	break;
	            case 2:
	                if(accessrights & W_OK)
	                	ok = true;
	            	break;
	            case 3:
	                if(accessrights & X_OK)
	                	ok = true;
	            	break;
	            default:
	                break;
	        }
        }
        if(!ok)
            break;
    }
    return ok;    
}

//------------------------------------------------------------------------------
// FUNCTION: getUserContext
//
// REMARKS: Retrieves the context user name
//
// PARAMETERS: 
//
// RETURN: string that will contain the context user
//------------------------------------------------------------------------------
String
SecurityProvider::getUserContext(void) 
{    
    return secUsername;
}
