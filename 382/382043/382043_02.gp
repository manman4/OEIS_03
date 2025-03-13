\\ E.g.f. A(x) satisfies A(x) = 1 + x*A(x)^3*exp(2*x*A(x)).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^3*exp(2*x*A +x*O(x^n)) ); Vec(serlaplace(A));
seq(30)

