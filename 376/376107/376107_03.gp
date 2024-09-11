\\ E.g.f. A(x) satisfies A(x) = x * (3*A(x) + exp(-A(x))).
seq(n) = my(A=x); for(i=1, n, A=x * (3*A + exp(-A + x*O(x^n))) ); Vec(serlaplace(A));
concat(0, seq(20))         