\\ E.g.f. A(x) satisfies A(x) = 1 + 2*x*exp(x)*A(x)^(1/2).
my(A=1, n=25); for(i=1, n, A= 1 + 2*x*exp(x + x*O(x^n))*sqrt(A) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A380050.
my(A=1, n=25); for(i=1, n, A= 1 + 2*x*exp(x + x*O(x^n))*sqrt(A) ); Vec(serlaplace(A^(1/2)))

