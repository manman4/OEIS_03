\\ E.g.f. satisfies A(x) = (1+x) * exp(x * A(x)^2 / (1+x)).
my(A=1, n=22); for(i=1, n, A= (1+x) * exp(x*A^2/(1+x)) + x*O(x^n) ); Vec(serlaplace(A))



