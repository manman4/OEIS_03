\\ E.g.f. A(x) satisfies A(x) = exp(x * A(x))/(1 - x*A(x)).
my(A=1, n=32); for(i=1, n, A=exp(x * A)/(1 - x*A) + x*O(x^n) ); Vec(serlaplace(A))

