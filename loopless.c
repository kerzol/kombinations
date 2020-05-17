//  __     __    __  ____  __    ____  ____  ____
// (  )   /  \  /  \(  _ \(  )  (  __)/ ___)/ ___)
// / (_/\(  O )(  O )) __// (_/\ ) _) \___ \\___ \
// \____/ \__/  \__/(__)  \____/(____)(____/(____/
//   ___  ____  __ _  ____  ____   __  ____  __  ____     __  ____
//  / __)(  __)(  ( \(  __)(  _ \ / _\(_  _)/  \(  _ \   /  \(  __)
// ( (_ \ ) _) /    / ) _)  )   //    \ )( (  O ))   /  (  O )) _)
//  \___/(____)\_)__)(____)(__\_)\_/\_/(__) \__/(__\_)   \__/(__)
//   ___  __   _  _  ____  __  __ _   __  ____  __  __   __ _  ____
//  / __)/  \ ( \/ )(  _ \(  )(  ( \ / _\(_  _)(  )/  \ (  ( \/ ___)
// ( (__(  O )/ \/ \ ) _ ( )( /    //    \ )(   )((  O )/    /\___ \
//  \___)\__/ \_)(_/(____/(__)\_)__)\_/\_/(__) (__)\__/ \_)__)(____/

  // Loopless generation is a cool combinarial technique that
  // allows us to generate a combinarial object in constant time (sic!).
  //
  // A book "Combinatorial Generation" by Frank Ruskey is a gentle introduction.
  //
  // So, imagine you want to generate all combinarial objects of
  // certain kind.  With loopless algorithm the overall complexity is
  // linear with respect to the number of objects you need to
  // generate.  Note that it is very difficult to be faster, because
  // even to write or print n objects you need O(n) seconds.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef unsigned long long int Clique;
typedef unsigned short int Kvalue;
typedef unsigned int Node;
typedef struct {
	Kvalue k;
	Kvalue  z;
	Node n; //number of z-subcliques in a k-clique
	Kvalue* tab;//all z-cliques
} combination;

Node binom (Kvalue n, Kvalue k) {
  // Perhaps the simplest algo to compute a binomial coefficient !
  // See D.E. Knuth, The Art of_Computer_Programming - Volume 1, 3rd ed, page 55

  // It is ok for our purposes.
  // Not factorial, but the overflow is still possible
  // see https://stackoverflow.com/questions/1838368 for details
  if (k > n) return 0;
  if (k < n - k) k = n - k;
  Node r = 1;
  for (Node d = 1; d <= k; ++d) {
    r *= n--;
    r /= d;
  }
  return r;
}

void printBits(size_t const size, void const * const ptr)
{
  // from https://stackoverflow.com/questions/111928/
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  int i, j;

  for (i=size-1;i>=0;i--)
    {
      for (j=7;j>=0;j--)
        {
	  byte = (b[i] >> j) & 1;
	  printf("%u", byte);
        }
    }
  puts("");
}

// We need a bit per element + one additional bit. The following type
// is sufficient to store a combination (k,n) when n < 63.
typedef unsigned long long REG;
#define BITISET(var, position) ((var) & (1<<(position)))

void visit(REG bit_combination, Kvalue k, combination* combs) {
  //  printBits( sizeof(REG), &c);
  Kvalue ki = 0;
  Kvalue i = 0;
  for (i = 0; (ki < k) && (i  <  sizeof(REG) * 8) ; i++) {
    if ( BITISET(bit_combination, i) ) {
      combs->tab [combs->n * k + ki] = i;
      ki++;
    }
  }
  combs->n++;
}


void the_coolest (REG n, Kvalue k, combination* combs) {
  // See paper "The coolest way to generate combinations"
  // written by Frank Ruskey and Aaron Williams
  // Discrete Mathematics 309 (2009) 5305–5320

  REG t = k;
  REG s = n - t;
  REG R0, R1, R2, R3, R4;
  R1 = 0;
  R2 = 1 << (s + t);
  R3 = (1 << t) - 1;
  while ( (R3 & R2) == 0 ) {
    visit (R3, k, combs);
    R0 = R3 & (R3 + 1);
    R1 = R0 ^ (R0 - 1);
    R0 = R1 + 1;
    R1 = R1 & R3;

    // saturating substraction. WHAAAT ???
    R4 = (R0 & R3) - 1;
    R4 &= -(R4 <= (R0 & R3));
    R0 = R4;

    R3 = R3 + R1 - R0;
  }
}

// Here we generate all stuff and stock it into the memory.  The
// coolest algorithm could easily be modified to generate combination
// à la volée without storing them or their number.
combination* gen_comb_coolest (Kvalue k, Kvalue z) {
	combination* comb = malloc (sizeof (combination));
	comb->k = k;
	comb->z = z;
	comb->n = 0;
	comb->tab = malloc (binom(k, z) * z * sizeof (Kvalue));
	the_coolest(k, z, comb);
	return comb;
}



