//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Common/ObjectNormalizer.h>

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//
// Clock
//
// Local object used to measure the performance of normalization for the test
// cases defined in this file.
//
class _Clock
{
public:
    _Clock(void) : _start(0), _stop(0), _total(0)
    {
    }

    inline void start(void)
    {
        _start = TimeValue::getCurrentTime().toMilliseconds();
    }

    inline void stop(void)
    {
        _stop = TimeValue::getCurrentTime().toMilliseconds();;

        // update total
        _total += double(_stop - _start);
    }

    inline void reset(void)
    {
        _start = 0;
        _stop = 0;

        _total = 0;
    }

    inline double getElapsed(void)
    {
        return(_total);
    }

private:
    Uint32 _start;
    Uint32 _stop;

    double _total;

} _clock;

static char * verbose = 0;
static CIMRepository * repository = 0;

//
// Basic ObjectNormalizer tests
//

// test improper ObjectNormalizer seeding
void Test001a(void)
{
    cout << "Test001a" << endl << endl;

    CIMClass cimClass;

    ObjectNormalizer normalizer(
        cimClass,
        false,
        false,
        false);

    normalizer.processClassObjectPath(CIMObjectPath());

    normalizer.processInstanceObjectPath(CIMObjectPath());

    normalizer.processInstance(CIMInstance());
}

// test bad arguments
void Test001b(void)
{
    cout << "Test001b" << endl << endl;

    ObjectNormalizer normalizer(
        CIMClass("CIM_ManagedElement"),
        false,
        false,
        false);

    try
    {
        normalizer.processClassObjectPath(CIMObjectPath());

        cout << "Failed to detect null class object path." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        cout << "CIMException: " << e.getMessage() << endl;
    }

    try
    {
        normalizer.processInstanceObjectPath(CIMObjectPath());

        cout << "Failed to detect null instance object path." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        cout << "CIMException: " << e.getMessage() << endl;
    }

    try
    {
        normalizer.processInstance(CIMInstance());

        cout << "Failed to detect null instance." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        cout << "CIMException: " << e.getMessage() << endl;
    }
}

//
// object path failures
//

