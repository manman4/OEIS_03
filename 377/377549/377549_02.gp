\\ E.g.f. satisfies A(x) = 1 + x*A(x)^5*exp(x*A(x)^2).
my(A=1, n=25); for(i=1, n, A=1 + x * A^5 * exp(x*A^2 +x*O(x^n)) ); Vec(serlaplace(A))


