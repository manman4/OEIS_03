\\ E.g.f. A(x) satisfies A(x) = 1/sqrt( 1 - 2*x*exp(x*A(x)) ).
my(A=1, n=25); for(i=1, n, A=1 / (1 - 2*x*exp(x*A + x*O(x^n)) )^(1/2) ); Vec(serlaplace(A))

