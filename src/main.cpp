#include <ilcplex/ilocplex.h>
#include "data.h"
#include "master.h"
#include "price.h"
#include "column.h"

int main(int argc, char **argv)
{
   try
   {
      IloEnv env;
      Data input(argc, argv, &env);
      Master m(&input);
      Price p(&input);
      Column c(&input,&m,&p);
      c.solve();
      c.result();
      return 0;
   }
   catch (IloException &ex)
   {
      cerr << "Error: " << ex << endl;
   }
   catch (const std::invalid_argument &ia)
   {
      cerr << "Error: " << ia.what() << endl;
   }
   catch (const std::invalid_argument &ia)
   {
      cerr << "Invalid argument: " << ia.what() << endl;
   }
   catch (...)
   {
      cerr << "Error" << endl;
   }

   return 0;
}
