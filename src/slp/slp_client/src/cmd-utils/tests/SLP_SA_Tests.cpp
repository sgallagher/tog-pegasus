#define SLP_LIB_IMPORT
#include "../slp_client/slp_client.h"
#include <Pegasus/Client/CIMClient.h>
#define SLP_PORT 427
#define MAX_LIFE 9999
#define LOCALHOST_IP "127.0.0.1"
#include <Pegasus/Server/SLPAttrib.h>
int find(char *,char *);
char* replac(char *,char *);

using namespace Pegasus;
SLPAttrib SLPHttpAttribObj1;
SLPAttrib SLPHttpAttribObj2;

    static char *predicate;
    static BOOL parsable= TRUE;
    static char fs='\t', rs='\n';
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

int test1 ()
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
    int no_of_regs=0;
    char *changedata = (char *)NULL;

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();


    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    if (NULL != (client = create_slp_client (addr,
                                            iface,
                                            SLP_PORT,
                                            "DSA",
                                            scopes,
                                            FALSE,
                                            FALSE)))
    {

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(2);
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response (client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {/*start of url if*/
                url_list = temp->msg.srvRply.urlList->next;
                while (! _LSLP_IS_HEAD(url_list))
                {/*start of url while */
                    if (NULL != url_list->attrs &&
                        ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                        lslpAtomList *attrs = url_list->attrs->next;
                        while (! _LSLP_IS_HEAD (attrs) && attrs->str &&
                               strlen (attrs->str))
                        { //while traversing attr list
                            if (!String::compare (attrs->str,httpAttrs1))
                            {
                                std::cout <<"Respose is same as expected \n "
                                <<attrs->str<<std::endl;
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

    }
    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}


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

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();
    SLPHttpAttribObj2.fillData("https");
    SLPHttpAttribObj2.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    httpUrl2 = strdup(SLPHttpAttribObj2.getServiceUrl().getCString());
    httpAttrs2 = strdup(SLPHttpAttribObj2.getAttributes().getCString());
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
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

		client->converge_srv_req(client, type, predicate, scopes);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response(client, &responses);

		lslpURL *url_list;
		temp = responses.next;
		if (temp != NULL && temp->type == srvRply)
		{
		    if ((NULL != temp->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY (
									  temp->msg.srvRply.urlList)))
				{//start of url if
                    url_list = temp->msg.srvRply.urlList->next;
                    while (!_LSLP_IS_HEAD (url_list))
                    {//start of url while
                        if (NULL != url_list->attrs && !
                                       _LSLP_IS_HEAD (url_list->attrs->next))
                        {
                            lslpAtomList *attrs = url_list->attrs->next;
                            while (! _LSLP_IS_HEAD(attrs) && attrs->str && strlen(attrs->str))
                            { //while traversing attr list
								if (!String::compare(attrs->str,httpAttrs1))
								{
										std::cout <<"Respose is same as expected \n "
												  <<attrs->str<<std::endl;
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
			printf ("%c", rs);

		}

		no_of_regs = client->srv_reg_local (client, httpUrl2, httpAttrs2, type, scopes, life);
		sleep (5);
		std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

		client->converge_srv_req (client, type, predicate, scopes);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response (client, &responses);

		temp = responses.next;
		if (temp != NULL && temp->type == srvRply)
		{
			if ((NULL != temp->msg.srvRply.urlList) &&
				(! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
			{//start of url if
				url_list = temp->msg.srvRply.urlList->next;
				while ( ! _LSLP_IS_HEAD(url_list))
				{//start of url while
					if (NULL != url_list->attrs && ! _LSLP_IS_HEAD(url_list->attrs->next))
					{
						lslpAtomList *attrs = url_list->attrs->next;
						while (! _LSLP_IS_HEAD(attrs) && attrs->str && strlen(attrs->str))
						{ //while traversing attr list
							if (!String::compare(attrs->str,httpAttrs2))
							{
								std::cout <<"Response is same as expected \n\n"
								<<attrs->str<<std::endl;
							}
							attrs = attrs->next;
						}  //while traversing attr list
					}
					else
					{  // if no attr list, print the record separator
						printf ("%c", rs);
					}
					url_list = url_list->next;// if there is another url, print
												// a record separator
				} // while traversing url list
			} // if there are urls to print

			// print the record separator
			printf ("%c", rs);
		}


		sleep(3);

		no_of_regs = client->srv_reg_local (client, httpUrl1, httpAttrs1,
											type, scopes,0);

		if (no_of_regs)
		{
			std::cout<<"Unregistration for http successful"<<std::endl;
		}
		else
		{
			std::cout<<"Failed to Unregistor "<<std::endl;
		}
		client->converge_srv_req (client, type, predicate, scopes);

		sleep (5);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response (client, &responses);
		lslpURL *url_list1;
		temp1 = responses.next;
		if (temp1 != NULL && temp1->type == srvRply)
		{
			if ((NULL != temp1->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(
								temp1->msg.srvRply.urlList)))
			{//start of url if
				url_list1 = temp1->msg.srvRply.urlList->next;
				while ( ! _LSLP_IS_HEAD(url_list1))
				{//start of url while
					if (NULL != url_list1->attrs && ! _LSLP_IS_HEAD(
									url_list1->attrs->next))
					{
						lslpAtomList *attrs1 = url_list1->attrs->next;
						while (! _LSLP_IS_HEAD(attrs1) && attrs1->str &&
							strlen (attrs1->str))
						{ //while traversing attr list
							if (attrs1->str == 0)
							{
								std::cout <<"\n\n Testcase failed \n\n"
								<<strlen(attrs1->str)<<std::endl;
							}
							attrs1 = attrs1->next;
						}  //while traversing attr list
					}
					else
					{  // if no attr list, print the record separator
						printf("%c", rs);
					}
					url_list1 = url_list1->next;// if there is another url, print a record separator
				} // while traversing url list
			} // if there are urls to print
			// print the record separator
			printf ("%c", rs);
	    }

		no_of_regs = client->srv_reg_local(client, httpUrl2, httpAttrs2, type, scopes,0);

		if (no_of_regs)
		{
				std::cout<<"Unregistration for https is successful"<<std::endl;
		}
		else
		{
			std::cout<<"Failed to Unregistor https "<<std::endl;
		}
		client->converge_srv_req (client, type, predicate, scopes);

		sleep(5);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response (client, &responses);
		temp1 = responses.next;

		if (temp1 != NULL && temp1->type == srvRply)
		{
				if ((NULL != temp1->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(
									temp1->msg.srvRply.urlList)))
				{//start of url if
					url_list1 = temp1->msg.srvRply.urlList->next;
					while ( ! _LSLP_IS_HEAD (url_list1))
					{//start of url while
						if(NULL != url_list1->attrs && ! _LSLP_IS_HEAD
																(url_list1->attrs->next))
						{
							lslpAtomList *attrs1 = url_list1->attrs->next;
							while (! _LSLP_IS_HEAD(attrs1) && attrs1->str &&
											strlen (attrs1->str))
							{ //while traversing attr list
								if(attrs1->str == 0)
								{
									std::cout <<"\n\n Testcase failed \n\n"<<strlen(attrs1->str)<<std::endl;
								}
								attrs1 = attrs1->next;
							}  //while traversing attr list
						}
						else
						{  // if no attr list, print the record separator
							printf("%c", rs);
						}
						url_list1 = url_list1->next;// if there is another url, print a record separator
					} // while traversing url list
				} // if there are urls to print
				// print the record separator
				printf("%c", rs);
			}

	   
    }

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

int test3 ()
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

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();

    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());

    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
        std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
                                           type, scopes, life);
        sleep(2);
        if (no_of_regs)
        {
            std::cout<<"Registration is successful"<<std::endl;
        }
        else
        {
            std::cout<<"Failed to Registor "<<std::endl;
        }

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(
                                                          temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while (! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if (NULL != url_list->attrs &&
                            ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                            lslpAtomList *attrs = url_list->attrs->next;
                            while (! _LSLP_IS_HEAD(attrs) && attrs->str
                                            && strlen(attrs->str))
                            { //while traversing attr list
                            if (!String::compare(attrs->str,httpAttrs1))
                            {
                            std::cout <<"Respose is same as expected \n "
                            <<attrs->str<<std::endl;
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


        sleep(3);
        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
                                                                                type, scopes,0);

        if (no_of_regs)
        {
	        std::cout<<"Unregistration is successful"<<std::endl;
        }
        else
        {
            std::cout<<"Failed to Unregistor "<<std::endl;
        }
        client->converge_srv_req(client, type, predicate, scopes);

        sleep(5);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);
        lslpURL *url_list1;
        temp1 = responses.next;
        if (temp1 != NULL && temp1->type == srvRply)
        {
            if ((NULL != temp1->msg.srvRply.urlList) && (! _LSLP_IS_EMPTY(
                            temp1->msg.srvRply.urlList)))
            {//start of url if
                url_list1 = temp1->msg.srvRply.urlList->next;
                while ( ! _LSLP_IS_HEAD(url_list1))
                {//start of url while
                    if (NULL != url_list1->attrs &&
                            ! _LSLP_IS_HEAD(url_list1->attrs->next))
                    {
                        lslpAtomList *attrs1 = url_list1->attrs->next;
                        while (! _LSLP_IS_HEAD(attrs1) && attrs1->str
                                        && strlen(attrs1->str))
                        { //while traversing attr list
							if (attrs1->str == 0)
							{
								std::cout <<"\n\n Testcase failed \n\n"
								<<strlen(attrs1->str)<<std::endl;
							}
							attrs1 = attrs1->next;
                        }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                        printf("%c", rs);
                        //dont_print_extra_rs = TRUE;
                    }
                    url_list1 = url_list1->next;
                        // if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
                // print the record separator
            printf("%c", rs);
        }


        sleep(3);


    }


free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

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

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
		std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
		no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
											type, scopes, life);
		sleep(2);
		std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

		client->converge_srv_req(client, type, predicate, scopes);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response(client, &responses);

		lslpURL *url_list;
		temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList) &&
                    (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while ( ! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if (NULL != url_list->attrs &&
                            ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                            lslpAtomList *attrs = url_list->attrs->next;
                            while (! _LSLP_IS_HEAD(attrs) && attrs->str
                                            && strlen(attrs->str))
                            { //while traversing attr list
                                    if (!String::compare(attrs->str,httpAttrs1))
                                    {
                                    std::cout <<"Respose is same as expected \n "
                                    <<attrs->str<<std::endl;
                                    }
                                    attrs = attrs->next;
                            }  //while traversing attr list
                    }
                    else
                    {
                        // if no attr list, print the record separator
                        printf("%c", rs);
                    }
                    url_list = url_list->next;// if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
            // print the record separator
            printf("%c", rs);
        }

        sleep(3);


    }
    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

int test5()
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

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();
    SLPHttpAttribObj2.fillData("https");
    SLPHttpAttribObj2.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    httpUrl2 = strdup(SLPHttpAttribObj2.getServiceUrl().getCString());
    httpAttrs2 = strdup(SLPHttpAttribObj2.getAttributes().getCString());
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
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList) &&
                            (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while ( ! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if (NULL != url_list->attrs &&
                            ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                        lslpAtomList *attrs = url_list->attrs->next;
                        while (! _LSLP_IS_HEAD(attrs) && attrs->str
                                        && strlen(attrs->str))
                        { //while traversing attr list
                            if (!String::compare(attrs->str,httpAttrs1))
                            {
                                std::cout <<"Respose is same as expected \n "<<attrs->str<<std::endl;
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

        no_of_regs = client->srv_reg_local(client, httpUrl2, httpAttrs2,
                                           type, scopes, life);
        sleep(5);
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList)
                            && (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while ( ! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if (NULL != url_list->attrs &&
                                    ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                        lslpAtomList *attrs = url_list->attrs->next;
                        while(! _LSLP_IS_HEAD(attrs) && attrs->str
                                        && strlen(attrs->str))
                        { //while traversing attr list
							if(!String::compare(attrs->str,httpAttrs2))
							{
								std::cout <<"Response is same as expected \n\n"
								<<attrs->str<<std::endl;
							}
							attrs = attrs->next;
                        }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                            printf("%c", rs);
                            //dont_print_extra_rs = TRUE;
                    }
                    url_list = url_list->next;// if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
                // print the record separator
            printf("%c", rs);
        }


        sleep(3);


    }
    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

int test6()
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
    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    // Get all the SLP attributes and data for the Pegasus cimserver.
    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    //std::cout<<httpUrl1<<"My Print"<<httpAttrs1<<std::endl;
    if (NULL != (client = create_slp_client(addr,
                                           iface,
                                           SLP_PORT,
                                           "DSA",
                                           scopes,
                                           FALSE,
                                           FALSE)))
    {
		no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1,
																					type, scopes, 0);
		sleep(2);
		std::cout <<"\n\n Num of Registration "<<no_of_regs<<std::endl;

		client->converge_srv_req(client, type, predicate, scopes);

		responses.isHead = TRUE;
		responses.next = responses.prev = &responses;

		client->get_response(client, &responses);
		lslp_print_srv_rply_parse(&responses, fs, rs);
		lslp_print_srv_rply(&responses);

		lslpMsg *srvrply;
		lslpURL *url_list;
		srvrply = &responses;
		url_list = srvrply->msg.srvRply.urlList->next;
        std::cout <<"Num of Registration "<<url_list->attrs<<std::endl;
        if (srvrply != NULL && srvrply->msg.srvRply.urlList != 0)
        {
            if(url_list->attrs == 0 || srvrply->type == srvRply)
            {
                    printf("!!!!!!!Invalid!!!!!11\n");
            }
            else
            {
                            printf("\n\n\n\n\n response \n\n\n");
            }
        }

    }
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

int test7()
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
    int no_of_regs=0;

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
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
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        lslpURL *url_list;
        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList)
                            && (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                    url_list = temp->msg.srvRply.urlList->next;
                    while (! _LSLP_IS_HEAD(url_list))
                    {//start of url while
                                    if (NULL != url_list->attrs &&
                                            ! _LSLP_IS_HEAD(url_list->attrs->next))
                                    {
                                    lslpAtomList *attrs = url_list->attrs->next;
                                    while (! _LSLP_IS_HEAD(attrs) && attrs->str
                                                    && strlen(attrs->str))
                                    { //while traversing attr list
                                    if(!String::compare(attrs->str,httpAttrs1))
                                    {
                            std::cout <<"Respose is same as expected \n "
                            <<attrs->str<<std::endl;
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

        no_of_regs = client->srv_reg_local(client, httpUrl1, httpAttrs1, type, scopes, life);
        sleep(5);
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if((NULL != temp->msg.srvRply.urlList)
                    && (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
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
									std::cout <<"Response is same as expected \n\n"
									<<attrs->str<<std::endl;
								}
								attrs = attrs->next;
							}  //while traversing attr list
                        }
                        else
                        {  // if no attr list, print the record separator
                                printf("%c", rs);
                                //dont_print_extra_rs = TRUE;
                        }
                        url_list = url_list->next;// if there is another url, print a record separator
                    } // while traversing url list
            } // if there are urls to print
                // print the record separator
            printf("%c", rs);
        }


        sleep(3);


    }

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    destroy_slp_client(client);
    return( 32 );
}

int test8()
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

    SLPHttpAttribObj1.fillData("http");
    SLPHttpAttribObj1.formAttributes();
    SLPHttpAttribObj2.fillData("https");
    SLPHttpAttribObj2.formAttributes();

    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
    scopes = strdup("DEFAULT");

    type = strdup(SLPHttpAttribObj1.getServiceType().getCString());
    httpUrl1 = strdup(SLPHttpAttribObj1.getServiceUrl().getCString());
    httpAttrs1 = strdup(SLPHttpAttribObj1.getAttributes().getCString());
    httpAttrs2 = strdup(SLPHttpAttribObj2.getAttributes().getCString());
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
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

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
                                    std::cout <<"Respose is same as expected \n "
                                    <<attrs->str<<std::endl;
                                    changedata = replac(attrs->str,"http://127.0.0.1:5988");
                                    }
                                    attrs = attrs->next;
                            }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                            printf("%c", rs);
                            //dont_print_extra_rs = TRUE;
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
        std::cout <<"Num of Registration "<<no_of_regs<<std::endl;

        client->converge_srv_req(client, type, predicate, scopes);

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);

        temp = responses.next;
        if (temp != NULL && temp->type == srvRply)
        {
            if ((NULL != temp->msg.srvRply.urlList) &&
            (! _LSLP_IS_EMPTY(temp->msg.srvRply.urlList)))
            {//start of url if
                url_list = temp->msg.srvRply.urlList->next;
                while ( ! _LSLP_IS_HEAD(url_list))
                {//start of url while
                    if(NULL != url_list->attrs &&
                            ! _LSLP_IS_HEAD(url_list->attrs->next))
                    {
                        lslpAtomList *attrs = url_list->attrs->next;
                        while(! _LSLP_IS_HEAD(attrs) && attrs->str
                                                        && strlen(attrs->str))
                        { //while traversing attr list
                            if(!String::compare(attrs->str,httpAttrs2))
                            {
                                std::cout <<"Response is same as expected"
                                <<attrs->str<<std::endl;
                            }
                            attrs = attrs->next;
                        }  //while traversing attr list
                    }
                    else
                    {  // if no attr list, print the record separator
                        printf("%c", rs);
                                //dont_print_extra_rs = TRUE;
                    }
                    url_list = url_list->next;// if there is another url, print a record separator
                } // while traversing url list
            } // if there are urls to print
            // print the record separator
            printf("%c", rs);
        }


        sleep(3);


    }
free_datastructs(type, httpUrl1, httpAttrs1, addr, scopes, iface);
    std::cout <<__FUNCTION__<<"\t"<<__LINE__<<std::endl;
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
    return 0;
}
//PEGASUS_NAMESPACE_END
