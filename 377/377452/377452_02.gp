\\ E.g.f. satisfies A(x) = 1/(1 - A(x) * (exp(x) - 1))^2.
my(A=1, n=32); for(i=1, n, A=1/(1 - A*(exp(x + x*O(x^n)) - 1))^2 ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 - A*(exp(x + x*O(x^n)) - 1))^2 ); Vec(serlaplace(A^(1/2)))
