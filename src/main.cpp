#include <ilcplex/ilocplex.h>
#include "data.h"
#include "master.h"
#include "price.h"
#define EPSILON 1e-6

int main(int argc, char **argv)
{
   try
   {
      IloEnv env;
      Data input(argc, argv, &env);
      Master m(&input);
      Price p(&input);

      while (true)
      {
         m.solve();

         for (int i = 0; i < input.nItems(); i++)
            p.setDual(i, m.getDual(i));

         p.solve();

         if (p.reducedCost() > -EPSILON)
            break;
         else
            m.addColumn(p.newColumn());
      }

      m.solveIP();
      cout << "Solution status: " << m.getStatus() << endl;
      m.debug();
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
