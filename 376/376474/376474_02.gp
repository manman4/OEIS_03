M=18;

\\ E.g.f. satisfies A(x) = exp( x^2*A(x)^2 / (1 - x*A(x)) ).
my(A=1, n=M); for(i=1, n, A=exp(x^2*A^2/(1 - x*A + x*O(x^n)))); Vec(serlaplace(A))

