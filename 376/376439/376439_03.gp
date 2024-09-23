M=18;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 - x^2*A(x)^2 * (exp(x*A(x)) - 1))^3.
my(A=1, n=M); for(i=1, n, A=1/(1 - x^2*A^2 * (exp(x*A + x*O(x^n)) - 1))^3); Vec(serlaplace(A))


