//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Sushma Fernandes (sushma_fernandes@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Bapu Patil (bapu_patil@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//              Terry Martin, Hewlett-Packard Company (terry.martin@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1428
//              Seema Gupta (gseema@in.ibm.com) for Bug#1617
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <windows.h>
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>
#include <io.h>
#include <conio.h>
#include <direct.h>
#include <sys/types.h>
#include <windows.h>
#include <process.h>
#include <lm.h>

#define SECURITY_WIN32
#include <security.h>

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ACCESS_EXISTS 0
#define PEGASUS_ACCESS_WRITE 2
#define PEGASUS_ACCESS_READ 4
#define PEGASUS_ACCESS_READ_AND_WRITE 6

#define PW_BUFF_LEN 65

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER largeInt = { ft.dwLowDateTime, ft.dwHighDateTime };
    largeInt.QuadPart -= 0x19db1ded53e8000;
    seconds = long(largeInt.QuadPart / (10000 * 1000));
    milliseconds = long((largeInt.QuadPart % (10000 * 1000)) / 10);
    // This is a real hack. Added the following line after timevalue was
    // corrected and this apparently wrong. ks 7 apri 2002
    milliseconds = milliseconds / 1000;
}

String System::getCurrentASCIITime()
{
    char tmpbuf[128];
    _strdate( tmpbuf );
    String date = tmpbuf;
    _strtime( tmpbuf );
    date.append("-");
    date.append(tmpbuf);
    return date;
}

void System::sleep(Uint32 seconds)
{
    Sleep(seconds * 1000);
}

Boolean System::exists(const char* path)
{
    return _access(path, PEGASUS_ACCESS_EXISTS) == 0;
}

Boolean System::canRead(const char* path)
{
    return _access(path, PEGASUS_ACCESS_READ) == 0;
}

Boolean System::canWrite(const char* path)
{
    return _access(path, PEGASUS_ACCESS_WRITE) == 0;
}

Boolean System::getCurrentDirectory(char* path, Uint32 size)
{
    return GetCurrentDirectory(size, path) != 0;
}

Boolean System::isDirectory(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return false;

    return (st.st_mode & _S_IFDIR) != 0;
}

Boolean System::changeDirectory(const char* path)
{
    return chdir(path) == 0;
}

Boolean System::makeDirectory(const char* path)
{
    return _mkdir(path) == 0;
}

Boolean System::getFileSize(const char* path, Uint32& size)
{
    struct stat st;

    if (stat(path, &st) != 0)
        return false;

    size = st.st_size;
    return true;
}

Boolean System::removeDirectory(const char* path)
{
    return rmdir(path) == 0;
}

Boolean System::removeFile(const char* path)
{
    return unlink(path) == 0;
}

Boolean System::renameFile(const char* oldPath, const char* newPath)
{
    return rename(oldPath, newPath) == 0;
}

DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName)
{
    return DynamicLibraryHandle(LoadLibrary(fileName));
}

void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle)
{
    FreeLibrary(HINSTANCE(libraryHandle));
}

String System::dynamicLoadError(void) {
return String();
}

DynamicSymbolHandle System::loadDynamicSymbol(
    DynamicLibraryHandle libraryHandle,
    const char* symbolName)
{
    return DynamicSymbolHandle(GetProcAddress(
    (HINSTANCE)libraryHandle, symbolName));
}

String System::getHostName()
{
    static char hostname[PEGASUS_MAXHOSTNAMELEN];

    if (!*hostname)
        gethostname(hostname, sizeof(hostname));

    return hostname;
}

String System::getFullyQualifiedHostName ()
{
    static char FQHostName[PEGASUS_MAXHOSTNAMELEN];

    if (!*FQHostName)
    {
        String hostname = getHostName();
        struct hostent* hostEnt;

        hostEnt = gethostbyname((const char *)hostname.getCString());
        if (hostEnt == NULL)
        {
            return String::EMPTY;
        }
        strcpy(FQHostName, hostEnt->h_name);
    }

    return FQHostName;
}

String System::getSystemCreationClassName ()
{
    return "CIM_ComputerSystem";
}

