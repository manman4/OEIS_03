\\ E.g.f. A(x) satisfies A(x) = x * exp(3*A(x)) * (1 + A(x)).
seq(n) = my(A=x); for(i=1, n, A=x * exp(3*A + x*O(x^n)) * (1 + A) ); Vec(serlaplace(A));
concat(0, seq(20))         