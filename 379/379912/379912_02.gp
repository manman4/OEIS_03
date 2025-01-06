\\ E.g.f. A(x) satisfies A(x) = 1/(exp(-x*A(x)^3) - x*A(x)).
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x*A^3 + x*O(x^n)) - x*A) ); Vec(serlaplace(A))

