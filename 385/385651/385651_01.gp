\\ E.g.f. A(x) satisfies A(x) = exp( x*(A(x) + A(2*x))^3 ).
my(A=1, n=20); for(i=1, n, A= exp( x*(A + subst(A, x, 2*x))^3 + x*O(x^n) )); Vec(serlaplace(A))

