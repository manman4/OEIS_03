\\ E.g.f. A(x) satisfies A(x) = 1/(1 - 4*x*A(x)^3)^(1/4).
my(A=1, n=40); for(i=1, n, A=1/(1 - 4*x*A^3)^(1/4) + O(x^n)); Vec(serlaplace(A))