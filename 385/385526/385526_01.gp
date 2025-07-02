\\ E.g.f. A(x) satisfies A(x) = exp(x*A(3*x)).
my(A=1, n=20); for(i=1, n, A= exp( x*subst(A, x, 3*x) + x*O(x^n) )); Vec(serlaplace(A))

