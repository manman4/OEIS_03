\\ E.g.f. satisfies A(x) = 1/(1 + A(x) * log(1 - x))^4.
my(A=1, n=32); for(i=1, n, A=1/(1 + A*log(1 - x + x*O(x^n)))^4 ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 + A*log(1 - x + x*O(x^n)))^4 ); Vec(serlaplace(A^(1/4)))