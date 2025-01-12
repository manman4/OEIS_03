\\ E.g.f. A(x) satisfies A(x) = sqrt( 1 + 2*x*exp(x*A(x)) ).
my(A=1, n=25); for(i=1, n, A= sqrt( 1 + 2*x*exp(x*A) + x*O(x^n) ) ); Vec(serlaplace(A))
