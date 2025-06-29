\\ E.g.f. A(x) satisfies A(x) = exp( arcsinh(x * A(x)^3) ).
my(A=1, n=20); for(i=1, n, A= exp( asinh(x * A^3 + x*O(x^n)) )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = (1 + 2*x*A(x)^4)^(1/2).
my(A=1, n=20); for(i=1, n, A= (1 + 2*x*A^4 + x*O(x^n))^(1/2)); Vec(serlaplace(A))