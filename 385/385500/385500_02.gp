\\ E.g.f. A(x) satisfies A(x) = exp( arctanh(x) * A(x) ).
my(A=1, n=20); for(i=1, n, A= exp( atanh(x + x*O(x^n)) * A )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = ( (1+x)/(1-x) )^(A(x)/2).
my(A=1, n=20); for(i=1, n, A= ( (1+x)/(1-x) + x*O(x^n) )^(A/2)); Vec(serlaplace(A))