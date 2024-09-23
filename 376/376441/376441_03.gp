M=17;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 + x*A(x) * log(1 - x^2*A(x)^2))^2.
my(A=1, n=M); for(i=1, n, A=1/(1 + x*A * log(1 - x^2*A^2 + x*O(x^n)))^2); Vec(serlaplace(A))

