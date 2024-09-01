\\ E.g.f. satisfies A(x) = exp( 4 * (exp(x*A(x)^(3/4)) - 1) ).
my(A=1, n=22); for(i=1, n, A=exp( 4 * (exp(x*A^(3/4)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A))


my(A=1, n=22); for(i=1, n, A=exp( 4 * (exp(x*A^(3/4)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A^(1/4)))
