#include "Service.h"

PEGASUS_NAMESPACE_BEGIN

bool Service::run(
    int argc,
    char** argv,
    const char* serviceName, 
    ServiceHandler* serviceHandler,
    bool detach)
{
    if (!detach)
    {
	serviceHandler->main(argc, argv);
	return false;
    }

    return false;
}

PEGASUS_NAMESPACE_END
