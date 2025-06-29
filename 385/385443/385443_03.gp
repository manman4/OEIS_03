\\ E.g.f. A(x) satisfies A(x) = exp( (1/3) * arcsinh(3*x*A(x)) ).
my(A=1, n=20); for(i=1, n, A= exp( (1/3) * asinh(3*x*A + 3*x*O(x^n)) )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(x) = (1 + 6*x*A(x)^4)^(1/6).
my(A=1, n=20); for(i=1, n, A= (1 + 6*x*A^4 + x*O(x^n))^(1/6)); Vec(serlaplace(A))