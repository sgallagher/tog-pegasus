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
//
//%/////////////////////////////////////////////////////////////////////////////

#define SLP_LIB_IMPORT
#include <Pegasus/Common/PegasusAssert.h>
#include "../slp_client/slp_client.h"
#include <Pegasus/Client/CIMClient.h>
#define SLP_PORT 427
#define MAX_LIFE PEGASUS_SLP_REG_TIMEOUT * 60
#define LOCALHOST_IP "127.0.0.1"
#include <Pegasus/Server/SLPAttrib.h>
int find(char *,char *);
char* replac(char *,char *);
BOOL  parseFind1(lslpMsg* ,  char * );

using namespace Pegasus;
static char *predicate;
static BOOL parsable= TRUE;
static char fs='\t', rs='\n';

    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    char *httpUrl2 = (char *)NULL;
    char *httpAttrs2  = (char *)NULL;
    int no_of_regs=0;
BOOL parseFind1(lslpMsg *temp,  char* httpAttr)
{
    BOOL  found = false;
    lslpURL    *url_list;
    if (temp != NULL && temp->type == srvRply)
    {
        if ((NULL != temp->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
        {/*start of url if*/
                url_list = temp->msg.srvRply.urlList->next;
                while (! _LSLP_IS_HEAD(url_list) && !found)
                {/*start of url while */
                    if (NULL != url_list->attrs &&
                        ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                        lslpAtomList *attrs = url_list->attrs->next;
                        while (! _LSLP_IS_HEAD (attrs) && attrs->str && strlen (attrs->str) && !found)
                        { //while traversing attr list
                            if (!String::compare (attrs->str,httpAttr))
                            {   
                                found = true;
                                break;
                            }
                            attrs = attrs->next;
                        }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                        printf("%c", rs);
                    }
                    url_list = url_list->next;
                    // if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
            // print the record separator
            printf("%c", rs);
    }
    
    return found;
}
void free_datastructs(char *type, char *httpUrl1, char *httpAttrs1, char *addr, char *scopes, char *iface)
{
     if (type != NULL)
     {
         free(type);
     }
     if (httpUrl1 != NULL)
     {
         free(httpUrl1);
     }
     if (httpAttrs1 != NULL)
     {
         free(httpAttrs1);
     }
     if (addr != NULL)
     {
         free(addr);
     }
     if(scopes != NULL)
     {
         free(scopes);
     }
     if(iface != NULL)
     {
         free(iface);
     }
}

// Registration and verification for http
// This testcase register cimserver with http port and  checks if the registration is
// succesful or not. If the registration fails tests are terminated.
// It will also check whether the data registered with SLP is same as th einput data
// used for registration or not. If not test will be terminated. 
int test1 ()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    
    char *changedata = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;
    
    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client (addr,iface,SLP_PORT,"DSA", scopes,FALSE, FALSE)))
    {
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes,life );
        if(no_of_regs) 
           std::cout <<"++++++ Registration is Successful in test1 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test1 ++++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs);    
        client->converge_srv_req(client, type, predicate, scopes);
        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;
        if(!client->get_response (client, &responses))
          std::cout <<"get response FAILED"<<std::endl;
        lslpURL *url_list;
        temp = responses.next;
 
        BOOL found = FALSE;
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout <<"+++++ test1 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test1 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found); 
   }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test1 ++++++"<<std::endl; 
   }  
   free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
   destroy_slp_client(client);
   return( 32 );
}

// This testcase register cimserver with http and https port and  checks if the registration is
// succesful or not. If the registration fails tests are terminated.
// It will also check whether the data registered with SLP is same as the input data
// used for registration or not. If not test will be terminated. 
// It will also deregister the registration for both http port and https port. If the registration
// fails testcase will be terminated.

