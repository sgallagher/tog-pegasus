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

static void
_extractAttributes(const String &attrList,
                   Array<Attribute> &attrArray)
{
  Uint32 posAttrKey=0, posEqual=0;

  posAttrKey = attrList.find(PEG_SLP_ATTR_BEGIN);
  while (posAttrKey != PEG_NOT_FOUND && (posAttrKey+1) < attrList.size())
    {
      posEqual = attrList.find(posEqual+1, PEG_SLP_ATTR_END);
      String attrKey(attrList.subString((posAttrKey+1), (posEqual-posAttrKey-1)));

      attrArray.append(Attribute(attrKey));
      
      // ATTN: skip over anything in value that is a '(', '=', ')', or ','?
      posAttrKey = attrList.find(posAttrKey+1, PEG_SLP_ATTR_BEGIN);
    }
}

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
  int8 *type = strdup("service:wbem");
  int8 *predicate = NULL;
  int16 port = 427;
  BOOL dir_agent = FALSE;
  
  Array<CIMServerDescription> connections;
  
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

                              Array<Attribute> attributes;
	
                              /* check for attributes */
                              if (NULL != url_list->attrs && !_LSLP_IS_HEAD(url_list->attrs->next))
                                {
                                  lslpAtomList *attrs = url_list->attrs->next;
                                  while(!_LSLP_IS_HEAD(attrs))
                                    {
                                      _extractAttributes(String(attrs->str), attributes);
                                      attrs = attrs->next;
                                    }
                                }

                              /* add to connections array */
                              connection.setAttributes(attributes);
                              connections.append(connection);
                            }

                          url_list = url_list->next;
                        }
                    }
                }
            }
          _LSLP_UNLINK(srvReplyEntry);
          lslpDestroySLPMsg(srvReplyEntry, LSLP_DESTRUCTOR_DYNAMIC);
        }
      destroy_slp_client(client);
    }

  return connections;
}

PEGASUS_NAMESPACE_END

