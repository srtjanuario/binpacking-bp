#ifndef MINKNAP_BY_DAVID_PISINGER
#define MINKNAP_BY_DAVID_PISINGER
#include <string>
using namespace std;

/* ======================================================================
           MINKNAP.C, David Pisinger   march 1993, revised feb. 1998
           Converted to C++ by https://github.com/januarioccp
   ====================================================================== */

/* This is the C++ code corresponding to the paper:
 *
 *   D. Pisinger
 *   A minimal algorithm for the 0-1 knapsack problem
 *   Operations Research, 45, 758-767 (1997).
 *
 * Further details on the project can also be found in
 *
 *   D. Pisinger
 *   Algorithms for Knapsack Problems
 *   Report 95/1, DIKU, University of Copenhagen
 *   Universitetsparken 1
 *   DK-2100 Copenhagen
 *
 * The algorithm may be used for academic, non-commercial purposes 
 * only.
 * -------------------------------------------------------------------
 * The present code is a callable routine which solves a 0-1 Knapsack
 * Problem:
 *
 *           maximize   \sum_{j=1}^{n} p_{j} x_{j}
 *           subject to \sum_{j=1}^{n} w_{j} x_{j} \leq c
 *                      x_{j} \in \{0,1\}, j = 1,\ldots,n
 *
 * The minknap algorithm is called as
 *
 *          z = minknap(n, p, w, x, c)
 * 
 * where p[], w[], x[] are arrays of integers. The optimal objective
 * value is returned in z, and x[] gives the solution vector.
 * If you need a different interface for your algorithm, minknap
 * may easily be adapted to your own datastructures since all tables
 * are copied to the internal representation. 
 *
 * Since the minknap algorithm is based on dynamic programming, you
 * must give an upper limit MAXSTATES on the number of states. The 
 * constant is defined below. Different types should be defined as
 * follows:
 * 
 *    itype     should be sufficiently large to hold a profit or weight
 *    stype     should be sufficient to hold sum of profits/weights
 *    ptype     should hold the product of an stype and itype
 * 
 * The code has been tested on a hp9000/735, and conforms with the
 * ANSI-C standard. 
 * 
 * Errors and questions are refered to:
 *
 *   David Pisinger, associate professor
 *   DIKU, University of Copenhagen,
 *   Universitetsparken 1,
 *   DK-2100 Copenhagen.
 *   e-mail: pisinger@diku.dk
 *   fax: +45 35 32 14 01
 * 
 *  OR
 * 
 *  https://github.com/januarioccp
 */


/* ======================================================================
                                  definitions
   ====================================================================== */

#define MAXSTATES 400000 

/* ======================================================================
				   macros
   ====================================================================== */

#define SYNC            5      /* when to switch to linear scan in bins */
#define SORTSTACK     200      /* depth of stack used in qsort */
#define MINMED        100      /* find exact median in qsort if larger size */

#define TRUE  1
#define FALSE 0

#define LEFT  1
#define RIGHT 2

#define PARTIATE 1
#define SORTALL  2

#define MAXV (8*sizeof(btype)) /* number of bits in a long integer */
#define PMAX 1                 /* profit of worlds most efficient item  */
#define WMAX 0                 /* weight of worlds most efficient item  */
#define PMIN 0                 /* profit of worlds least efficient item */
#define WMIN 1                 /* weight of worlds least efficient item */

#define DET(a1, a2, b1, b2)    ((a1) * (ptype) (b2) - (a2) * (ptype) (b1))
#define SWAP(a, b)   { item t; t = *(a); *(a) = *(b); *(b) = t; }
#define DIFF(a,b)              ((int) ((b)-(a)+1))
#define NO(a,p)                ((int) ((p) - (a)->fitem + 1))
#define N(a,p)                 ((int) ((p) - (a)->d.set1))
#define L(x)                   ((long) (x))
#define SZ(a)                  (*(((int *) (a)) - 4) - 1)


/* ======================================================================
				 type declarations
   ====================================================================== */

typedef int           boolean;
typedef long          ntype;   /* number of states/items   */
typedef long        itype;   /* item profits and weights */
typedef long        stype;   /* sum of pofit or weight   */
typedef long double        ptype;   /* product type (sufficient precision) */
typedef unsigned long btype;   /* binary representation of solution */

/* item record */
typedef struct irec {
  itype   p;     /* profit */
  itype   w;     /* weight */
  boolean *x;    /* solution variable */
} item;

