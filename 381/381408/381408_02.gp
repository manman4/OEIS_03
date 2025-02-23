\\ E.g.f. A(x) satisfies A(x) = exp( 2 * x * cosh(x * A(x)) ).
my(A=1, n=22); for(i=1, n, A=exp( 2 * x * cosh(x * A) + x*O(x^n) ) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A381407.
my(A=1, n=22); for(i=1, n, A=exp( 2 * x * cosh(x * A) + x*O(x^n) ) ); Vec(serlaplace(A^(1/2)))