\\ G.f. A(x) satisfies A(x) = 1 + 9*x*A(x)^(1/3).
my(A=1, n=22); for(i=1, n, A= 1 + 9 * x * A^(1/3) + x*O(x^n) ); Vec(A)

\\ G.f.: 1/B(-x), where B(x) is the g.f. of A135864.
my(A=1, n=22); for(i=1, n, A= 1 + 9 * x * A^(1/3) + x*O(x^n) ); Vec(1/subst(A, x, -x))