Uint32 System::lookupPort(
    const char * serviceName,
    Uint32 defaultPort)
{
    Uint32 localPort;

    struct servent *serv;

    //
    // Get the port number.
    //
    if ( (serv = getservbyname(serviceName, TCP)) != NULL )
    {
        localPort = ntohs(serv->s_port);
    }
    else
    {
        localPort = defaultPort;
    }

    return localPort;
}

String System::getPassword(const char* prompt)
{
    char password[PW_BUFF_LEN] = {0};
    int num_chars = 0;
    int ch;

    fputs(prompt, stderr);

    while ((ch = _getch()) != '\r' &&
            num_chars < PW_BUFF_LEN)
        {
        // EOF
        if (ch == EOF)
        {
            fputs("[EOF]\n", stderr);
            return String::EMPTY;
        }
        // Backspace or Delete
        else if ((ch == '\b' || ch == 127) &&
                num_chars > 0)
        {
            password[--num_chars] = '\0';
            fputs("\b \b", stderr);
        }
        // CTRL+C
        else if (ch == 3)
        {
            // _getch() does not catch CTRL+C
            fputs("^C\n", stderr);
            exit(-1);
        }
        // CTRL+Z
        else if (ch == 26)
        {
            fputs("^Z\n", stderr);
            return String::EMPTY;
        }
        // Esc
        else if (ch == 27)
        {
            fputc('\n', stderr);
            fputs(prompt, stderr);
            num_chars = 0;
        }
        // Function keys (0 or E0) are a guards for a Function key codes
        else if (ch == 0 || ch == 0xE0)
        {
            ch = (ch << 4) | _getch();
            // Handle DELETE, left arrow, keypad DEL, and keypad left arrow
            if ((ch == 0xE53 || ch == 0xE4B || ch == 0x053 || ch == 0x04b) &&
                    num_chars > 0)
            {
                password[--num_chars] = '\0';
                fputs("\b \b", stderr);
            }
            else
            {
                fputc('\a', stderr);
            }
        }
        else if ((num_chars < sizeof(password) - 1) &&
                    !iscntrl(((unsigned char)(ch))))
        {
            password[num_chars++] = ch;
            fputc('*', stderr);
        }
        else
        {
            fputc('\a', stderr);
        }
    }

    fputc('\n', stderr);
    password[num_chars] = '\0';

    return String(password);
}

String System::getEffectiveUserName()
{
#if (_MSC_VER >= 1300) || defined(PEGASUS_WINDOWS_SDK_HOME)

	//Bug 3076 fix
	char fullUserName[UNLEN+1];
	DWORD userNameSize = sizeof(fullUserName);
	char computerName[MAX_COMPUTERNAME_LENGTH+1];
    DWORD computerNameSize = sizeof(computerName);    
	char userName[UNLEN+1];
    char userDomain[UNLEN+1];
	String userId;

	if (!GetUserNameEx(NameSamCompatible, fullUserName, &userNameSize))
	{
		return String();
	}

	char* index = strchr(fullUserName, '\\');
	*index = '\0';
	strcpy(userDomain, fullUserName);
	strcpy(userName, index + 1);
 
	//The above function will return the system name as the domain if
	//the user is not on a real domain.  Strip this out so that the rest of
	//our windows user functions work.  What if the system name and the domain
	//name are the same?
    GetComputerName(computerName, &computerNameSize);
		
	if (strcmp(computerName, userDomain) != 0) 
	{
		userId.append(userDomain);
		userId.append("\\");
		userId.append(userName);
	} else
	{
		userId.append(userName);
	}

	return userId;

#else //original getEffectiveUserName function
    
    int retcode = 0;

    // UNLEN (256) is the limit, not including null
    char pUserName[256+1] = {0};
    DWORD nSize = sizeof(pUserName);

    retcode = GetUserName(pUserName, &nSize);
    if (retcode == 0)
    {
        // zero is failure
        return String();
    }

    return String(pUserName);
#endif
}