int test2 ()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp,*temp1;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    char *httpUrl2 = (char *)NULL;
    char *httpAttrs2  = (char *)NULL;
    int no_of_regs=0;
    char *changedata = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;

    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();
    SLPHttpsAttribObj.fillData("https");
    SLPHttpsAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    httpUrl2 = strdup(SLPHttpsAttribObj.getServiceUrl().getCString());
    httpAttrs2 = strdup(SLPHttpsAttribObj.getAttributes().getCString());
    if(NULL != (client = create_slp_client(addr,
                                          iface,
                                          SLP_PORT,
                                          "DSA",
                                          scopes,
                                          FALSE,
                                          FALSE)))
    {

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(2);
        Boolean found = false;
        if(no_of_regs) 
            std::cout <<"++++ Registration is Successful in test2 +++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test2 +++++"<<std::endl;

	client->converge_srv_req(client, type, predicate, scopes);

	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	client->get_response(client, &responses);

	lslpURL *url_list;
	temp = responses.next;
        
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout <<"++++++ test2 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test2 failed +++++"<<std::endl;
        found = false;    

	no_of_regs = client->srv_reg_local (client, httpUrl2, httpAttrs2, type, scopes, life);
        
        if(no_of_regs) 
            std::cout <<"++++ Registration is Successful in test2 +++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test2 +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs);
	client->converge_srv_req (client, type, predicate, scopes);

	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	client->get_response (client, &responses);

	temp = responses.next;
        found = parseFind1(temp, httpAttrs2);
        if(found)
           std::cout <<"++++++ test2 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test2 failed +++++"<<std::endl;
        found = false;    

	sleep(3);
       // Unregister http
	no_of_regs = client->srv_reg_local (client, httpUrl1, httpAttrs1, type, scopes,0);

	if (no_of_regs)
	{
            std::cout<<"test2 Unregistration for http successful"<<std::endl;
	}
	else
	{
	    std::cout<<"test2 Failed to Unregistor "<<std::endl;
	}
        PEGASUS_TEST_ASSERT(no_of_regs);
	client->converge_srv_req (client, type, predicate, scopes);


	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	client->get_response (client, &responses);
	lslpURL *url_list1;
	temp1 = responses.next;
        found = parseFind1(temp1, httpAttrs1);
        if (found)
            std::cout<<"+++++ testr2 failed as unregistered item was found +++++"<<std::endl;
        else
            std::cout<<"+++++ test2 passed as unregistered item is not found +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(!found);
        found = false;
	no_of_regs = client->srv_reg_local(client, httpUrl2, httpAttrs2, type, scopes,0);

	if (no_of_regs)
	{
	std::cout<<"++++++ test2 Unregistration for https is successful ++++++"<<std::endl;
	}
	else
	{
	std::cout<<"++++++ test2 Failed to Unregistrer https ++++++"<<std::endl;
	}
	client->converge_srv_req (client, type, predicate, scopes);

	sleep(5);

	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	client->get_response (client, &responses);
	temp1 = responses.next;
        found = parseFind1(temp1, httpAttrs2);
        if (found)
            std::cout<<"+++++ test2 failed as unregistered item was found +++++"<<std::endl;
        else
            std::cout<<"+++++ test2 passed as unregistered item is not found +++++" <<std::endl;
        PEGASUS_TEST_ASSERT(!found);
        found = false;
   }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test2 ++++++"<<std::endl; 
   }  
   if (httpUrl2 != NULL)
   {
       free(httpUrl2);
   }
   if (httpAttrs2 != NULL)
   {
       free(httpAttrs2);
   }
   free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
   destroy_slp_client(client);
   return( 32 );
}

// This testcase register CIMServer with http port and validates the registration.
// Deregisters the registered service and check if we can find the unregistered
// service. If found testcase would fail else passed.

