//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "PegasusSLPWrapper.h"

PEGASUS_NAMESPACE_BEGIN

CIMServerDiscoveryRep::CIMServerDiscoveryRep()
{
}

CIMServerDiscoveryRep::~CIMServerDiscoveryRep()
{
}

Array<CIMServerDescription>
CIMServerDiscoveryRep::lookup(const Array<Attribute> & criteria)
{
  struct slp_client *client = NULL;
  lslpMsg responses, *srvReplyEntry;
  int8 *scopes = strdup("DEFAULT");
  int8 *iface = NULL;
  int8 *addr = NULL;
  int8 *type = strdup("service:wbem.pegasus");
  int8 *predicate = NULL;
  int16 port = 427;
  BOOL dir_agent = FALSE;
  
  if (NULL != (client = create_slp_client(addr, 
           iface, 
           port, 
           "DSA", 
           scopes, 
           FALSE, 
           dir_agent)))
    {
      if (addr != NULL && inet_addr(addr) == inet_addr("127.0.0.1"))
        {
          client->local_srv_req(client, type, predicate, scopes);
        }
      else
        {
          SOCKADDR_IN address;
          address.sin_port = htons(port);
          address.sin_family = AF_INET;
          if (addr != NULL)
            address.sin_addr.s_addr = inet_addr(addr);
          else
            address.sin_addr.s_addr = _LSLP_MCAST;
          client->unicast_srv_req(client, type, predicate, scopes, &address);
        }

      responses.isHead = TRUE;
      responses.next = responses.prev = &responses;
      
      client->get_response(client, &responses);
      while(!_LSLP_IS_EMPTY(&responses))
        {
          srvReplyEntry = responses.next;
          if (srvReplyEntry->type == srvRply) 
            {
              lslpURL *url_list;
              if (srvReplyEntry != NULL && srvReplyEntry->type == srvRply)
                {
                  if ((NULL != srvReplyEntry->msg.srvRply.urlList) && 
                     (!_LSLP_IS_EMPTY( srvReplyEntry->msg.srvRply.urlList)))
                    {
                      url_list = srvReplyEntry->msg.srvRply.urlList->next;
                      while(!_LSLP_IS_HEAD(url_list))
                        {
                          /* check for urls */
                          if (NULL != url_list->url)
                            {
                              CIMServerDescription connection(url_list->url);
                              // printf("URL: %s\n", url_list->url);

                              Array<Attribute> attributes;
	
                              /* check for attributes */
                              if (NULL != url_list->attrs && !_LSLP_IS_HEAD(url_list->attrs->next))
                                {
                                  lslpAtomList *attrs = url_list->attrs->next;
                                  while(!_LSLP_IS_HEAD(attrs))
                                    {
                                      attributes.append(Attribute("template=" + String(attrs->str)));
                                      // printf("ATTR: %s\n", attrs->str);
                                      attrs = attrs->next;
                                    }
                                }
                            }

                          url_list = url_list->next;
                        }
                    }
                  // printf("\n\n");
                }
            }
          _LSLP_UNLINK(srvReplyEntry);
          lslpDestroySLPMsg(srvReplyEntry, LSLP_DESTRUCTOR_DYNAMIC);
        }
      destroy_slp_client(client);
    }
/*
      for (Uint32 i=0; i<urls.size(); i++)
            {
              CIMServerDescription connection(urls[i]);

              Array<Attribute> attributes;

              // SLPFindAttrs()
              // @param1 - handle - slp handle
              // @param2 - service url or type
              // @param3 - scope list - NULL is all localhost can query
              // @param4 - attribute list - NULL is all attributes
              // @param5 - pointer to custom data to use in callback
              result = ::SLPFindAttrs( hslp,
                                       (const char *)urls[i].getCString(),
                                       NULL,
                                       NULL,
                                       wbemAttrCallback,
                                       (void *)&attributes);

              // SLPParseSrvURL()
              // @param1 - url - obtained from SLPFindSrvs()
              // @param2 - parsed url - output param
              SLPSrvURL *pSrvUrl = NULL;
              if ( SLP_OK == ::SLPParseSrvURL(
                                    (const char *)urls[i].getCString(),
                                    &pSrvUrl))
                {
                  // add to the end to protect against existing attributes of the same name.
                  attributes.append(Attribute(PEG_CUSTOM_ATTR_HOST"=" + String(pSrvUrl->s_pcHost)));
                  CIMValue value(Uint32(pSrvUrl->s_iPort));
                  attributes.append(Attribute(PEG_CUSTOM_ATTR_PORT"=" + String(value.toString())));

                  // free up slp library memory
                  ::SLPFree(pSrvUrl);
                }
              connection.setAttributes(attributes);
              connections.append(connection);
            }
        }

      // SLPClose()
      // @param1 - handle - slp handle
      SLPClose(hslp);
    }
*/
  return NULL;
}

PEGASUS_NAMESPACE_END