String System::encryptPassword(const char* password, const char* salt)
{
    BYTE pbBuffer[PW_BUFF_LEN] = {0};
    DWORD dwByteCount;
    char pcSalt[3] = {0};

    strncpy(pcSalt, salt, 2);
    dwByteCount = strlen(password);
    memcpy(pbBuffer, password, dwByteCount);
    for (DWORD i=0; (i<dwByteCount) || (i>=PW_BUFF_LEN); i++)
            (i%2 == 0) ? pbBuffer[i] ^= pcSalt[1] : pbBuffer[i] ^= pcSalt[0];

    return String(pcSalt) + String((char *)pbBuffer);
}

Boolean System::isSystemUser(const char* userName)
{
    Boolean isSystemUser = false;

    char mUserName[UNLEN+1];
    char mDomainName[UNLEN+1];
    char tUserName[UNLEN+1];
    wchar_t wUserName[UNLEN+1];
    wchar_t wDomainName[UNLEN+1];
    char* pbs;
    bool usingDomain = false;

    LPBYTE pComputerName=NULL;
    DWORD dwLevel = 1;
    LPUSER_INFO_1 pUserInfo = NULL;
    NET_API_STATUS nStatus = NULL;

    // Make a copy of the specified username, it cannot be used directly because it's
    // declared as const and strchr() may modify the string.
    strncpy(tUserName, userName, sizeof(tUserName) - 1);
    tUserName[sizeof(tUserName)- 1] = '\0';

    //separate the domain and user name if both are present.
    if (NULL != (pbs = strchr(tUserName, '\\')))
    {
        *pbs = '\0';
        strcpy(mDomainName, tUserName);
        strcpy(mUserName, pbs+1);
        usingDomain = true;

    } else if ((NULL != (pbs = (strchr(tUserName, '@')))) ||
               (NULL != (pbs = (strchr(tUserName, '.')))))
    {
        *pbs = '\0';
        strcpy(mDomainName, pbs+1);
        strcpy(mUserName, tUserName);
        usingDomain = true;

    } else
    {
        strcpy(mDomainName, ".");
        strcpy(mUserName, tUserName);
    }

    //convert domain name to unicode
    if (!MultiByteToWideChar(CP_ACP, 0, mDomainName, -1, wDomainName, strlen(mDomainName)+1))
    {
        return false;
    }

    //convert username to unicode
    if (!MultiByteToWideChar(CP_ACP, 0, mUserName, -1, wUserName, strlen(mUserName)+1))
    {
        return false;
    }

    if (usingDomain)
    {
        //get domain controller
        DWORD rc = NetGetDCName(NULL, wDomainName, &pComputerName);
        if (rc == NERR_Success)
        {
            wcscpy(wDomainName, (LPWSTR) pComputerName); //this is automatically prefixed with "\\"
        }
        /*
        else
        {
            // failover
            // ATTN: This is commented out until there is resolution on Bugzilla 2236. -hns 2/2005
            // This needs to be more thoroughly tested when we uncomment it out.

            PDOMAIN_CONTROLLER_INFO DomainControllerInfo = NULL;

            //this function does not take wide strings
            rc = DsGetDcName(NULL,
                             mDomainName,
                             NULL,
                             NULL,
                             DS_DIRECTORY_SERVICE_REQUIRED,  //not sure what flags we want here
                             &DomainControllerInfo);

            if (rc == ERROR_SUCCESS && DomainControllerInfo)
            {
                strcpy(mDomainName, DomainControllerInfo->DomainName);
                NetApiBufferFree(DomainControllerInfo);

                if (!MultiByteToWideChar(CP_ACP, 0, mDomainName, -1, wDomainName, strlen(mDomainName)+1))
                {
                    return false;
                }
            }
        }
        */
    }

    //get user info
    nStatus = NetUserGetInfo(wDomainName,
                             wUserName,
                             dwLevel,
                             (LPBYTE *)&pUserInfo);

    if (nStatus == NERR_Success)
    {
        isSystemUser = true;
    }

    if (pComputerName != NULL)
    {
        NetApiBufferFree(pComputerName);
    }

    if (pUserInfo != NULL)
    {
        NetApiBufferFree(pUserInfo);
    }

    return isSystemUser;
}


