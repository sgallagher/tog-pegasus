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
// Author: Dong Xiang, EMC Corporation (xiang_dong@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <Pegasus/Common/Config.h>

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Listener/CIMListener.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// MyIndicationConsumer
//
////////////////////////////////////////////////////////////////////////////////
class MyIndicationConsumer : public CIMIndicationConsumer
{
public:
	MyIndicationConsumer(String name);
	~MyIndicationConsumer();
	
	void consumeIndication(const OperationContext& context,
		const String & url, 
		const CIMInstance& indicationInstance);

private:
	String name;

};
MyIndicationConsumer::MyIndicationConsumer(String name)
{
	this->name = name;
}
MyIndicationConsumer::~MyIndicationConsumer()
    {
}

void MyIndicationConsumer::consumeIndication(
		const OperationContext & context,
		const String & url,
	  const CIMInstance& indicationInstance)
	{
	String msg = "Consumer <" + name + "> received " +
		indicationInstance.getPath().toString();

	PEG_TRACE_STRING(TRC_LISTENER,Tracer::LEVEL4,msg);
	PEGASUS_STD(cerr) << msg << PEGASUS_STD(endl);
	}

////////////////////////////////////////////////////////////////////////////////
//
// MyTraceSettings
//
///////////////////////////////////////////////////////////////////////////////
class MyTraceSettings
{
public:
	static void setTraceComponents(String traceComponents);

	static String _traceFileName;
};

String MyTraceSettings::_traceFileName = "cimlistener.trc";

void MyTraceSettings::setTraceComponents(String traceComponents)
{
	// set trace path
	const char* tmp = getenv("PEGASUS_HOME");
	String pegasusHome = tmp;

	// set logger directory
	String logsDirectory = pegasusHome + String("/logs");
	Logger::setHomeDirectory(logsDirectory);
	Logger::setlogLevelMask("TRACE");

	// Set the file path to  $PEGASUS_HOME directory 
  String traceFilePath = pegasusHome + String("/") + _traceFileName;


	CString fileName = traceFilePath.getCString();
	if (Tracer::isValidFileName(fileName))
	{ 
		cout << "setTraceFile: " << (const char*)fileName << endl;

		Uint32 retCode = Tracer::setTraceFile(fileName);
	  // Check whether the filepath was set
	  if(retCode == 1)
	  {
			cout << "Unable to write to trace file: " << fileName << endl;

			Logger::put(Logger::DEBUG_LOG,System::CIMLISTENER,
	                Logger::WARNING,
	                "Unable to write to trace file $0",
	                (const char*)fileName);
    }
	}
	// set trace level
  Tracer::setTraceLevel(Tracer::LEVEL4);

	// set trace components
	Tracer::setTraceComponents(traceComponents);
}

////////////////////////////////////////////////////////////////////////////////
        //
// main()
        //
////////////////////////////////////////////////////////////////////////////////

int main()
{
	//String traceComponents = "Http,XmlIO";
	String traceComponents = "Listener";
	MyTraceSettings::setTraceComponents(traceComponents);

	try
	{

		int portNumber = 2003;
		CIMListener listener(portNumber);

		// add cosumer
		MyIndicationConsumer* consumer1 = new MyIndicationConsumer("1");
		listener.addConsumer(consumer1);

		MyIndicationConsumer* consumer2 = new MyIndicationConsumer("2");
		listener.addConsumer(consumer2);

		// start listener
		listener.start();		

		char buf[255]={0};
		while(true)
		{
			cin.getline(buf,255);
			
			if(strlen(buf)>0 && strcmp(buf,"exit")==0)
				break;
		}

		delete consumer1;
		delete consumer2;

		listener.stop();
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
    }

	
	return 0;
}
