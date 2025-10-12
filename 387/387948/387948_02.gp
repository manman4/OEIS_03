M=18;

\\ E.g.f. A(x) satisfies A(x) = exp( x^2*A(x)^2 / (1 - x*A(x))^2 ).
my(A=1, n=M); for(i=1, n, A=exp( x^2*A^2/(1 - x*A + x*O(x^n))^2 )); Vec(serlaplace(A))

