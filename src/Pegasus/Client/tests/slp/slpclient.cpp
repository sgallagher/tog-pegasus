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

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Client/CIMServerDiscovery.h>
#include <Pegasus/Client/CIMServerDescription.h>
#include <Pegasus/Client/WBEMSLPTemplate.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
  try
    {
      CIMServerDiscovery disco;

      Array<Attribute> criteria;
      Attribute attr(PEG_WBEM_SLP_SERVICE_ID"="PEG_WBEM_SLP_SERVICE_ID_DEFAULT);
//      criteria.append(attr);
//      Array<CIMServerDescription> connections = disco.lookup(criteria);
      Array<CIMServerDescription> connections = disco.lookup();

      for (Uint32 i=0; i<connections.size(); i++)
        {
          CIMClient client;
          String host(connections[i].getValue("host", "localhost"));
          String port(connections[i].getValue("port", "5988"));
          Uint32 port_number = atoi((const char *)port.getCString());

          try
            {
              PEGASUS_STD(cout) << "Attempting connection: '" << host << "' using port '"<< port << "'" << PEGASUS_STD(endl);
              Array<Attribute> attributes = connections[i].getAttributes();
              for (Uint32 j=0; j<attributes.size(); j++)
                {
                  PEGASUS_STD(cout) << "===> '" << attributes[j] << "'" << PEGASUS_STD(endl);
                }
              client.connect(host, port_number, "", "");
              PEGASUS_STD(cout) << "Successful connection: '" << host << "' using port '"<< port << "'" << PEGASUS_STD(endl);
              client.disconnect();
              PEGASUS_STD(cout) << "Disconnected connection: '" << host << "' using port '"<< port << "'" << PEGASUS_STD(endl);
            }
          catch(Exception& e)
            {
              PEGASUS_STD(cerr) << "Skip connection: " << e.getMessage() << PEGASUS_STD(endl);
            }
        }

      if (connections.size() == 0)
        {
          if (criteria.size() > 0)
            PEGASUS_STD(cout) << "Warning: No registered wbem connections found using criteria: '" << attr << "'" << PEGASUS_STD(endl);
          else
            PEGASUS_STD(cout) << "Warning: No registered wbem connections found." << PEGASUS_STD(endl);
        }
    }
  catch(Exception& e)
    {
      PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
      exit(1);
    }

  PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

  return 0;
}
