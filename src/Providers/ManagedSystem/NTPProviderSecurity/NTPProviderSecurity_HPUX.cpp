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
    IdentityContainer container = context.get(IdentityContainer::NAME);
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
//                                         SEC_OPT_WRITE,
//                                         SEC_OPT_READ_WRITE or
//                                         SEC_OPT_EXECUTE
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
    int ps, opt, i, j,
        ct = 0, ngr = 0;
    ushort rt, gr, ot,
           trt, tgr;
    Boolean ok = false,
            isRoot = false;
    char buffer[500];
    char *member;
    gid_t grps[100];
    String strTmp;
    String path;
    String strValue;
    Array<String> strCmd;
    Array<String> strMembers;
    
    // Retrieve uid from user
    strValue.clear();
    if(String::equalNoCase(username, "root"))
        isRoot = true;
    else
    {
       pwd = getpwent();
       if(pwd != NULL) 
       {
          strValue.assign(pwd->pw_name);
          while(!String::equalNoCase(strValue, username)) 
          {
               pwd = getpwent();
               if(pwd == NULL)
                   break;
               strValue.assign(pwd->pw_name);
           }
       }
        endpwent();
        if(strValue.size() == 0 || !String::equalNoCase(strValue, username))
            return ok;
    }
    
    // Retrieve groups from user
    strValue.clear(); 
    memset(&grps, 0, sizeof(grps));
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
        grp = getgrent();
    }
    endgrent();
    
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
    
    for(int i=0; i<strCmd.size(); i++) {
       ok = false;
       strTmp.assign(strCmd[i]);

          if(stat(strTmp.getCString(), &st) == -1)
           return ok;
        
        // Identify file permissions
        // Root
        rt = (st.st_basemode / 64);
        trt = rt * 64;
        
        // Group
        gr = (st.st_basemode - trt) / 8;
        tgr = gr * 8;

        // Other
        ot = (st.st_basemode - trt - tgr);
        
        if(isRoot) 
           ok = true;
        else
        {
            if(st.st_uid == pwd->pw_uid)
                ok = true;
            else
            {    
                 switch(opt) {
                  case 1:
                       ok = (ot >= 4);
                        if(!ok && gr >= 4) {
                            for(j=0; j<ngr; j++) {
                                if(st.st_gid == grps[j]) {
                                    ok = true;
                                    break;
                                }
                            }
                        }
                      break;
                  case 2:
                       ok = (ot == 2 || ot == 3 || 
                            ot >= 6 || gr == 2 ||
                             gr == 3);

                        if(!ok && gr >= 6) {
                            for(j=0; j<ngr; j++) {
                                if(st.st_gid == grps[j]) {
                                    ok = true;
                                    break;
                                }
                            }
                        }
                      break;
                  case 3:
                       ok = (ot >= 1 && ot % 2 != 0);
                        
                        if(!ok && gr >= 1 && gr % 2 != 0) {
                            for(j=0; j<ngr; j++) {
                                if(st.st_gid == grps[j]) {
                                    ok = true;
                                    break;
                                }
                            }
                        }
                      break;
                   default:
                       break;
               }
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
