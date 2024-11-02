\\ E.g.f. satisfies A(x) = 1 + x * exp(x*A(x)^4).
my(A=1, n=25); for(i=1, n, A=1 + x * exp(x*A^4 + x*O(x^n)) ); Vec(serlaplace(A))


