\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)^3/(1 - x*A(x)^5).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^3/(1 - x*A^5) + x*O(x^n)); Vec(A);
seq(28)


