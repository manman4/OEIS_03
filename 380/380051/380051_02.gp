\\ E.g.f. A(x) satisfies A(x) = ( 1 + 3*x*exp(x)*A(x) )^(1/3).
my(A=1, n=25); for(i=1, n, A= ( 1 + 3*x*exp(x + x*O(x^n))*A )^(1/3) ); Vec(serlaplace(A))

