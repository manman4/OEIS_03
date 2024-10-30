\\ E.g.f. satisfies A(x) = 1/(1 - A(x) * (exp(x*A(x)) - 1))^3.
my(A=1, n=32); for(i=1, n, A=1/(1 - A * (exp(x*A) - 1))^3 + x*O(x^n) ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 - A * (exp(x*A) - 1))^3 + x*O(x^n) ); Vec(serlaplace(A^(1/3)))


