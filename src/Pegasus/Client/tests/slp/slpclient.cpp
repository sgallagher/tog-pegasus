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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMServerDescription.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Client/CIMServerDiscovery.h>
#include <Pegasus/Client/WBEMSLPTemplate.h>
#include <string.h> // for strdup

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(int argc, char** argv)
{
#ifdef PEGASUS_ENABLE_SLP
  try
    {
      CIMServerDiscovery disco;

      Array<Attribute> criteria;
      Attribute attr(PEG_WBEM_SLP_SERVICE_ID"="PEG_WBEM_SLP_SERVICE_ID_DEFAULT);
      Array<CIMServerDescription> connections ;
	      SLPClientOptions* opts = (SLPClientOptions*)NULL; //new SLPClientOptions();
      if(argc==2){
	      // argv[1] should be a DA address
	      opts = new SLPClientOptions();
	      opts->target_address = strdup(argv[1]);
	      opts->scopes=strdup("DEFAULT");
	      opts->spi=strdup("");
	      opts->use_directory_agent = false;
      }
      connections = disco.lookup(opts);
      if((SLPClientOptions*)NULL!=opts){
	      delete opts;
      }
	
      for (Uint32 i=0; i<connections.size(); i++)
        {
          PEGASUS_STD(cout) << "\n======================================================" << PEGASUS_STD(endl);
          PEGASUS_STD(cout) << connections[i].getUrl() << PEGASUS_STD(endl);
          PEGASUS_STD(cout) << "======================================================" << PEGASUS_STD(endl);
          Array<Attribute> attributes = connections[i].getAttributes();
          for (Uint32 j=0; j<attributes.size(); j++)
            {
              PEGASUS_STD(cout) << "'" << attributes[j] << "'" << PEGASUS_STD(endl);
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
#else
  PEGASUS_STD(cout) << "+++++ PEGASUS_ENABLE_SLP *not* set during the pegasus build" << PEGASUS_STD(endl);
#endif // PEGASUS_ENABLE_SLP

  return 0;
}
