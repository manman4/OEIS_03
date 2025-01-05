\\ E.g.f. A(x) satisfies A(x) = (exp(-x*A(x)) + x*A(x))^2.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A) + x*A)^2 + x*O(x^n) ); Vec(serlaplace(A))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A379868.
my(A=1, n=25); for(i=1, n, A=(exp(-x*A) + x*A)^2 + x*O(x^n) ); Vec(serlaplace(A^(1/2)))