void test3 ()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp,*temp1;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    int no_of_regs=0;
    char *changedata = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;

    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());

    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
                                           type, scopes, life);
        sleep(2);
        Boolean found = false;
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test3 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test3 ++++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs); 
        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout<< "+++++ registered service is same as quired service+++++"<<std::endl;
        else
           std::cout <<"++++++ failed -- registered service is not same as quired service +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
        found = false;  
        //deregister http
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes,0);

        if (no_of_regs)
        {
	    std::cout<<"+++++ http Unregistration is successful in test3 ++++++"<<std::endl;
        }
        else
        {
            std::cout<<"+++++ Failed to Unregister https in test3 ++++++"<<std::endl;
        }
        PEGASUS_TEST_ASSERT(no_of_regs); 
        
        client->converge_srv_req(client, type, predicate, scopes);


        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);
        lslpURL *url_list1;
        temp1 = responses.next;
        found = parseFind1(temp1, httpAttrs1);
        if (found)
            std::cout<<"+++++ test3 failed as unregistered item was found +++++"<<std::endl;
        else
            std::cout<<"+++++ test3 passed as unregistered item is not found +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(!found);
    }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test3 ++++++"<<std::endl; 
   }  
    free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return;
}

// To check if a registration is done correctrly or not.
// This would validate service registration and registerdata for the service.
// Also tests for a non-existent service.

int test4()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    char *httpUrl2 = (char *)NULL;
    char *httpAttrs2  = (char *)NULL;
    int no_of_regs=0;
    char *changedata = (char *)NULL;

    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;  // is used for checking a failing case.
    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();
    SLPHttpsAttribObj.fillData("https");
    SLPHttpsAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    httpUrl2 = strdup(SLPHttpsAttribObj.getServiceUrl().getCString());
    httpAttrs2 = strdup(SLPHttpsAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
	no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
							type, scopes, life);
	sleep(2);
        Boolean found = false;
        if(no_of_regs) 
           std::cout <<"++++ Registration is Successful in test4 +++++"<<std::endl;
        else
           std::cout <<"++++++ Registration failed in test4 +++++"<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	client->get_response(client, &responses);

	lslpURL *url_list;
	temp = responses.next;
        // Test for existing data , should pass.
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout<<"+++++ test4 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test4 failed +++++"<<std::endl;
        found = false;
        // Test for non- existent registrtaion. should fail
        found = parseFind1(temp, httpAttrs2);
        if(!found)
           std::cout<<"+++++ test4 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test4 failed +++++"<<std::endl;
       
        PEGASUS_TEST_ASSERT(!found);      
    }
    else
    {
        std::cout <<"++++++ Failed to create SLP client in test4 ++++++"<<std::endl; 
    }
   if (httpUrl2 != NULL)
   {
       free(httpUrl2);
   }
   if (httpAttrs2 != NULL)
   {
       free(httpAttrs2);
   }
    free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}
//
void test5()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    char *httpUrl2 = (char *)NULL;
    char *httpAttrs2  = (char *)NULL;
    int no_of_regs=0;
    char *changedata = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;

    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();
    SLPHttpsAttribObj.fillData("https");
    SLPHttpsAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    httpUrl2 = strdup(SLPHttpsAttribObj.getServiceUrl().getCString());
    httpAttrs2 = strdup(SLPHttpsAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(2);
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test5 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test5 ++++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs);
        
        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        
        Boolean found = false;
        
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout <<"++++++ test5 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test5 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
        found = false;
        
        no_of_regs = client->srv_reg_local(client, httpUrl2, httpAttrs2, type, scopes, life);
        sleep(5);
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test5 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test5 ++++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs);

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        found = parseFind1(temp, httpAttrs2);
        if(found)
           std::cout <<"++++++ test5 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test5 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
    }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test5 ++++++"<<std::endl; 
   }  
   if (httpUrl2 != NULL)
   {
       free(httpUrl2);
   }
   if (httpAttrs2 != NULL)
   {
       free(httpAttrs2);
   }
   free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
   destroy_slp_client(client);
   return;
}

