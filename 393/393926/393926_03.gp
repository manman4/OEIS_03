\\ G.f. A(x) satisfies A(x) = 1 + 3*x/(1-x) - 2*x/(1-x)^2 + x*A(x)^2.
a(n) = my(A=1); for(i=0, n, A=1 + 3*x/(1-x) - 2*x/(1-x)^2 + x*A^2 +x*O(x^n)); Vec(A);
a(27)                  