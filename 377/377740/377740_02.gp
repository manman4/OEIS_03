\\ E.g.f. satisfies A(x) = exp(x) * (1 + x * A(x))^2.
my(A=1, n=32); for(i=1, n, A=exp(x + x*O(x^n)) * (1 + x*A)^2 ); Vec(serlaplace(A))


