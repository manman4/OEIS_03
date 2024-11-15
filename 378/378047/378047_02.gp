\\ E.g.f. satisfies A(x) = (1+x)^2 * exp(x * A(x) / (1+x)).
my(A=1, n=22); for(i=1, n, A= (1+x)^2 * exp(x*A/(1+x)) + x*O(x^n) ); Vec(serlaplace(A))


