\\ 	E.g.f. satisfies A(x) = exp( 3 * (exp(x*A(x)^(2/3)) - 1) ).
my(A=1, n=22); for(i=1, n, A=exp( 3 * (exp(x*A^(2/3)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A))

my(A=1, n=22); for(i=1, n, A=exp( 3 * (exp(x*A^(2/3)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A^(1/3)))
