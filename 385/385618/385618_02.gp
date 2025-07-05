\\ G.f. A(x) satisfies A(x) = 1/( 1 - x*(A(2*x) + A(3*x)) ).
my(A=1, n=20); for(i=1, n, A= 1/( 1 - x*(subst(A, x, 2*x) + subst(A, x, 3*x)) + x*O(x^n) )); Vec(A)

