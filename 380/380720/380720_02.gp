\\ E.g.f. A(x) satisfies A(x) = exp(x * A(x)^2 * (1 - x*A(x))^4) / (1 - x*A(x))^2.
my(A=1, n=22); for(i=1, n, A=exp(x * A^2 * (1 - x*A)^4 + x*O(x^n) ) / (1 - x*A)^2 ); Vec(serlaplace(A))



