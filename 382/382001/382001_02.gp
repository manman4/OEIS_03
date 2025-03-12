\\ E.g.f. A(x) satisfies A(x) = 1 + x*exp(2*x)*A(x)^6.
my(A=1, n=25); for(i=1, n, A=1 + x*exp(2*x + x*O(x^n)) * A^6); Vec(serlaplace(A))


