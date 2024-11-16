\\ E.g.f. satisfies A(x) = exp(x * (1-x)^3 * A(x)) / (1-x)^4.
my(A=1, n=22); for(i=1, n, A= exp(x*(1-x)^3*A) / (1-x)^4 + x*O(x^n) ); Vec(serlaplace(A))

