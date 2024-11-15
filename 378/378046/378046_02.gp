\\ E.g.f. satisfies A(x) = (1+x) * exp(x * A(x)^3 / (1+x)^2).
my(A=1, n=22); for(i=1, n, A= (1+x) * exp(x*A^3/(1+x)^2) + x*O(x^n) ); Vec(serlaplace(A))



