\\ G.f. A(x) satisfies A(x) = 1/(1-x) + x/(1-x)^4 + x*A(x)^2.
a(n) = my(A=1); for(i=0, n, A=1/(1-x) + x/(1-x)^4 + x*A^2 +x*O(x^n)); Vec(A);
a(24)                  