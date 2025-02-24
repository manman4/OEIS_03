\\ E.g.f. A(x) satisfies A(x) = (1 + sinh(x*A(x)))^3.
my(A=1, n=22); for(i=1, n, A=(1 + sinh(x*A))^3 + x*O(x^n) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A381430.
my(A=1, n=22); for(i=1, n, A=(1 + sinh(x*A))^3 + x*O(x^n) ); Vec(serlaplace(A^(1/3)))

