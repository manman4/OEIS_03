M=20;

\\ E.g.f. A(x) satisfies A(x) = 1 + x*exp(x^3)*A(x)^2.
my(A=1, n=M); for(i=1, n, A=1 + x*exp(x^3 + x*O(x^n)) * A^2); Vec(serlaplace(A))  