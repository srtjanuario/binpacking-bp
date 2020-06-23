#include <ilcplex/ilocplex.h>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stack>
using namespace std;

#define EPSILON 1e-6

// Read data from filename
static void readData(const char *filename, IloNum &binCapacity, IloNumArray &itemWeight);
static void masterDebug(IloCplex &binPackingSolver, IloNumVarArray Lambda, IloRangeArray Fill);
static void subDebug(IloAlgorithm &patSolver, IloNumVarArray Use, IloObjective obj);
static void resultDebug(IloCplex &binPackingSolver, IloNumVarArray Lambda);

struct Node
{
   vector< pair<int, int> > Separate;  
   vector< pair<int, int> > Together;  
	bool isTogether; 
   // bool is_root;
	double bound; 
};



int GUB = INT_MAX-2;
void profundidade(Node &no);
stack<Node> arvoreDFS;

int main(int argc, char **argv)
{
   /**
    * User env to manage memory and identify modeling objects.
    * */
   IloEnv env;
   try
   {
      /**
       * Stores the capacity of my bin
       * */
      IloNum binCapacity;

      /**
       * Stores the weight of each item
       *  */
      IloNumArray itemWeight(env);

      /**
       * Read the input data from the data file passed as argument to the program
       * */
      if (argc > 1)
         readData(argv[1], binCapacity, itemWeight);
      else
         throw invalid_argument("Plese, give me an input file");

      IloInt nItens = itemWeight.getSize();

      int *x = new int[nItens];
      int *p = new int[nItens];
      int *w = new int[nItens];

      for (int i = 0; i < nItens; i++)
         w[i] = itemWeight[i];

      /**
       * Declare:
       * - array of variables
       * - range of constraints
       * - an objective function
       * - a model for the master problem
       * */
      IloNumVarArray Lambda(env, nItens, 0, IloInfinity);
      IloRangeArray Fill(env);

      IloModel masterBinPacking(env);

      /**
       * Define variables
       * */
      for (int i = 0; i < nItens; i++)
         Lambda[i].setName(("L_" + to_string(i)).c_str());

      /**
       * Define range
       * */
      for (int i = 0; i < nItens; i++)
         Fill.add(Lambda[i] == 1); // ? ? ?
      masterBinPacking.add(Fill);

      /**
       * Define objective function
       * */
      IloObjective binsUsed = IloAdd(masterBinPacking, IloMinimize(env, IloSum(Lambda)));

      IloCplex binPackingSolver(masterBinPacking);
      binPackingSolver.setOut(env.getNullStream());

      /// PATTERN-GENERATION PROBLEM ///

      IloModel patGen(env);

      IloObjective ReducedCost = IloAdd(patGen, IloMinimize(env, 1));
      IloNumVarArray Use(env, nItens, 0.0, 1, ILOINT);
      patGen.add(IloScalProd(itemWeight, Use) <= binCapacity);

      IloCplex patSolver(patGen);
      patSolver.setOut(env.getNullStream());

      /// COLUMN-GENERATION PROCEDURE ///
      IloNumArray price(env, nItens);
      IloNumArray newPatt(env, nItens);


      ////////////
      //// BB ////
      ////////////
      clock_t beginC, endC;

      GUB++;
      Node sol3;
      beginC = clock();
      profundidade(sol3);
      endC = clock();
      cout << "Optimal -> ";
      printNode(sol3);
      cout << double(endC - beginC) / CLOCKS_PER_SEC << " seconds" << endl;

      ////////////
      //// BB ////
      ////////////
      
      while (true)
      {
         /// OPTIMIZE OVER CURRENT PATTERNS ///

         binPackingSolver.solve();
         // masterDebug (binPackingSolver, Lambda, Fill);

         /// FIND AND ADD A NEW PATTERN ///
         for (int i = 0; i < nItens; i++)
            price[i] = -binPackingSolver.getDual(Fill[i]);

         ReducedCost.setLinearCoefs(Use, price);

         patSolver.solve();
         // subDebug (patSolver, Use, ReducedCost);

         if (patSolver.getValue(ReducedCost) > -EPSILON) break;
         
         patSolver.getValues(newPatt, Use);

         Lambda.add(IloNumVar(binsUsed(1) + Fill(newPatt)));
      }

      masterBinPacking.add(IloConversion(env, Lambda, ILOINT));

      binPackingSolver.solve();
      binPackingSolver.setOut(cout);
      cout << "Solution status: " << binPackingSolver.getStatus() << endl;
      resultDebug(binPackingSolver, Lambda);

      delete[] x;
      delete[] p;
   }
   catch (IloException &ex)
   {
      cerr << "Error: " << ex << endl;
   }
   catch (const std::invalid_argument &ia)
   {
      cerr << "Error: " << ia.what() << endl;
   }
   catch (...)
   {
      cerr << "Error" << endl;
   }

   env.end();
   return 0;
}

