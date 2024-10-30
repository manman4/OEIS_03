\\ E.g.f. satisfies A(x) = 1/(1 + A(x)^2 * log(1 - x))^2.
my(A=1, n=32); for(i=1, n, A=1/(1 + A^2 * log(1 - x + x*O(x^n)))^2 ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 + A^2 * log(1 - x + x*O(x^n)))^2 ); Vec(serlaplace(A^(1/2)))