Boolean System::isPrivilegedUser(const String& userName)
{
    Boolean isPrivileged = false;

    char mUserName[UNLEN+1];
    char mDomainName[UNLEN+1];
    wchar_t wUserName[UNLEN+1];
    wchar_t wDomainName[UNLEN+1];
    char* pbs;
    char userStr[UNLEN+1];
    bool usingDomain = false;

    LPBYTE pComputerName=NULL;
    DWORD dwLevel = 1;
    LPUSER_INFO_1 pUserInfo = NULL;
    NET_API_STATUS nStatus = NULL;

    //get the username in the correct format
    strcpy(userStr, (const char*)userName.getCString());

    //separate the domain and user name if both are present.
    if (NULL != (pbs = strchr(userStr, '\\')))
    {
        *pbs = '\0';
        strcpy(mDomainName, userStr);
        strcpy(mUserName, pbs+1);
        usingDomain = true;

    } else if ((NULL != (pbs = (strchr(userStr, '@')))) ||
               (NULL != (pbs = (strchr(userStr, '.')))))
    {
        *pbs = '\0';
        strcpy(mDomainName, pbs+1);
        strcpy(mUserName, userStr);
        usingDomain = true;

    } else
    {
        strcpy(mDomainName, ".");
        strcpy(mUserName, userStr);
    }

    //convert domain name to unicode
    if (!MultiByteToWideChar(CP_ACP, 0, mDomainName, -1, wDomainName, strlen(mDomainName)+1))
    {
        return false;
    }

    //convert username to unicode
    if (!MultiByteToWideChar(CP_ACP, 0, mUserName, -1, wUserName, strlen(mUserName)+1))
    {
        return false;
    }

    if (usingDomain)
    {
        //get domain controller
        DWORD rc = NetGetDCName(NULL, wDomainName, &pComputerName);
        if (rc == NERR_Success)
        {
            wcscpy(wDomainName, (LPWSTR) pComputerName); //this is automatically prefixed with "\\"
        }
        /*
        else
        {
            // failover
            // ATTN: This is commented out until there is resolution on Bugzilla 2236. -hns 2/2005
            // This needs to be more thoroughly tested when we uncomment it out.

            PDOMAIN_CONTROLLER_INFO DomainControllerInfo = NULL;

            //this function does not take wide strings
            rc = DsGetDcName(NULL,
                             mDomainName,
                             NULL,
                             NULL,
                             DS_DIRECTORY_SERVICE_REQUIRED,  //not sure what flags we want here
                             &DomainControllerInfo);

            if (rc == ERROR_SUCCESS && DomainControllerInfo)
            {
                strcpy(mDomainName, DomainControllerInfo->DomainName);
                NetApiBufferFree(DomainControllerInfo);

                if (!MultiByteToWideChar(CP_ACP, 0, mDomainName, -1, wDomainName, strlen(mDomainName)+1))
                {
                    return false;
                }
            }
        }
        */
    }

    //get privileges
    nStatus = NetUserGetInfo(wDomainName,
                             wUserName,
                             dwLevel,
                             (LPBYTE *)&pUserInfo);

    if ((nStatus == NERR_Success) &&
        (pUserInfo != NULL) &&
        (pUserInfo->usri1_priv == USER_PRIV_ADMIN))
    {
        isPrivileged = true;
    }

    if (pComputerName != NULL)
    {
        NetApiBufferFree(pComputerName);
    }

    if (pUserInfo != NULL)
    {
        NetApiBufferFree(pUserInfo);
    }

    return isPrivileged;
}

String System::getPrivilegedUserName()
{
    // ATTN-NB-03-20000304: Implement better way to get the privileged
    // user on the system.

    return (String("Administrator"));
}

