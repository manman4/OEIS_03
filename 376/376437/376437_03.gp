M=17;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 + x^2*A(x)^2 * log(1 - x*A(x)))^3.
my(A=1, n=M); for(i=1, n, A=1/(1 + x^2*A^2 * log(1 - x*A + x*O(x^n)))^3); Vec(serlaplace(A))