// Algorithm 5.7 from "Combinatorial Generation" book by Frank Ruskey
// We need some global vars
Kvalue* A = NULL;
short int J;
void next_comb() {
  // Non loops (sic)!
  if (J < 0) {
    A[0] = A[0] - 1;
    if (A[0] == 0) {
      J = J + 2;
    }
  } else if (A[J+1] == A[J] + 1) {
    A[J+1] = A[J];
    A[J] = J;
    if (A[J+1] == A[J] + 1) {
      J = J + 2;
    }
  } else {
    A[J] = A[J] + 1;
    if (J > 0) {
      A[J-1] = A[J] - 1;
      J = J - 2;
    }
  }
}
// Here we generate all stuff and stock it into the memory.  This
// algorithm could easily be modified to generate combinations à la
// volée without storing them or their number.
combination* gen_comb_algo57 (Kvalue k, Kvalue z) {
	combination* comb = malloc (sizeof (combination));
	comb->k = k;
	comb->z = z;
	comb->n = 0;
	comb->tab = malloc (binom(k, z) * z * sizeof (Kvalue));
	A = malloc (sizeof(Kvalue) * (z+1));
	for (Kvalue i = 0; i < z; i++) {
	  A[i] = i;
	}
	A[z] = k;
	J = z-1;
	while (A[z] >= k) {
	  memcpy (comb->tab + (comb->n * comb->z), A, comb->z * sizeof (Kvalue));
	  next_comb ();
          comb->n++;
	}
	free (A);
	return comb;
}

///
/// OLD CODE
///
Clique fact(Clique n) {
	if (n == 0 || n == 1)
		return 1;
	else
		return n * fact(n - 1);
}

Node nchoosek(Kvalue n, Kvalue k){
	return fact((Clique)n) / (fact((Clique)k) * fact((Clique)(n-k)));
}

void mkcomb_r(combination* comb, Node i, Node j) {
	Kvalue k;
	static Kvalue* data=NULL;

	if (data==NULL){
		data=malloc(comb->z*sizeof(Kvalue));
	}

	if (i == comb->z) {
		for (k=0; k<comb->z; k++)
			comb->tab[comb->n*comb->z+k]=data[k];
		comb->n++;
		return;
	}
	if (j >= comb->k)
		return;

	//j is included, put it and go next
	data[i] = j;
	mkcomb_r(comb, i + 1, j + 1);
	//j is excluded, do not put it and go next
	mkcomb_r(comb, i, j + 1);
}

combination* mkcomb(Kvalue k, Kvalue z){
	combination *comb=malloc(sizeof(combination));
	comb->k=k;
	comb->z=z;
	comb->n=0;
	comb->tab=malloc(nchoosek(k,z)*z*sizeof(Kvalue));
	mkcomb_r(comb, 0, 0);
	return comb;
}
//
// END OF OLD CODE
//


void print_combinations (combination* comb) {
  for (Node i = 0; i < comb->n; i++) {
    for (Kvalue j = 0; j < comb->z; j++) {
      printf("%d ",comb->tab [i * comb->z + j]);
    }
    printf("\n");
  }
}



// Just a function to mesure time
clock_t GLOBAL_LAST_TIME = 0;
void lap () {
  printf (" LAP TIME : %f \n",
	  (double)(clock() - GLOBAL_LAST_TIME) / CLOCKS_PER_SEC);
  GLOBAL_LAST_TIME = clock();
}

int main (int argc, char** argv) {
  if (argc < 3) {
    printf ("Usage : %s n k\n Test it like %s 5 3", argv[0], argv[0]);
    return 1;
  }

  // n is k
  // k is z
  // Easy ?
  // Niet !
  Kvalue k = atoi (argv [1]);
  Kvalue z = atoi (argv [2]);

  printf ("\n\n Count combinations, new vs old\n\n");

  lap ();
  printf ("Number of combinations (new method) : %d \n", binom (k, z));
  lap ();
  printf ("Number of combinations (old method) : %d \n", nchoosek (k, z));
  lap ();


  printf ("\n\n    Generate combinations, new vs old \n\n");

  printf ("\n****\n");
  printf ("New, \"The coolest\" method\n");
  printf ("Start generation\n");
  lap();
  combination* c = gen_comb_coolest (k, z);
  // DEBUG
  print_combinations (c);
  printf ("End generation, %d generated \n", c->n);
  lap ();

  printf ("\n****\n");
  printf ("New, Algorithm 5.7 method\n");
  printf ("Start generation \n");
  lap();
  c = gen_comb_algo57 (k, z);
  // DEBUG
  print_combinations (c);
  printf ("End generation, %d generated \n", c->n);
  lap();

  printf ("\n****\n");
  printf ("Old method \n");
  lap();
  c = mkcomb (k, z);
  // DEBUG
  print_combinations (c);
  printf ("End generation (old method), %d generated \n", c->n);
  lap();


}
