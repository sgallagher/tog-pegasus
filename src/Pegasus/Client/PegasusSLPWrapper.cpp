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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By: Stuart Naisbitt (stuartn@veritas.com)
//              David Dillard (david.dillard@veritas.com)
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

/*
 * PegasusSLP supports return of attributes piggybacked on the srvReply,
 * other implementations may not
 */
static BOOL
_lookup_attrs(const String &save_url, Array<Attribute>& attribs,const SLPClientOptions* options)
{
    struct slp_client *client = NULL;
    lslpMsg responses, *attrReplyEntry;
    
    const char *scopes;
    const char *spi;
    char *iface;
    char *addr;
    const char *type;
    const char *predicate;
    int16 port;
    BOOL dir_agent;
    
    char* tags = (char*)NULL;
    int16 converge=(int16)0;
    if(0==save_url.size()){
        /* Pilot error */
        return FALSE;
    }

    char* url=strdup((const char*)save_url.getCString());
    if (url == NULL)
    {
        return FALSE;
    }

    if((SLPClientOptions*)NULL == options){
        addr = NULL;
        iface = NULL;
        port = 427;
        scopes = "DEFAULT";
        spi="DSA";
        type = "service:wbem";
        predicate = NULL;
        dir_agent = FALSE;
    }else{
        scopes = (char*)options->scopes;
        spi = (char*)options->spi;
        iface = options->local_interface;
        addr = options->target_address;
        type = options->service_type;
        predicate = options->predicate;
        port = options->target_port;
        dir_agent = options->use_directory_agent==true?1:0;
    }
    /* largely cut-and-paste from slp_attrreq.cpp
     * with gratuitous reformatting
     */
    if(NULL != (client = create_slp_client(
                    addr,
                    iface,
                    port,
                    spi,
                    scopes,
                    FALSE, 
                    dir_agent))) {
        if(addr != NULL && inet_addr(addr) == inet_addr("127.0.0.1")) {
            client->local_attr_req(client, url, scopes, tags);
        } else if(converge) {
            client->_convergence = converge ;
            client->converge_attr_req(client, url, scopes, tags);
        } else {
            SOCKADDR_IN address;
            address.sin_port = htons(port);
            address.sin_family = AF_INET;
            if(addr != NULL) {
                address.sin_addr.s_addr = inet_addr(addr);
                client->unicast_attr_req(client, url, scopes, tags, &address);
            }else {
                address.sin_addr.s_addr = _LSLP_MCAST;
                client->converge_attr_req(client, url, scopes, tags);
            }
        } /* end of request  */

        responses.isHead = TRUE;
        responses.next = responses.prev = &responses;

        client->get_response(client, &responses);
        while( ! _LSLP_IS_EMPTY(&responses) ) {
            attrReplyEntry = responses.next;
            if(attrReplyEntry->type == attrRep) {
                if( attrReplyEntry->msg.attrRep.attrListLen > 0) {
                    String attrString = attrReplyEntry->msg.attrRep.attrList;
                    _extractAttributes(attrString,attribs);
                }
                
            }/* if we got an attr rply */ 
            _LSLP_UNLINK(attrReplyEntry);
            lslpDestroySLPMsg(attrReplyEntry, LSLP_DESTRUCTOR_DYNAMIC);
        } /* while traversing response list */ 

        destroy_slp_client(client);

     } /* client successfully created */ 

    free(url);
    return TRUE;
}/*static BOOL _lookup_attrs()*/

Array<CIMServerDescription>
CIMServerDiscoveryRep::lookup(const Array<Attribute> & criteria, const SLPClientOptions* options)
{
  struct slp_client *client = NULL;
  lslpMsg responses, *srvReplyEntry;
  
  const char *scopes; // = strdup("DEFAULT");
  const char *spi; // = strdup("DSA");
  char *iface; // = NULL;
  char *addr; // = NULL;
  const char *type; // = strdup("service:wbem");
  const char *predicate; // = NULL;
  int16 port; // = 427;
  BOOL dir_agent; // = FALSE;
 
  if((SLPClientOptions*)NULL == options){
      addr = NULL;
      iface = NULL;
      port = 427;
      scopes = "DEFAULT";
      spi="DSA";
      type = "service:wbem";
      predicate = NULL;
      dir_agent = FALSE;
  }else{
      scopes = (char*)options->scopes;
      spi = (char*)options->spi;
      iface = options->local_interface;
      addr = options->target_address;
      type = options->service_type;
      predicate = options->predicate;
      port = options->target_port;
      dir_agent = options->use_directory_agent==true?1:0;
      options->print();
      
  }
  Array<CIMServerDescription> connections;
  
  if (NULL != (client = create_slp_client(
                  addr, // target_addr
                  iface, // local_interface
                  port, // target_port
                  spi, // spi
                  scopes, // scopes
                  FALSE, // should_listen
                  dir_agent // use_das
                  )))
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
          if (addr != NULL){
            address.sin_addr.s_addr = inet_addr(addr);
      }else{
            address.sin_addr.s_addr = _LSLP_MCAST;
      }
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
                              /* PegasusSLP reurns attributes with srvReply
                               * per RFC 3059. Other implementations do not.
                               */
                              if (NULL != url_list->attrs && !_LSLP_IS_HEAD(url_list->attrs->next))
                                {
                                  lslpAtomList *attrs = url_list->attrs->next;
                                  while(!_LSLP_IS_HEAD(attrs))
                                    {
                                      _extractAttributes(String(attrs->str), attributes);
                                      attrs = attrs->next;
                                    }
                                }/*if attrs*/
                  /* add to connections array */
                  /* VRTS - interop. Add unconditionally.
                   * Not initialised in contsructor */
                  connection.setAttributes(attributes);
                  connections.append(connection);
                            }/*if url*/
              url_list = url_list->next;
                        }/*while we have urls*/
                    }/*if urlList*/
                }/*if srvReply*/
            }/*if type==srvReply*/
          _LSLP_UNLINK(srvReplyEntry);
          lslpDestroySLPMsg(srvReplyEntry, LSLP_DESTRUCTOR_DYNAMIC);
        }/*while*/
      destroy_slp_client(client);
    }

    /* If no attributes were present, then the attributes array
     * has zero size. Rescan the connections and create a fresh slp_client to 
     * get the attributes, if none present
     */
    if(0==connections.size()){
      return connections;
    }
    for(int i=0; i< connections.size();i++){
        Array<Attribute> attrs = connections[i].getAttributes();
        if(0==attrs.size()){
            String url=connections[i].getUrl();
            if(TRUE==_lookup_attrs(url,attrs,options)){
                connections[i].setAttributes(attrs);
            }
        }
    }

  return connections;
}

PEGASUS_NAMESPACE_END
