//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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

#include "OpenSLPWrapper.h"

PEGASUS_NAMESPACE_BEGIN

static SLPBoolean
wbemSrvUrlCallback(SLPHandle hslp, 
                   const char* srvurl, 
                   unsigned short lifetime, 
                   SLPError errcode, 
                   void* cookie)
{
  if (errcode == SLP_OK)
    {
      Array<String> *url_cookie = static_cast<Array<String> *>(cookie);
      url_cookie->append(srvurl);
    }
    
    return SLP_TRUE;
}

static SLPBoolean
wbemAttrCallback( SLPHandle hslp, 
                  const char* attrlist, 
                  SLPError errcode, 
                  void* cookie)
{
    if (errcode == SLP_OK)
      {
        Array<Attribute> *attr_cookie = static_cast<Array<Attribute> *>(cookie);

        String attrList(attrlist);
        Uint32 posAttrKey=0, posEqual=0;

        posAttrKey = attrList.find(PEG_SLP_ATTR_BEGIN);
        while (posAttrKey != PEG_NOT_FOUND && (posAttrKey+1) < attrList.size())
          {
            posEqual = attrList.find(posEqual+1, PEG_SLP_ATTR_DELIMITER);
            String attrKey(attrList.subString((posAttrKey+1), (posEqual-posAttrKey-1)));

            // SLPParseAttrs()
            // @param1 - attribute list - from attribute call back
            // @param2 - attribute id - this the name of the attribute to obtain
            // @param3 - value of attribute - output param
            String attrValue;
            char *attr_value = NULL;
            if (SLP_OK == ::SLPParseAttrs(
                              (const char *)attrList.getCString(),
                              (const char *)attrKey.getCString(),
                              &attr_value))
              {
                attr_cookie->append(Attribute(attrKey + "=" + String(attr_value)));
                attrValue = attr_value;

                // free up slp library memory
                ::SLPFree(attr_value);
              }
            
            // ATTN: skip over anything in value that is a '(', '=', ')', or ','?
            posAttrKey = attrList.find(posAttrKey+1, PEG_SLP_ATTR_BEGIN);
          }
      }
    
    return SLP_TRUE;
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
  SLPError result;
  SLPHandle hslp;
  Array<CIMServerDescription> connections;

  // SLPOpen()
  // @param1 - language - NULL is the default locale
  // @param2 - async - FALSE is synchronous slp handle
  // @param3 - handle - pointer to slp handle
  if (SLPOpen(NULL, SLP_FALSE, &hslp) == SLP_OK)
    {
      Attribute attrServiceId;
      for (Uint32 idx=0; idx<criteria.size(); idx++)
        {
          if (criteria[idx].find(PEG_WBEM_SLP_SERVICE_ID) == 0)
            {
              attrServiceId = criteria[idx];
            }
        }

      String serviceType(PEG_WBEM_SLP_TYPE);
      String serviceId(attrServiceId.getValue(String()));
      if (serviceId != String::EMPTY)
         serviceType = serviceId;

      // SLPFindSrvs()
      // @param1 - handle - slp handle
      // @param2 - service type - wbem
      // @param3 - scope list - NULL is all localhost can query
      // @param4 - filter - NULL is all that match type
      // @param5 - pointer to custom data to use in callback
      Array<String> urls;
      result = ::SLPFindSrvs(hslp,
                             (const char *)serviceType.getCString(),
                             NULL,
                             NULL,
                             wbemSrvUrlCallback,
                             (void *)&urls);

      if (result == SLP_OK)
        {
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

  return connections;
}

PEGASUS_NAMESPACE_END

