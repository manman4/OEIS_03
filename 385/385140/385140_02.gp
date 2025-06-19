\\ E.g.f. A(x) satisfies A(x) = exp(2*x*A(-x)^(1/2)).
my(A=1, n=22); for(i=1, n, A = exp(2*x*subst(A, x, -x)^(1/2) + x*O(x^n)) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A141369.
my(A=1, n=22); for(i=1, n, A = exp(2*x*subst(A, x, -x)^(1/2) + x*O(x^n)) ); Vec(serlaplace(A^(1/2)))




