\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - 3*x*exp(x*A(x)^3) )^(1/3).
my(A=1, n=25); for(i=1, n, A=1 / (1 - 3*x*exp(x*A^3 + x*O(x^n)) )^(1/3) ); Vec(serlaplace(A))
