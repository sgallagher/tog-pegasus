#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLParser.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int
main (int argc, char *argv[])
{
   CIMClient client;

   try
   {
      client.connectLocal ();
   }
   catch (const Exception &e)
   {
      cerr << "Caught: " << e.getMessage () << endl;
   }

   Array<CIMInstance> instances;

   try
   {
      instances = client.enumerateInstances(
                      CIMNamespaceName("root/cimv2"),
                      CIMName ("PG_UnixProcess"),
                      true,  // deepInheritance
                      false, // localOnly
                      false, // includeQualifiers
                      true); // includeClassOrigin

      cout << "instances.size () = " << instances.size () << endl;
   }
   catch (const Exception &e)
   {
      cerr << "Caught: " << e.getMessage () << endl;
   }

   String queryLanguage = "WQL";
   String query=
       "SELECT CSName, OSName FROM PG_UnixProcess WHERE Handle = \"2037\"";
   WQLSelectStatement *stmt = new WQLSelectStatement(queryLanguage, query);

   try
   {
      WQLParser::parse (query, *stmt);
   }
   catch (const Exception &e)
   {
      cerr << "Caught: " << e.getMessage () << endl;
   }

   cout << "stmt->getQueryLanguage () " << stmt->getQueryLanguage () << endl;
   cout << "stmt->getQuery () " << stmt->getQuery () << endl;
   cout << "stmt->getAllProperties () " << stmt->getAllProperties () << endl;

   // public String getSelectString ()

   CIMPropertyList cplSelect = stmt->getSelectPropertyList ();

   cout << "cplSelect.size () = " << cplSelect.size () << endl;

   for (Uint32 i = 0; i < cplSelect.size (); i++)
   {
      cout << "cplSelect[" << i << "] = " << cplSelect[i].getString () << endl;
   }

   // public SelectList getSelectList ()
   // public FromExp getFromClause ()
   // public QueryExp getWhereClause ()

   CIMPropertyList cplWhere = stmt->getWherePropertyList ();

   cout << "cplWhere.size () = " << cplWhere.size () << endl;

   for (Uint32 i = 0; i < cplWhere.size (); i++)
   {
      cout << "cplWhere[" << i << "] = " << cplWhere[i].getString () << endl;
   }

   for (Uint32 i = 0; i < instances.size (); i++)
   {
//////cout << "evaluate ("
//////     << instances[i].getPath ().toString ()
//////     << ") = "
//////     << stmt->evaluate (instances[i])
//////     << endl;

      if (stmt->evaluate (instances[i]))
      {
         CIMInstance selectedInstance = instances[i].clone ();

         stmt->applyProjection (selectedInstance, false);

         cout << selectedInstance.getPath ().toString () << endl;
      }
   }

   stmt->print ();

   delete stmt;

   client.disconnect ();

   return 0;
}
