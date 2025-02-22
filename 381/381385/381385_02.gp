\\ E.g.f. A(x) satisfies A(x) = 1/( 1 - sin(x * A(x)) / A(x) )^2.
my(A=1, n=22); for(i=1, n, A=1/( 1 - sin(x*A) / A + x*O(x^n))^2 ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A381384.
my(A=1, n=22); for(i=1, n, A=1/( 1 - sin(x*A) / A + x*O(x^n))^2 ); Vec(serlaplace(A^(1/2)))
