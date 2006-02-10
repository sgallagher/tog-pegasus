//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
/*****************************************************************************
 *  Description:   portability routines for nucleus RTOS
 *
 *  Originated: December 19, 2001 
 *	Original Author: Mike Day md@soft-hackle.net
 *                                mdday@us.ibm.com
 *
 *  $Header: /cvs/MSB/pegasus/src/slp/slp_client/src/cmd-utils/slp_client/Attic/lslp-nucleus.cpp,v 1.3.16.1 2006/02/10 16:17:21 a.dunfey Exp $ 	                                                            
 *               					                    
 *  Copyright (c) 2001 - 2003  IBM                                          
 *  Copyright (c) 2000 - 2003 Michael Day                                    
 *                                                                           
 *  Permission is hereby granted, free of charge, to any person obtaining a  
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the     
 *  Software is furnished to do so, subject to the following conditions:       
 * 
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 * 
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/




#include "lslp-nucleus.h"

unsigned int errno;


NU_HOSTENT * nucleus_gethostbyname(const char *name)         //jeb
{
  static NU_HOSTENT name_host_entry;
  
  if (NU_SUCCESS == NU_Get_Host_By_Name((char*)name, &name_host_entry))   //jeb
    return &name_host_entry;
  return NULL;
}

NU_HOSTENT * nucleus_gethostbyaddr(const void *addr, int len, int type)   //jeb
{
  static NU_HOSTENT addr_host_entry;

  if ( type != NU_FAMILY_IP || len != 4 )
    return NULL;
  if(NU_SUCCESS == NU_Get_Host_By_Addr((char *)addr, len, type, &addr_host_entry))
    return &addr_host_entry;
  return NULL;
}
            

/************************************************************************
*                                                                       
*   FUNCTION                                                              
*
*       inet_ntoa                                      
*
*   DESCRIPTION
*
*       This function transforms a UINT32 IP address into a string of
*       the form xxx.xxx.xxx.xxx.
*                                                                       
*   INPUTS                                                                
*
*       in      The data structure containing the UINT32 IP address.
*                                                                       
*   OUTPUTS                                                               
*
*       A pointer to the new string.
*                                                                       
*************************************************************************/
char *inet_ntoa(struct in_addr in)
{
    static      char    b[18];
    register    unsigned char   *p;
    CHAR        temp[3];
    INT         i, j;
    
    memset(b, 0, 18);
    p = (UINT8 *)&in;

    for (i = 0; i <= 3; i++)
    {
        if (p[i] == 0)
        {
            for (j = 0; j < 3; j++)
                strcat(b, (CHAR*)(NU_ITOA((INT)(p[j]), temp, 10)));
        }
        else    
            strcat(b, (CHAR*)(NU_ITOA((INT)(p[i]), temp, 10)));

        if (i != 3)
            strcat(b, ".");
    }

    return (b);
}

/************************************************************************
*                                                                       
*   FUNCTION                                                              
*
*       assert                                      
*                                                                       
*************************************************************************/
//#ifdef SLPDBG
//void assert(int expr)
//{
//  if (!expr)
//       _system_info(TRUE,"ASSERT:%s/%d", __FILE__, __LINE__);
////_system_info(TRUE,"ASSERT:%s/%d\n", __FILE__, __LINE__); HALT;}
//}
//#endif
