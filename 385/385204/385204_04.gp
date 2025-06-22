M=25;

\\ G.f. A(x) satisfies A(x) = ( 1 + 25*x*A(x)^2 )^(1/5).
my(A=1, n=M); for(i=1, n, A=( 1 + 25*x*A^2 )^(1/5) +x*O(x^n) ); Vec(A)

\\ G.f.: 1/B(-x), where B(x) is the g.f. of A385203.
my(A=1, n=M); for(i=1, n, A=( 1 + 25*x*A^2 )^(1/5) +x*O(x^n) ); Vec(1/subst(A, x, -x))

