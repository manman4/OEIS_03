M=17;

\\ G.f. A(x) satisfies A(x) = (1 + 9*x*A(x)^8)^(1/3).
my(A=1, n=M); for(i=1, n, A=( 1 + 9*x*A^8 )^(1/3) +x*O(x^n) ); Vec(A)

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^13).
my(A=1, n=M); for(i=1, n, A=( 1 + 9*x*A^8 )^(1/3) +x*O(x^n) ); Vec(A - 1/subst(A, x, -x*A^13))




