\\ E.g.f. A(x) satisfies A(x) = Sum_{k>=0} x^k/k! * A(2*k*x).
my(A=1, n=20); for(i=1, n, A= sum(k=0, i, x^k/k!*subst(A, x, 2*k*x)) + x*O(x^n)); Vec(serlaplace(A))