// This testcase unregister a service and  and check for the empty response for the SA.

void  test6()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;;
    char *httpAttrs1  = (char *)NULL;
    int no_of_regs=0;
    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;
    
    // Get all the SLP attributes and data for the Pegasus cimserver.
    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();

    scopes = strdup("DEFAULT");
    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
	no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, 0);
	sleep(2);
        Boolean found = false;
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test6 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test6 ++++++"<<std::endl;
        sleep(5);
	client->converge_srv_req(client, type, predicate, scopes);

	responses.isHead = TRUE;
	responses.next = responses.prev = &responses;

	if(!client->get_response(client, &responses))
          std::cout<<"Get Response failed in test6"<<std::endl;
	lslp_print_srv_rply_parse(&responses, fs, rs);
	lslp_print_srv_rply(&responses);

	lslpMsg *srvrply;
	lslpURL *url_list;
	srvrply = &responses;
	url_list = srvrply->msg.srvRply.urlList->next;
        if (url_list != NULL && srvrply != NULL && srvrply->msg.srvRply.urlList != 0)
        {
             std::cout<<" +++++ test6 Failed +++++"<<std::endl;
        }
        else
             std::cout<<" +++++ test6  passed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(!(url_list != NULL && srvrply != NULL && srvrply->msg.srvRply.urlList != 0));
    }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test6 ++++++"<<std::endl; 
   }  
    free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return;
}

// This testcase checks if the SLP allows duplicate registrations are not.
// SLP replaces the existing registration with the new registration. It will
// not maintain duplicate registrations.

void  test7()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;
    int no_of_regs=0;

    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
                                           type, scopes, life);
        sleep(2);
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test7 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test7 ++++++"<<std::endl;
        PEGASUS_TEST_ASSERT(no_of_regs);
        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        Boolean found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout <<"++++++ test7 passed  +++++"<<std::endl;
        else
           std::cout <<"++++++ test7 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
        found = false;

        //Register with same data, as used in earlier registrtion.

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(5);

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout<<"+++++ test7 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test7 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
    }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test7 ++++++"<<std::endl; 
   }  

    free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return;
}

// This testcase registers cimserver and validates the registration.
// Modifies the registration data and reregisters with modified data and
// validates if this reregistration is ocrrect or not. 

