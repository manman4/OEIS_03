\\ E.g.f. A(x) satisfies A(x) = (1 + x*A(x))^4 * exp(4 * x * A(x)).
my(A=1, n=20); for(i=1, n, A = (1 + x*A)^4 * exp(4 * x * A) + x*O(x^n) ); Vec(serlaplace(A))

