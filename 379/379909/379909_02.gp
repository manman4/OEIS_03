\\ E.g.f. A(x) satisfies A(x) = exp(-x*A(x)^2) + x*A(x).
my(A=1, n=25); for(i=1, n, A=exp(-x*A^2 + x*O(x^n)) + x*A ); Vec(serlaplace(A))
