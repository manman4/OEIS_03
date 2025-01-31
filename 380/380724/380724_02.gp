\\ E.g.f. A(x) satisfies A(x) = exp(x * A(x)^3) / (1 - x*A(x)^3).
my(A=1, n=22); for(i=1, n, A=exp(x * A^3 + x*O(x^n) ) / (1 - x*A^3) ); Vec(serlaplace(A))


