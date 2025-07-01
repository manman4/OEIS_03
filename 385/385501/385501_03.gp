\\ E.g.f. A(x) satisfies A(x) = exp( arctanh(x*A(x)) ).
my(A=1, n=20); for(i=1, n, A= exp( atanh(x*A + x*O(x^n)) )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = sqrt( (1+x*A(x))/(1-x*A(x)) ).
my(A=1, n=20); for(i=1, n, A= sqrt( (1+x*A)/(1-x*A) + x*O(x^n) )); Vec(serlaplace(A))

