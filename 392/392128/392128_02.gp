\\ E.g.f. A(x) satisfies A(x) = (1 + 5*x*A(x)^3)^(1/5).
my(A=1, n=40); for(i=1, n, A=(1 + 5*x*A^3)^(1/5) + O(x^n)); Vec(serlaplace(A))