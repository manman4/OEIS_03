\\ E.g.f. A(x) satisfies A(x) = ( exp(-x*A(x)^(2/3)) + x*A(x) )^3.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A^(2/3) + x*O(x^n)) + x*A)^3 ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A379877.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A^(2/3) + x*O(x^n)) + x*A)^3 ); Vec(serlaplace(A^(1/3)))



