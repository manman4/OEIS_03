\\ E.g.f. A(x) satisfies A(x) = (1 + x*A(x))^2 * exp(2 * x * A(x)).
my(A=1, n=25); for(i=1, n, A = (1 + x*A)^2 * exp(2 * x * A) + x*O(x^n) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A377892.
my(A=1, n=25); for(i=1, n, A = (1 + x*A)^2 * exp(2 * x * A) + x*O(x^n) ); Vec(serlaplace(A^(1/2)))
