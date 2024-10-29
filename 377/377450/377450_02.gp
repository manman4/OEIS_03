\\ E.g.f. satisfies A(x) = 1/(1 - A(x)^3 * (exp(x) - 1)).
my(A=1, n=32); for(i=1, n, A=1/(1 - A^3*(exp(x + x*O(x^n)) - 1)) ); Vec(serlaplace(A))


