my(A=1, n=42); for(i=1, n, A=1/( 1 - 9*x*A )^(2/3) +x*O(x^n) ); Vec(A)

\\ G.f.: B(x)^2, where B(x) is the g.f. of A245114.
my(A=1, n=42); for(i=1, n, A=1/( 1 - 9*x*A )^(2/3) +x*O(x^n) ); Vec(A^(1/2))

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^(7/2)).
my(A=1, n=42); for(i=1, n, A=1/( 1 - 9*x*A )^(2/3) +x*O(x^n) ); Vec(A - 1/subst(A, x, -x*A^(7/2)))

