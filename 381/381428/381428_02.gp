\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - sinh(x) * A(x)^2 ).
my(A=1, n=22); for(i=1, n, A=1/( 1 - sinh(x) * A^2 + x*O(x^n) ) ); Vec(serlaplace(A))
