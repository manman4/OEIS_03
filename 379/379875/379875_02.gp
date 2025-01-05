\\ E.g.f. A(x) satisfies A(x) = exp(-x*A(x)) + x.
my(A=1, n=25); for(i=1, n, A=exp(-x*A + x*O(x^n)) + x ); Vec(serlaplace(A))

