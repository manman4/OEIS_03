\\ E.g.f. satisfies A(x) = (1 + x)^2 * exp(x * A(x)).
my(A=1, n=32); for(i=1, n, A=(1 + x)^2 * exp(x * A) + x*O(x^n) ); Vec(serlaplace(A))

