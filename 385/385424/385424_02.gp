\\ E.g.f. A(x) satisfies A(x) = exp( arcsin(x) * A(x) ).
my(A=1, n=20); for(i=1, n, A= exp( asin(x + x*O(x^n)) * A )); Vec(serlaplace(A))

