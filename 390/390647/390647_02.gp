M=20;

\\ E.g.f. A(x) satisfies A(x) = 1 + x^2*exp(x)*A(x)^3.
my(A=1, n=M); for(i=1, n, A=1 + x^2*exp(x + x*O(x^n)) * A^3); Vec(serlaplace(A))  