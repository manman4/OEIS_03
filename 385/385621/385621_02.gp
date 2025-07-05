\\ E.g.f. A(x) satisfies A(x) = exp( x*(A(x) + A(3*x))/2 ).
my(A=1, n=20); for(i=1, n, A= exp( x*(A + subst(A, x, 3*x))/2 + x*O(x^n) )); Vec(serlaplace(A))

