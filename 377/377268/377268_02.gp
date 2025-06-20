my(A=1, n=42); for(i=1, n, A=( 1 - 9*x*A )^(1/3) +x*O(x^n) ); Vec(A)

\\ G.f. A(x) satisfies A(x) = 1/A(-x/A(x)).
my(A=1, n=42); for(i=1, n, A=( 1 - 9*x*A )^(1/3) +x*O(x^n) ); Vec(1/subst(A, x, -x/A))
