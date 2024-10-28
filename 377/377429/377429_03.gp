\\ E.g.f. A(x) satisfies A(x) = 1/(1 + log(1 - x*A(x)))^4.
my(A=1, n=32); for(i=1, n, A=1/(1 + log(1 - x*A))^4 +x*O(x^n) ); Vec(serlaplace(A))

my(A=1, n=32); for(i=1, n, A=1/(1 + log(1 - x*A))^4 +x*O(x^n) ); Vec(serlaplace(A^(1/4)))