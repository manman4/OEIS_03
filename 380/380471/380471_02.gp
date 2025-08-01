M=20;

\\ G.f. A(x) satisfies A(x) = 1/( 1 - 25*x*A(x)^4 )^(1/5).
my(A=1, n=M); for(i=1, n, A=1/( 1 - 25*x*A^4 )^(1/5) +x*O(x^n) ); Vec(A)

\\ G.f. A(x) satisfies A(x) = ( 1 + 25*x*A(x)^9 )^(1/5).
my(A=1, n=M); for(i=1, n, A=( 1 + 25*x*A^9 )^(1/5) +x*O(x^n) ); Vec(A)

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^13).
my(A=1, n=M); for(i=1, n, A=( 1 + 25*x*A^9 )^(1/5) +x*O(x^n) ); Vec(A - 1/subst(A, x, -x*A^13))

