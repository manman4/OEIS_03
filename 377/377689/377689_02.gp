\\ E.g.f. satisfies A(x) = (1 + x * (exp(x*A(x)) - 1))^3.
my(A=1, n=25); for(i=1, n, A=(1 + x * (exp(x*A +x*O(x^n)) - 1))^3 ); Vec(serlaplace(A))

my(A=1, n=25); for(i=1, n, A=(1 + x * (exp(x*A +x*O(x^n)) - 1))^3 ); Vec(serlaplace(A^(1/3)))

