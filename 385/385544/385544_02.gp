\\ G.f. A(x) satisfies A(x) = 1 + Sum_{k>=1} k * x^k * A(k*x).
my(A=1, n=20); for(i=1, n, A= 1 + sum(k=1, i, k*x^k*subst(A, x, k*x) + x*O(x^n))); Vec(A)

