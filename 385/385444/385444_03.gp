\\ E.g.f. A(x) satisfies A(x) = exp( (1/4) * arcsinh(4*x*A(x)) ).
my(A=1, n=20); for(i=1, n, A= exp( (1/4) * asinh(4*x*A + 4*x*O(x^n)) )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = (1 + 8*x*A(x)^5)^(1/8).
my(A=1, n=20); for(i=1, n, A= (1 + 8*x*A^5 + x*O(x^n))^(1/8)); Vec(serlaplace(A))