// class object path (null object path)
void Test002a(void)
{
    cout << "Test002a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    cout << "cimClass object path = " << cimClass.getPath().toString() << endl;

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    try
    {
        _clock.start();

        CIMObjectPath normalizedObjectPath = normalizer.processClassObjectPath(cimObjectPath);

        _clock.stop();

        cout << "Failed to detect a null class object path." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// class object path (incorrect class name)
void Test002b(void)
{
    cout << "Test002b" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    cout << "cimClass object path = " << cimClass.getPath().toString() << endl;

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("CIM_SoftwareFeatureBAD");  // use different case to test that too

    try
    {
        _clock.start();

        CIMObjectPath normalizedObjectPath = normalizer.processClassObjectPath(cimObjectPath);

        _clock.stop();

        cout << "Failed to detect class object path with incorrect class name." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object path (incorrect class name)
void Test002c(void)
{
    cout << "Test002c" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    cout << "cimClass object path = " << cimClass.getPath().toString() << endl;

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("CIM_SoftwareFeatureBAD");  // use different case to test that too

    try
    {
        _clock.start();

        CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

        _clock.stop();

        cout << "Failed to detect instance object path with incorrect class name." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object path (no key properties in instance, no keys in object path)
void Test002d(void)
{
    cout << "Test002d" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    cout << "cimClass object path = " << cimClass.getPath().toString() << endl;

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName(cimClass.getClassName());

    try
    {
        _clock.start();

        CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

        _clock.stop();

        cout << "Failed to detect instance object path with no key properties and no keys." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

//
// object path successes
//

// class object path (normal)
void Test003a(void)
{
    cout << "Test003a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    cout << "cimClass object path = " << cimClass.getPath().toString() << endl;

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName(cimClass.getClassName());

    _clock.start();

    CIMObjectPath normalizedObjectPath = normalizer.processClassObjectPath(cimObjectPath);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedObjectPath.toString() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// class object path (with erroneous and extra information)
void Test003b(void)
{
    cout << "Test003b" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("cim_softwarefeature");  // use lowercase. normalization should fix case

    // fake keys
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("FakeProperty1", CIMValue(String("junk"))));
    keys.append(CIMKeyBinding("FakeProperty2", CIMValue(String("more junk"))));

    cimObjectPath.setKeyBindings(keys);

    _clock.start();

    CIMObjectPath normalizedObjectPath = normalizer.processClassObjectPath(cimObjectPath);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedObjectPath.toString() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object path (normal)
void Test003c(void)
{
    cout << "Test003c" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("cim_softwarefeature");  // use lowercase. normalization should fix case

    // simple keys
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instance #003c"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    cimObjectPath.setKeyBindings(keys);

    _clock.start();

    CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedObjectPath.toString() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object path (with erroneous and missing information)
void Test003d(void)
{
    cout << "Test003d" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    // give vendor a default value for this test
    cimClass.getProperty(cimClass.findProperty("Vendor")).setValue(CIMValue(String("DefaultVendor")));
    cimClass.getProperty(cimClass.findProperty("Name")).setValue(CIMValue(String("DefaultName")));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("cim_softwarefeature");

    // keys
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    //keys.append(CIMKeyBinding("Name", CIMValue(String("Test Intance #003d"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    //keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    cimObjectPath.setKeyBindings(keys);

    _clock.start();

    CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedObjectPath.toString() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object path (with erroneous and extra information)
void Test003e(void)
{
    cout << "Test003e" << endl <<endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName("cim_softwarefeature");  // use lowercase. normalization should fix case

    // simple keys
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instance #003e"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    // fake keys
    keys.append(CIMKeyBinding("FakeProperty1", CIMValue(String("junk"))));
    keys.append(CIMKeyBinding("FakeProperty2", CIMValue(String("more junk"))));

    cimObjectPath.setKeyBindings(keys);

    _clock.start();

    CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedObjectPath.toString() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

//
// instance object failures
//

// instance object with no properties and no object path
void Test004a(void)
{
    cout << "Test004a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // no properties specified

    // no object path specified

    try
    {
        _clock.start();

        CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

        _clock.stop();

        cout << "Failed to dected instance with no properties and no object path." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance object with incorrect property type
void Test004b(void)
{
    cout << "Test004b" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // only populate keys, let the normalizer do the rest
    cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));
    cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #004b"))));
    cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));
    cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));
    cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));

    // incorrect property type
    cimInstance.addProperty(CIMProperty("ElementName", CIMValue(Uint32(0))));   // should be String

    // no object path specified

    try
    {
        _clock.start();

        CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

        _clock.stop();

        cout << "Failed to detect incorrect property type." << endl;

        throw 0;
    }
    catch(CIMException & e)
    {
        _clock.stop();

        cout << "CIMException: " << e.getMessage() << endl;
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

//
// instance object successes
//

// instance with key properties but no object path
void Test005a(void)
{
    cout << "Test005a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // only populate keys, let the normalizer do the rest
    cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));
    cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #005a"))));
    cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));
    cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));
    cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));

    // no object path specified

    _clock.start();

    CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedInstance.getPath().toString() << endl;

        XmlWriter::printInstanceElement(normalizedInstance);
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance with object path but no properties
void Test005b(void)
{
    cout << "Test005b" << endl <<endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // all properties
    cimInstance.addProperty(CIMProperty("Caption", CIMValue(String("Software Feature"))));
    cimInstance.addProperty(CIMProperty("Description", CIMValue(String("Test Software Feature"))));
    cimInstance.addProperty(CIMProperty("ElementName", CIMValue(String("Test Instance #005b"))));
    cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));   // key
    cimInstance.addProperty(CIMProperty("InstanceDate", CIMValue(CIMDateTime::getCurrentDateTime())));
    cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #005b"))));
    cimInstance.addProperty(CIMProperty("OperationalStatus", CIMValue(Array<Uint16>())));
    cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));  // key
    cimInstance.addProperty(CIMProperty("Status", CIMValue(String("Status"))));
    cimInstance.addProperty(CIMProperty("StatusDescriptions", CIMValue(Array<String>())));
    cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));    // key
    cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));   // key

    // complete object path
    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName(cimInstance.getClassName());

    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instance #005b"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    cimObjectPath.setKeyBindings(keys);

    cimInstance.setPath(cimObjectPath);

    _clock.start();

    CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedInstance.getPath().toString() << endl;

        XmlWriter::printInstanceElement(normalizedInstance);
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// instance with local properties only
void Test005c(void)
{
    cout << "Test005c" << endl <<endl;

    _clock.reset();

    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // all properties
    cimInstance.addProperty(CIMProperty("Caption", CIMValue(String("Software Feature"))));
    cimInstance.addProperty(CIMProperty("Description", CIMValue(String("Test Software Feature"))));
    cimInstance.addProperty(CIMProperty("ElementName", CIMValue(String("Test Instance #005c"))));
    cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));   // key
    cimInstance.addProperty(CIMProperty("InstanceDate", CIMValue(CIMDateTime::getCurrentDateTime())));
    cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #005c"))));
    cimInstance.addProperty(CIMProperty("OperationalStatus", CIMValue(Array<Uint16>())));
    cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));  // key
    cimInstance.addProperty(CIMProperty("Status", CIMValue(String("Status"))));
    cimInstance.addProperty(CIMProperty("StatusDescriptions", CIMValue(Array<String>())));
    cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));    // key
    cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));   // key

    // complete object path
    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName(cimInstance.getClassName());

    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instance #005c"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    cimObjectPath.setKeyBindings(keys);

    cimInstance.setPath(cimObjectPath);

    _clock.start();

    CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedInstance.getPath().toString() << endl;

        XmlWriter::printInstanceElement(normalizedInstance);
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// include with properties containing class origin
void Test005d(void)
{
    cout << "Test005d" << endl <<endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = true;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    CIMInstance cimInstance(cimClass.getClassName());

    // no properties
    // all properties
    cimInstance.addProperty(CIMProperty("Caption", CIMValue(String("Software Feature"))));
    cimInstance.addProperty(CIMProperty("Description", CIMValue(String("Test Software Feature"))));
    cimInstance.addProperty(CIMProperty("ElementName", CIMValue(String("Test Instance #005d"))));
    cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));   // key
    cimInstance.addProperty(CIMProperty("InstanceDate", CIMValue(CIMDateTime::getCurrentDateTime())));
    cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #005d"))));
    cimInstance.addProperty(CIMProperty("OperationalStatus", CIMValue(Array<Uint16>())));
    cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));  // key
    cimInstance.addProperty(CIMProperty("Status", CIMValue(String("Status"))));
    cimInstance.addProperty(CIMProperty("StatusDescriptions", CIMValue(Array<String>())));
    cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));    // key
    cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));   // key

    // complete object path
    CIMObjectPath cimObjectPath;

    cimObjectPath.setClassName(cimInstance.getClassName());

    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
    keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instnace #005d"))));
    keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
    keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
    keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

    cimObjectPath.setKeyBindings(keys);

    cimInstance.setPath(cimObjectPath);

    _clock.start();

    CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

    _clock.stop();

    if(verbose)
    {
        cout << normalizedInstance.getPath().toString() << endl;

        XmlWriter::printInstanceElement(normalizedInstance);
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

//
// Test100a()
//
void Test100a(void)
{
    cout << "Test100a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "cim_softwarefeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    for(Uint32 i = 0, n = 100; i < n; i++)
    {
        CIMObjectPath cimObjectPath;

        cimObjectPath.setClassName(cimClass.getClassName());

        // simple keys
        Array<CIMKeyBinding> keys;

        keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
        keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instance #100a.") + CIMValue(i).toString())));
        keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
        keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
        keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

        cimObjectPath.setKeyBindings(keys);

        _clock.start();

        CIMObjectPath normalizedObjectPath = normalizer.processInstanceObjectPath(cimObjectPath);

        _clock.stop();

        if(verbose)
        {
            cout << normalizedObjectPath.toString() << endl;
        }
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// large instance set with object paths set and without qualifiers
void Test101a(void)
{
    cout << "Test101a" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    for(Uint32 i = 0, n = 100; i < n; i++)
    {
        CIMInstance cimInstance(cimClass.getClassName());

        // all properties
        cimInstance.addProperty(CIMProperty("Caption", CIMValue(String("Software Feature"))));
        cimInstance.addProperty(CIMProperty("Description", CIMValue(String("Test Software Feature"))));
        cimInstance.addProperty(CIMProperty("ElementName", CIMValue(String("Test Instance #101a.") + CIMValue(i).toString())));
        cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));   // key
        cimInstance.addProperty(CIMProperty("InstanceDate", CIMValue(CIMDateTime::getCurrentDateTime())));
        cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #101a.") + CIMValue(i).toString()))); // key
        cimInstance.addProperty(CIMProperty("OperationalStatus", CIMValue(Array<Uint16>())));
        cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));  // key
        cimInstance.addProperty(CIMProperty("Status", CIMValue(String("Status"))));
        cimInstance.addProperty(CIMProperty("StatusDescriptions", CIMValue(Array<String>())));
        cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));    // key
        cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));   // key

        // complete object path
        CIMObjectPath cimObjectPath;

        cimObjectPath.setClassName(cimInstance.getClassName());

        Array<CIMKeyBinding> keys;

        keys.append(CIMKeyBinding("IdentifyingNumber", CIMValue(String("AAAAA"))));
        keys.append(CIMKeyBinding("Name", CIMValue(String("Test Instnace #101a." + CIMValue(i).toString()))));
        keys.append(CIMKeyBinding("ProductName", CIMValue(String("TestObjectNormalizer"))));
        keys.append(CIMKeyBinding("Vendor", CIMValue(String("Pegasus"))));
        keys.append(CIMKeyBinding("Version", CIMValue(String("2.0"))));

        cimObjectPath.setKeyBindings(keys);

        _clock.start();

        CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

        _clock.stop();

        if(verbose)
        {
            cout << normalizedInstance.getPath().toString() << endl;

            XmlWriter::printInstanceElement(normalizedInstance);
        }
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