Boolean System::isGroupMember(const char* userName, const char* groupName)
{
    Boolean retVal = false;

    LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
    DWORD dwLevel = 0;
    DWORD dwFlags = LG_INCLUDE_INDIRECT ;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    NET_API_STATUS nStatus;


    //
    // Call the NetUserGetLocalGroups function
    // specifying information level 0.
    //
    // The LG_INCLUDE_INDIRECT flag specifies that the
    // function should also return the names of the local
    // groups in which the user is indirectly a member.
    //
    nStatus = NetUserGetLocalGroups(NULL,
                                   (LPCWSTR)userName,
                                   dwLevel,
                                   dwFlags,
                                   (LPBYTE *) &pBuf,
                                   dwPrefMaxLen,
                                   &dwEntriesRead,
                                   &dwTotalEntries);

    //
    // If the call succeeds,
    //
    if (nStatus == NERR_Success)
    {
        LPLOCALGROUP_USERS_INFO_0 pTmpBuf;
        DWORD i;
        DWORD dwTotalCount = 0;

        if ((pTmpBuf = pBuf) != NULL)
        {
            //
            // Loop through the local groups that the user belongs
            // and find the matching group name.
            //
            for (i = 0; i < dwEntriesRead; i++)
            {
                //
                // Compare the user's group name to groupName.
                //
                if ( strcmp ((char *)pTmpBuf->lgrui0_name, groupName) == 0 )
                {
                    // User is a member of the group.
                    retVal = true;
                    break;
                }

                pTmpBuf++;
                dwTotalCount++;
            }
        }
    }

    //
    // Free the allocated memory.
    //
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);

    //
    // If the given user and group are not found in the local group
    // then try on the global groups.
    //
    if (!retVal)
    {
        LPGROUP_USERS_INFO_0 pBuf = NULL;
        dwLevel = 0;
        dwPrefMaxLen = MAX_PREFERRED_LENGTH;
        dwEntriesRead = 0;
        dwTotalEntries = 0;

        //
        // Call the NetUserGetGroups function, specifying level 0.
        //
        nStatus = NetUserGetGroups(NULL,
                                  (LPCWSTR)userName,
                                  dwLevel,
                                  (LPBYTE*)&pBuf,
                                  dwPrefMaxLen,
                                  &dwEntriesRead,
                                  &dwTotalEntries);
        //
        // If the call succeeds,
        //
        if (nStatus == NERR_Success)
        {
            LPGROUP_USERS_INFO_0 pTmpBuf;
            DWORD i;
            DWORD dwTotalCount = 0;

            if ((pTmpBuf = pBuf) != NULL)
            {
                //
                // Loop through the global groups to which the user belongs
                // and find the matching group name.
                //
                for (i = 0; i < dwEntriesRead; i++)
                {
                    //
                    // Compare the user's group name to groupName.
                    //
                    if ( strcmp ((char *)pTmpBuf->grui0_name, groupName) == 0 )
                    {
                        // User is a member of the group.
                        retVal = true;
                        break;
                    }

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }

        //
        // Free the allocated buffer.
        //
        if (pBuf != NULL)
            NetApiBufferFree(pBuf);
    }

    return retVal;
}

Boolean System::changeUserContext(const char* userName)
{
    // ATTN: Implement this method to change the process user context to the
    //       specified user
    return false;
}

Uint32 System::getPID()
{
    return _getpid();
}

Boolean System::truncateFile(
    const char* path,
    size_t newSize)
{

    Boolean rv = false;
    int fd = open(path, O_RDWR);
    if (fd != -1)
    {
        if (chsize(fd, newSize) == 0)
        {
            rv = true;
        }

        close(fd);
    }

    return rv;
}

// Is absolute path?
Boolean System::is_absolute_path(const char *path)
{
    char full[_MAX_PATH];
    char path_slash[_MAX_PATH];
    char *p;

    strncpy(path_slash, path, _MAX_PATH);
    path_slash[_MAX_PATH-1] = '\0';

    for(p = path_slash; p < path_slash + strlen(path_slash); p++)
      if (*p == '/')
          *p = '\\';

    return (strcasecmp(_fullpath( full, path_slash, _MAX_PATH ), path_slash) == 0) ? true : false;
}

// Changes file permissions on the given file.
Boolean System::changeFilePermissions(const char* path, mode_t mode)
{
    // ATTN: File permissions are not currently defined in Windows
    return true;
}

Boolean System::verifyFileOwnership(const char* path)
{
    // ATTN: Implement this to check that the owner of the specified file is
    //       the same as the effective user for this process.
    return true;
}

void System::syslog(const String& ident, Uint32 severity, const char* message)
{
    // Not implemented
}

// System ID constants for Logger::put and Logger::trace
const String System::CIMSERVER = "cimserver";  // Server system ID

PEGASUS_NAMESPACE_END
