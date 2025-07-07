\\ E.g.f. A(x) satisfies A(x) = exp( x*((A(x) + A(-x))/2)^3 ).
my(A=1, n=20); for(i=1, n, A= exp( x*((A + subst(A, x, -x))/2)^3 + x*O(x^n) )); Vec(serlaplace(A))

\\ E.g.f. A(x) satisfies A(-x) = 1/A(x).
my(A=1, n=20); for(i=1, n, A= exp( x*((A + subst(A, x, -x))/2)^3 + x*O(x^n) )); Vec(serlaplace(subst(A, x, -x) - 1/A))
