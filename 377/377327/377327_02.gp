\\ E.g.f. satisfies A(x) = 1 - A(x)^2 * log(1 - x*A(x)^3).

my(A=1, n=32); for(i=1, n, A=1 - A^2 * log(1 - x*A^3) +x*O(x^n) ); Vec(serlaplace(A))



