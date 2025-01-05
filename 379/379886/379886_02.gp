\\ E.g.f. A(x) satisfies A(x) = 1/(exp(-x) - x*A(x)^2).
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x + x*O(x^n)) - x*A^2) ); Vec(serlaplace(A))


