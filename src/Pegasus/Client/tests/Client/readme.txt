
How to run this test:
=====================

1.  Shut down the server (very important).

2.  Compile test.mof into the repository (this must be done when server is
    shut down).

    % cimmof test.mof

3.  Start the server (in another window).

4.  Run this test.

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