typedef struct { /* i-stack */
  item  *f;      /* first item in interval */
  item  *l;      /* last item in interval */
} interval;

/* state in dynamic programming */
typedef struct pv {
  stype psum;    /* profit sum */
  stype wsum;    /* weight sum */
  btype vect;    /* solution vector */
} state;

/* set of states */
typedef struct pset {
  ntype size;    /* set size */
  state *fset;   /* first element in set */
  state *lset;   /* last element in set */
  state *set1;   /* first element in array */
  state *setm;   /* last element in array */
} stateset;

typedef struct { /* all problem information */
  ntype    n;               /* number of items         */
  item     *fitem;          /* first item in problem   */
  item     *litem;          /* last item in problem    */
  item     *ftouch;         /* first item considered for reduction */
  item     *ltouch;         /* last item considered for reduction */
  item     *s;              /* current core is [s,t]   */
  item     *t;              /*                         */
  item     *b;              /* break item              */
  item     *fpart;          /* first item returned by partial sort */
  item     *lpart;          /* last item returned by partial sort */
  stype    wfpart;          /* weight sum up to fpart  */
  item     *fsort;          /* first sorted item       */
  item     *lsort;          /* last sorted item        */
  stype    wfsort;          /* weight sum up to fsort  */
  stype    c;               /* current capacity        */
  stype    cstar;           /* origianl capacity       */
  stype    z;               /* current solution        */
  stype    zstar;           /* optimal solution        */
  stype    zwsum;           /* weight sum of zstar     */
  itype    ps, ws, pt, wt;  /* items for deriving bounds */

  btype    vno;             /* current vector number   */
  item *   vitem[MAXV];     /* current last MAXV items */
  item *   ovitem[MAXV];    /* optimal set of items    */
  btype    ovect;           /* optimal solution vector */

  stype    dantzig;         /* dantzig upper bound     */
  stype    ub;              /* global upper bound      */
  stype    psumb;           /* profit sum up to b      */
  stype    wsumb;           /* weight sum up to b      */
  boolean  firsttime;       /* used for restoring x    */
  boolean  welldef;         /* is x welldefined        */
  stateset  d;              /* set of partial vectors  */
  interval *intv1, *intv2;
  interval *intv1b, *intv2b;

  /* debug */
  long     iterates;        /* counters used to obtain specific */
  long     simpreduced;     /* information about the solution process */
  long     pireduced;
  long     pitested;
  long     maxstates;
  long     coresize;
  long     bzcore;
} allinfo;


/* ======================================================================
				  errorx
   ====================================================================== */

void errorx(string str,...);


/* ======================================================================
				  palloc
   ====================================================================== */

void pfree(void *p);

/* ======================================================================
				  findvect
   ====================================================================== */

state *findvect(stype ws, state *f, state *l);


/* ======================================================================
				push/pop
   ====================================================================== */

void push(allinfo *a, int side, item *f, item *l);

void pop(allinfo *a, int side, item **f, item **l);


/* ======================================================================
				improvesolution
   ====================================================================== */

void improvesolution(allinfo *a, state *v);


/* ======================================================================
				definesolution
   ====================================================================== */

void definesolution(allinfo *a);

/* ======================================================================
				median
   ====================================================================== */

item *median(item *f1, item *l1, ntype s);


/* ======================================================================
				partsort
   ====================================================================== */

void partsort(allinfo *a, item *f, item *l, stype ws, int what);


/* ======================================================================
				  haschance
   ====================================================================== */

boolean haschance(allinfo *a, item *i, int side);


/* ======================================================================
				  multiply
   ====================================================================== */

void multiply(allinfo *a, item *h, int side);


/* =========================================================================
				   simpreduce
   ========================================================================= */

void simpreduce(int side, item **f, item **l, allinfo *a);


/* ======================================================================
				  reduceset
   ====================================================================== */

void reduceset(allinfo *a);


/* ======================================================================
				  initfirst
   ====================================================================== */

void initfirst(allinfo *a, stype ps, stype ws);


/* ======================================================================
				  initvect
   ====================================================================== */

void initvect(allinfo *a);


/* ======================================================================
				  copyproblem
   ====================================================================== */

void copyproblem(item *f, item *l, int *p, int *w, int *x);


/* ======================================================================
				findbreak
   ====================================================================== */

void findbreak(allinfo *a);


/* ======================================================================
				minknap
   ====================================================================== */

stype minknap(int n, int *p, int *w, int *x, int c);

/* ======================================================================
				end
   ====================================================================== */

#endif