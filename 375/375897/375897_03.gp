\\ E.g.f. satisfies A(x) = 1 / (2 - exp(x * A(x)^(1/2)))^2.
my(A=1, n=22); for(i=1, n, A=1/(2-exp(x*A^(1/2)) +x*O(x^n))^2  ); Vec(serlaplace(A))

my(A=1, n=22); for(i=1, n, A=1/(2-exp(x*A^(1/2)) +x*O(x^n))^2  ); Vec(serlaplace(A^(1/2)))
