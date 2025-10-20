\\ G.f. A(x) satisfies A(x) = 1 + x*(1-x^3)^2*A(x)^3.
my(A=1, n=30); for(i=1, n, A=1 + x * (1-x^3)^2*A^3 + x*O(x^n)); Vec(A)
