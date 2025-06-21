M=25;

\\ G.f. A(x) satisfies A(x) = ( 1 + 49*x*A(x) )^(1/7).
my(A=1, n=M); for(i=1, n, A=( 1 + 49*x*A )^(1/7) +x*O(x^n) ); Vec(A)

\\ G.f.: 1/B(-x), where B(x) is the g.f. of A385206.
my(A=1, n=M); for(i=1, n, A=( 1 + 49*x*A )^(1/7) +x*O(x^n) ); Vec(1/subst(A, x, -x))

