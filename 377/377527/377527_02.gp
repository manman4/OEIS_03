\\ E.g.f. satisfies A(x) = 1/(1 - x * exp(x) * A(x)^2)^2.
my(A=1, n=32); for(i=1, n, A=1/(1 - x * exp(x + x*O(x^n)) * A^2)^2 ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 - x * exp(x + x*O(x^n)) * A^2)^2 ); Vec(serlaplace(A^(1/2)))

