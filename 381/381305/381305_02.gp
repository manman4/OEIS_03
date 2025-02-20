\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - sin(x * A(x)^(1/2)) / A(x)^(3/2) ).
my(A=1, n=22); for(i=1, n, A=1/( 1 - sin(x*A^(1/2)) / A^(3/2) + x*O(x^n) ) ); Vec(serlaplace(A))
