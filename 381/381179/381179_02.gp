\\ E.g.f. A(x) satisfies A(x) = 1 + sinh(x*A(x)) / A(x).
my(A=1, n=25); for(i=1, n, A= 1 + sinh(x * A) / A + x*O(x^n) ); Vec(serlaplace(A))



