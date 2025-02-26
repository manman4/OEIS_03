\\ E.g.f. A(x) satisfies A(x) = (1 + x*A(x) * cos(x*A(x)))^2.
my(A=1, n=22); for(i=1, n, A=(1 + x*A*cos(x*A))^2 + x*O(x^n) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A381520.
my(A=1, n=22); for(i=1, n, A=(1 + x*A*cos(x*A))^2 + x*O(x^n) ); Vec(serlaplace(A^(1/2)))

