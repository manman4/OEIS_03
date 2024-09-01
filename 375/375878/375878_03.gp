\\ 	E.g.f. satisfies A(x) = 1/(1 - x)^(2*A(x)^(1/2)).
my(A=1, n=22); for(i=1, n, A=1/(1-x)^(2*A^(1/2)) +x*O(x^n)  ); Vec(serlaplace(A))

my(A=1, n=22); for(i=1, n, A=1/(1-x)^(2*A^(1/2)) +x*O(x^n)  ); Vec(serlaplace(A^(1/2)))


