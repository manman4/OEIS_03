\\ E.g.f. A(x) satisfies A(x) = ( exp(-x*A(x)^(1/2)) + x*A(x) )^2.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A^(1/2) + x*O(x^n)) + x*A)^2 ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A377859.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A^(1/2) + x*O(x^n)) + x*A)^2 ); Vec(serlaplace(A^(1/2)))


