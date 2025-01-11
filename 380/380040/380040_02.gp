\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - 3*x*exp(x*A(x)) )^(2/3).
my(A=1, n=25); for(i=1, n, A=1 / (1 - 3*x*exp(x*A + x*O(x^n)) )^(2/3) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A380041.
my(A=1, n=25); for(i=1, n, A=1 / (1 - 3*x*exp(x*A + x*O(x^n)) )^(2/3) ); Vec(serlaplace(A^(1/2)))
