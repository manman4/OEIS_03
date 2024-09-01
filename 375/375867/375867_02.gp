\\ E.g.f. satisfies A(x) = exp( 2 * (exp(x*A(x)^(1/2)) - 1) ).
my(A=1, n=22); for(i=1, n, A=exp( 2 * (exp(x*A^(1/2)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A))


