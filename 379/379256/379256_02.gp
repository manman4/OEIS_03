M=25;

\\ G.f. A(x) satisfies A(x) = 1 + x * A(x)^3 * (1 + A(x)^4).
seq(n) = my(A=1); for(i=1, n, A=1 + x * A^3 * (1 + A^4) + x*O(x^n)); Vec(A);
seq(M)


