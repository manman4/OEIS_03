M=30;

\\ G.f. A(x) satisfies A(x) = 1 + x/A(x) * (1 - A(x) + A(x)^4).
seq(n) = my(A=1); for(i=1, n, A=1 + x/A * (1 - A + A^4) +x*O(x^n)); Vec(A);
seq(M)      