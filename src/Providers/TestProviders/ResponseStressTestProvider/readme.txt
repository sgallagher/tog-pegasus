This provider services the class TST_ResponseTestCxx.

This provider returns a defined number of instances of a defined
size to a request. It uses an input method (set) to define the size of
each object in the response and the number of responses to return.

The set method includes the following parameters:
    ResponseCount - Sets the number of instances to be returned
    Size - Sets the approximate size of each response instance.

The provider was defined  as a tool to push very large quantities of
large responses through the server as a way to stress the server while
knowing exactly the number and sizes returned.

The corresponding testclient is used primarily to confirm that the provider
code itself works, not as a stress test. We expect the stress tests
to be defined elsewhere in the Pegasus test source (ex. Pegasus/Client/tests).

For example, see the Clients/tests/pullop which used this provider
to compare results from enumerations and corresponding pull operations.


