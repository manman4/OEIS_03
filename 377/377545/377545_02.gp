\\ E.g.f. satisfies A(x) = 1/(1 - x * exp(x*A(x)))^3.
my(A=1, n=25); for(i=1, n, A=1/(1 - x * exp(x*A))^3 + x*O(x^n) ); Vec(serlaplace(A))

my(A=1, n=25); for(i=1, n, A=1/(1 - x * exp(x*A))^3 + x*O(x^n) ); Vec(serlaplace(A^(1/3)))

