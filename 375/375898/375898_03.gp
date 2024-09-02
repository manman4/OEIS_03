\\ E.g.f. satisfies A(x) = 1 / (2 - exp(x * A(x)^(1/3)))^3.
my(A=1, n=22); for(i=1, n, A=1/(2-exp(x*A^(1/3)) +x*O(x^n))^3  ); Vec(serlaplace(A))

my(A=1, n=22); for(i=1, n, A=1/(2-exp(x*A^(1/3)) +x*O(x^n))^3  ); Vec(serlaplace(A^(1/3)))

