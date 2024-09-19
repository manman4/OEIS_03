\\ E.g.f. satisfies A(x) = 1 + (x*A(x))^3 * (exp(x*A(x)) - 1).
seq(n) = my(A=1); for(i=1, n, A=1 + (x*A)^3 * (exp(x*A+ x*O(x^n))-1) ); Vec(serlaplace(A));
seq(20)