// large instance set without object paths and without qualifiers
void Test101b(void)
{
    cout << "Test101b" << endl << endl;

    _clock.reset();

    Boolean localOnly = false;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    CIMClass cimClass =
        repository->getClass(
            "root/cimv2",
            "CIM_SoftwareFeature",
            localOnly,
            true,   // always true. needed for key qualifiers
            localOnly ? true : includeClassOrigin,  // class origin is needed if local only is true
            CIMPropertyList());

    // ATTN: patch the class object path because the repository does not set the host or namespace elements.
    cimClass.setPath(CIMObjectPath("localhost", "root/cimv2", cimClass.getClassName()));

    ObjectNormalizer normalizer(
        cimClass,
        localOnly,
        includeQualifiers,
        includeClassOrigin);

    for(Uint32 i = 0, n = 100; i < n; i++)
    {
        CIMInstance cimInstance(cimClass.getClassName());

        // only populate keys, let the normalizer do the rest
        cimInstance.addProperty(CIMProperty("Caption", CIMValue(String("Software Feature"))));
        cimInstance.addProperty(CIMProperty("Description", CIMValue(String("Test Software Feature"))));
        cimInstance.addProperty(CIMProperty("ElementName", CIMValue(String("Test Instance #101b.") + CIMValue(i).toString())));
        cimInstance.addProperty(CIMProperty("IdentifyingNumber", CIMValue(String("AAAAA"))));   // key
        cimInstance.addProperty(CIMProperty("InstanceDate", CIMValue(CIMDateTime::getCurrentDateTime())));
        cimInstance.addProperty(CIMProperty("Name", CIMValue(String("Test Instance #101b.") + CIMValue(i).toString()))); // key
        cimInstance.addProperty(CIMProperty("OperationalStatus", CIMValue(Array<Uint16>())));
        cimInstance.addProperty(CIMProperty("ProductName", CIMValue(String("TestObjectNormalizer"))));  // key
        cimInstance.addProperty(CIMProperty("Status", CIMValue(String("Status"))));
        cimInstance.addProperty(CIMProperty("StatusDescriptions", CIMValue(Array<String>())));
        cimInstance.addProperty(CIMProperty("Vendor", CIMValue(String("Pegasus"))));    // key
        cimInstance.addProperty(CIMProperty("Version", CIMValue(String("2.0"))));   // key

        // no object path specified (obtained via instance)

        _clock.start();

        CIMInstance normalizedInstance = normalizer.processInstance(cimInstance);

        _clock.stop();

        if(verbose)
        {
            cout << normalizedInstance.getPath().toString() << endl;

            XmlWriter::printInstanceElement(normalizedInstance);
        }
    }

    cout << "*** " << _clock.getElapsed() << " milliseconds." << endl;
}

int main(int argc, char** argv)
{
    verbose = getenv("PEGASUS_TEST_VERBOSE");

    String repositoryRoot = String(getenv("PEGASUS_HOME")) + String("/repository");

    try
    {
        repository = new CIMRepository(repositoryRoot);
    }
    catch(...)
    {
        cout << "Failed to create CIMRepository object." << endl;
    }

	try
    {
        // basic object tests
        Test001a();
        Test001b();

        // object path failures
        Test002a();
        Test002b();
        Test002c();
        Test002d();

        // object path successes
        Test003a();
        Test003b();
        Test003c();
        Test003d();
        Test003e();

        // instance object failures
        Test004a();
        Test004b();

        // instance object successes
        Test005a();
        Test005b();
        Test005c();
        Test005d();

        // object path processing performance tests
        Test100a();

        // instance processing performance tests
        Test101a();
        Test101b();
    }
    catch(CIMException & e)
    {
        cout << "CIMException: " << e.getCode() << " " << e.getMessage() << endl;

        return(1);
    }
    catch(Exception & e)
    {
        cout << "Exception: " << e.getMessage() << endl;

        return(1);
    }
    catch(...)
    {
        cout << "unknown exception" << endl;

        return(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return(0);
}