static void readData(const char *filename, IloNum &binCapacity,
                     IloNumArray &itemWeight)
{
   ifstream in(filename);
   if (in)
   {
      int quantity;
      in >> quantity;
      in >> binCapacity;
      int weight;
      for (int i = 0; i < quantity; i++)
      {
         in >> weight;
         itemWeight.add(weight);
      }
   }
   else
   {
      cerr << "No such file: " << filename << endl;
      throw(1);
   }
}

static void masterDebug(IloCplex &binPackingSolver, IloNumVarArray Lambda,
                        IloRangeArray Fill)
{
   cout << endl;
   cout << "Using " << binPackingSolver.getObjValue() << " bins" << endl;
   cout << endl;
   for (int64_t j = 0; j < Lambda.getSize(); j++)
      cout << "  Lambda" << j << " = " << binPackingSolver.getValue(Lambda[j]) << endl;

   cout << endl;
   for (int i = 0; i < Fill.getSize(); i++)
      cout << "  Fill" << i << " = " << binPackingSolver.getDual(Fill[i]) << endl;

   cout << endl;
}

static void subDebug(IloAlgorithm &patSolver, IloNumVarArray Use,
                     IloObjective obj)
{
   // cout << endl;
   cout << "Reduced cost is " << patSolver.getValue(obj) << endl;
   cout << endl;
   // if (patSolver.getValue(obj) <= 0)
   // {
   //    for (IloInt i = 0; i < Use.getSize(); i++)

   //       cout << "  Use" << i << " = " << patSolver.getValue(Use[i]) << endl;

   //    cout << endl;
   // }
}

static void resultDebug(IloCplex &binPackingSolver, IloNumVarArray Lambda)
{
   cout << endl;
   cout << "Best integer solution uses "
        << binPackingSolver.getObjValue() << " bins" << endl;
   cout << endl;
   // for (IloInt j = 0; j < Lambda.getSize(); j++)
   // {
   //    cout << "  Lambda" << j << " = " << binPackingSolver.getValue(Lambda[j]) << endl;
   // }
}

void profundidade(Node &no)
{
	calcularSolucao(no);
	escolherSubtour(no);
	arvoreDFS.push(no);

	Node nutella;
	Node n;
	pair<int, int> arco_proibido;

	while (!arvoreDFS.empty())
	{
		nutella = arvoreDFS.top();
		if(!(nutella.bound < GUB)){
			arvoreDFS.pop();
			continue;
		}
		n.arcos_proibidos = nutella.arcos_proibidos;
		arco_proibido.first = nutella.subtour[nutella.escolhido][nutella.index];
		arco_proibido.second = nutella.subtour[nutella.escolhido][nutella.index + 1];
		n.arcos_proibidos.push_back(arco_proibido);
		calcularSolucao(n);
		escolherSubtour(n);

		nutella.index++;
		arvoreDFS.pop();
		
		if (nutella.index < nutella.subtour[nutella.escolhido].size()-1)
			arvoreDFS.push(nutella);
		
		if (n.bound < GUB)
		{
			arvoreDFS.push(n); //inserir novos nos na arvore
			if (n.subtour.size() == 1)
			{
				printNode(n);
				GUB = n.bound;
				no = n;
				arvoreDFS.pop();
				prune(arvoreDFS, GUB);
			}
		}
	}
}
