\\ E.g.f. A(x) satisfies A(x) = 1/( exp(-x*A(x)) - x )^2.
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x*A + x*O(x^n)) - x)^2 ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A379884.
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x*A + x*O(x^n)) - x)^2 ); Vec(serlaplace(A^(1/2)))