void  test8()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *scopes;
    int life = MAX_LIFE, port=SLP_PORT;
    char *addr = strdup(LOCALHOST_IP);
    char *type = (char *)NULL;
    char *iface = NULL;
    char *httpUrl1 = (char *)NULL;
    char *httpAttrs1  = (char *)NULL;
    char *httpUrl2 = (char *)NULL;
    char *httpAttrs2  = (char *)NULL;
    SLPAttrib SLPHttpAttribObj;
    SLPAttrib SLPHttpsAttribObj;
    int no_of_regs=0;
    char *changedata = (char *)NULL;

    SLPHttpAttribObj.fillData("http");
    SLPHttpAttribObj.formAttributes();
    SLPHttpsAttribObj.fillData("https");
    SLPHttpsAttribObj.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj.getAttributes().getCString());
    httpAttrs2 = strdup(SLPHttpsAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(2);
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful in test8 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test8 ++++++"<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        
        if (temp != NULL && temp->type == srvRply)
        {
            if((NULL != temp->msg.srvRply.urlList) &&
                    (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while( ! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if(NULL != url_list->attrs &&
                                                    ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                            lslpAtomList *attrs = url_list->attrs->next;
                            while(! _LSLP_IS_HEAD(attrs) && attrs->str
                                            && strlen(attrs->str))
                            { //while traversing attr list
                                    if(!String::compare(attrs->str,httpAttrs1))
                                    {
                                        changedata = replac(attrs->str,"http://127.0.0.1:5988");
                                    }
                                    attrs = attrs->next;
                            }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                            printf("%c", rs);
                    }
                    url_list = url_list->next;// if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
            // print the record separator
            printf("%c", rs);
        }

        no_of_regs = client->srv_reg_local(client, httpUrl1,
                                         changedata, type, scopes, life);
        sleep(5);
        if(no_of_regs) 
            std::cout <<"++++++ Registration is Successful with modified data in test8 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed with modified data in test8 ++++++"<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        Boolean found = parseFind1(temp, changedata);
        if(found)
           std::cout <<"++++++ test8 passed  +++++"<<std::endl;
        else
           std::cout <<"++++++ test8 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found);
    }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test8 ++++++"<<std::endl; 
   }  
   if (httpUrl2 != NULL)
   {
       free(httpUrl2);
   }
   if (httpAttrs2 != NULL)
   {
       free(httpAttrs2);
   }
   free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
   destroy_slp_client(client);
   return;
}


// Registration and verification of https
// This testcase register cimserver with https port and  checks if the registration is
// succesful or not. If the registration fails tests are terminated.
// It will also check whether the data registered with SLP is same as th einput data
// used for registration or not. If not test will be terminated. 
int test9 ()
{
    struct slp_client *client;
    time_t now,last;
    lslpMsg msg_list, responses,*temp;
    char *changedata = (char *)NULL;
    SLPAttrib SLPHttpsAttribObj;
    
    SLPHttpsAttribObj.fillData("https");
    SLPHttpsAttribObj.formAttributes();
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpsAttribObj.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpsAttribObj.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpsAttribObj.getAttributes().getCString());
    if (NULL != (client = create_slp_client (addr,
                                            iface,
                                            SLP_PORT,
                                            "DSA",
                                            scopes,
                                            FALSE,
                                            FALSE)))
    {

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes,life );
        if(no_of_regs) 
           std::cout <<"++++++ Registration is Successful in test9 ++++++"<<std::endl;
        else
            std::cout <<"++++++ Registration failed in test9 ++++++"<<std::endl;
        
        PEGASUS_TEST_ASSERT(no_of_regs);    
            
        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        if(!client->get_response (client, &responses))
          std::cout<<"get response FAILED"<<std::endl;

        lslpURL *url_list;
        temp = responses.next;
        
        BOOL found = FALSE;
        found = parseFind1(temp, httpAttrs1);
        if(found)
           std::cout <<"+++++ test9 passed +++++"<<std::endl;
        else
           std::cout <<"++++++ test9 failed +++++"<<std::endl;
        PEGASUS_TEST_ASSERT(found); 
   }
   else
   {
       std::cout <<"++++++ Failed to create SLP client in test9 ++++++"<<std::endl; 
   }  
       
   free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
   destroy_slp_client(client);
   return( 32 );
}

char* replac (char *s,char *t)
{
    char *substr = (char *)malloc(sizeof(char) * (strlen(s)));
    char *new1= "https://127.0.0.1:5989";
    int len = strlen(t);
    int index = find(s,t);
    if (s == NULL || s == 0)
    {
        return(NULL);
    }
    strncpy(substr,s,index);
    substr[index]='\0';
    strcat(substr,new1);
    char *finalstr = s + index + strlen(t);
    strcat(substr,finalstr);
    return(substr);
}


int find (char *str,char *t)
{

    int len = strlen(str);
    int tlen = strlen(t)-1;
    int i=0,index=-1,j=0;
    int found=1;
    int save=0;
    while (len>i)
    {
        save=i;
        if (str[i]==t[0])
        {
            index = i;
            while (j<tlen && i<len)
            {
                if (str[i]==t[j])
                {
                    i++;
                    j++;
                }
                else
                {
                    found=0;
                    break;
                }
            }
            if (found)
            {
                return(index);
            }
			i = save + 1;
        }
        else
        {
            i+=1;
            if (len==i)
            {
                return(-1);
            }
        }
    }
    return(-1);
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    return 0;
}
