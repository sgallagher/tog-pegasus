#include <iostream>
#include <cassert>
#include <Pegasus/Common/StatisticalData.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

int main(int argc, char** argv)
{


StatisticalData* sd = StatisticalData::current();
StatisticalData* curr = StatisticalData::current();
//cur = StatisticalData::current();

//check to make sure current() returns a pointer to the existing StatisticalData
// object

assert(sd->length == curr->length);
assert(sd->requestSize == curr->requestSize);
assert(sd->copyGSD == curr->copyGSD);
assert(sd->numCalls[5] == curr->numCalls[5]);
assert(sd->requestSize[6] == curr->requestSize[6]);


//  *****************************************
// check the addToValue() method

sd->addToValue(10,5,StatisticalData::SERVER); //changes sd.numCalls[5] from 0 to 10
sd->addToValue(10,6,StatisticalData::BYTES_READ); //changes sd.requestSize[6] form 0 to 10

//assert(sd->numCalls[5] == 0);
//assert(sd->requestSize[6] == 10);

//**********************************************
// check the setCopyGSD method

sd->setCopyGSD(1);

assert(sd->copyGSD == 1);

//****************************************************
// make sure the cur the sd objects are still the same

assert(sd->length == curr->length);
assert(sd->requestSize == curr->requestSize);
assert(sd->copyGSD == curr->copyGSD);
assert(sd->numCalls[5] == curr->numCalls[5]);
assert(sd->requestSize[6] == curr->requestSize[6]);


//**************************

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}

