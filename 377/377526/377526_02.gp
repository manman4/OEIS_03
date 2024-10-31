\\ E.g.f. satisfies A(x) = 1 + x*exp(x)*A(x)^5.
my(A=1, n=32); for(i=1, n, A=1 + x * exp(x + x*O(x^n)) * A^5 ); Vec(serlaplace(A))


