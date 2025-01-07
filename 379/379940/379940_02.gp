\\ E.g.f. A(x) satisfies A(x) = 1/( exp(-x*A(x)^(2/3)) - x*A(x)^(1/3) )^3.
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x*A^(2/3) + x*O(x^n)) - x*A^(1/3))^3 ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A377892.
my(A=1, n=25); for(i=1, n, A=1 / (exp(-x*A^(2/3) + x*O(x^n)) - x*A^(1/3))^3 ); Vec(serlaplace(A^(1/3)))


