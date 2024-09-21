\\ E.g.f. satisfies A(x) = exp( x*A(x)*(exp(x^2*A(x)^2) - 1) ).
my(A=1, n=19); for(i=1, n, A=exp(x*A*(exp(x^2*A^2 + x*O(x^n)) - 1))); Vec(serlaplace(A))

