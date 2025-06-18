\\ G.f. A(x) satisfies A(x) = 1 + 9*x*A(x)^(5/3).
my(A=1, n=22); for(i=1, n, A= 1 + 9 * x * A^(5/3) + x*O(x^n) ); Vec(A)

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^(7/3)).
my(A=1, n=22); for(i=1, n, A= 1 + 9 * x * A^(5/3) + x*O(x^n) ); Vec(1/subst(A, x, -x*A^(7/3)))


