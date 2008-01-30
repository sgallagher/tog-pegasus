#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Listener/CIMListener.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class MyIndicationConsumer : public CIMIndicationConsumer
{
public:
    MyIndicationConsumer ()
    {
    }

    ~MyIndicationConsumer ()
    {
    }

    void consumeIndication (const OperationContext& context,
                            const String&           url,
                            const CIMInstance&      indicationInstance)
    {
       Uint32 ui32Index = indicationInstance.findProperty ("InstanceId");
       Uint64 ui64Num = 0;

       if (ui32Index != PEG_NOT_FOUND)
       {
          try
          {
             indicationInstance.getProperty(ui32Index).getValue().get(ui64Num);
          }
          catch (Exception& e)
          {
          }
       }

       cout << "! "
            << url
            << " ! "
            << indicationInstance.getClassName ().getString ()
            << " ! "
            << (int)ui64Num
            << endl;
    }

private:
};

int main (int argc, char *argv[])
{
   Uint32                portNumber            = 2005;
   CIMListener           listener (portNumber);
   MyIndicationConsumer *consumer              = new MyIndicationConsumer ();

   // Add our consumer
   listener.addConsumer (consumer);

   // Finish starting the CIMListener
   try
   {
       cout << "+++++ Starting the CIMListener at destination\n"
            << "       http://localhost:2005/TestIndicationStressTest"
            << endl;

       // Start the listener
       listener.start ();
   }
   catch (BindFailedException &bfe)
   {
       // Got a bind error.  The port is probably already in use.
       // Put out a message and fail.
       cerr << endl
            << "==>WARNING: unable to bind to listener port 2005"
            << endl;
       cerr << "The listener port may be in use." << endl;

       throw;
   }

   cout << "Press any key to terminate..." << endl;

   cin.get ();

   listener.stop ();
   listener.removeConsumer (consumer);

   delete consumer;

   return 0;
}
