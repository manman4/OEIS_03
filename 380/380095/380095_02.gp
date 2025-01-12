\\ E.g.f. A(x) satisfies A(x) = 1/sqrt( 1 - 2*x*A(x)^2*exp(x*A(x)^2) ).
my(A=1, n=25); for(i=1, n, A= 1/sqrt( 1 - 2*x*A^2*exp(x*A^2) + x*O(x^n) ) ); Vec(serlaplace(A))

