
How to run this test:
=====================

1.  This test requires that both the root/cimv2 and 
    root/SampleProvider namespaces have been populated.

    In particular, this test requires the existence
    of CIM_ManagedElement and CIM_ComputerSystem in root/cimv2.  
    It also requires that the class and instance definitions
    defined in pegasus/src/Providers/sample/Load/ClientTest.mof
    are loaded into the root/SampleProvider namespace.

    The Makefile in the pegasus directory can be 
    used to populate both namespaces.

    To populate the namespaces using cimmofl, use ...

    $(MAKE) repository
    $(MAKE) testrepository

    To populate the namespaces using cimmof, use ...

    $(MAKE) repositoryServer
    $(MAKE) testrepositoryServer

2.  Run this test.

    % Client

Here is the expected output:
============================

[//homefront/root/cimv2:Person.name="Sofi"]
[//homefront/root/cimv2:Person.name="Gabi"]

[//homefront/root/cimv2:Person.name="Sofi"]
[//homefront/root/cimv2:Person.name="Gabi"]

[//homefront/root/cimv2:Lineage.child="person.name=\"Sofi\"",parent="person.name=\"Mike\""]
[//homefront/root/cimv2:Lineage.child="person.name=\"Gabi\"",parent="person.name=\"Mike\""]

[//homefront/root/cimv2:Lineage.child="person.name=\"Sofi\"",parent="person.name
=\"Mike\""]
[//homefront/root/cimv2:Lineage.child="person.name=\"Gabi\"",parent="person.name
=\"Mike\""]

[//homefront/root/cimv2:Person]

[//homefront/root/cimv2:Lineage]
[//homefront/root/cimv2:LabeledLineage]

+++++ passed all tests
