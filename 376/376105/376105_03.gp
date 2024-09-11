\\ E.g.f. A(x) satisfies A(x) = x * (-A(x) + exp(3*A(x))).
seq(n) = my(A=x); for(i=1, n, A=x * (-A + exp(3*A + x*O(x^n))) ); Vec(serlaplace(A));
concat(0, seq(20))         