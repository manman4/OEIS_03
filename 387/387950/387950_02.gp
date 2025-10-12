M=18;

\\ E.g.f. A(x) satisfies A(x) = exp( x^3*A(x)^3 / (1 - x*A(x))^2 ).
my(A=1, n=M); for(i=1, n, A=exp( x^3*A^3/(1 - x*A + x*O(x^n))^2 )); Vec(serlaplace(A))

