\\ E.g.f. A(x) satisfies A(x) = exp(-x) + x*A(x)^3.
my(A=1, n=25); for(i=1, n, A=exp(-x + x*O(x^n)) + x*A^3 ); Vec(serlaplace(A))



