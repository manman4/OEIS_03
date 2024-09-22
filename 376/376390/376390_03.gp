M=17;

\\ E.g.f. A(x) satisfies A(x) = 1/(2 - exp(x*A(x)))^3.
my(A=1, n=M); for(i=1, n, A=1/(2 - exp(x*A + x*O(x^n)))^3); Vec(serlaplace(A))

\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A367135.
my(A=1, n=M); for(i=1, n, A=1/(2 - exp(x*A + x*O(x^n)))^3); Vec(serlaplace(A^(1/3)))
