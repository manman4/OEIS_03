M=17;

\\ G.f. A(x) satisfies A(x) = (1 + 9*x*A(x)^3)^(2/3).
my(A=1, n=M); for(i=1, n, A=( 1 + 9*x*A^3 )^(2/3) +x*O(x^n) ); Vec(A)

\\ G.f.: B(x)^2, where B(x) is the g.f. of A008931.
my(A=1, n=M); for(i=1, n, A=( 1 + 9*x*A^3 )^(2/3) +x*O(x^n) ); Vec(A^(1/2))




