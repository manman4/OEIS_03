\\ G.f. A(x) satisfies A(x) = Sum_{k>=0} (k*x)^k * A(k*x).
my(A=1, n=20); for(i=1, n, A= sum(k=0, i, (k*x)^k*subst(A, x, k*x) + x*O(x^n))); Vec(A)

