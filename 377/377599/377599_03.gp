\\ E.g.f. satisfies A(x) = exp( x * A(x) / (1-x)^2 ) / (1-x).
my(A=1, n=22); for(i=1, n, A= exp(x*A/(1-x)^2)/(1-x) + x*O(x^n) ); Vec(serlaplace(A))


