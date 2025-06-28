\\ E.g.f. A(x) satisfies A(x) = exp( arcsinh(x) * A(x) ).
my(A=1, n=20); for(i=1, n, A= exp( asinh(x + x*O(x^n)) * A )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = ( x + sqrt(x^2 + 1) )^A(x).
my(A=1, n=20); for(i=1, n, A= (x + sqrt(x^2 + 1))^A); Vec(serlaplace(A))