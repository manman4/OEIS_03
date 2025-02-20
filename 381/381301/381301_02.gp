\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - x * cos(x * A(x)^(1/2)) / A(x) ).
my(A=1, n=22); for(i=1, n, A=1/( 1 - x*cos(x*A^(1/2)) / A + x*O(x^n)) ); Vec(serlaplace(A))


