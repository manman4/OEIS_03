\\ E.g.f. satisfies A(x) = 1/(1 + A(x)^4 * log(1 - x)).
my(A=1, n=32); for(i=1, n, A=1/(1 + A^4 * log(1 - x + x*O(x^n))) ); Vec(serlaplace(A))

