M=19;

\\ G.f. A(x) satisfies A(x) = 1/(1 - x*A(x)^4 - x^2*A(x)^3*A'(x)).
my(A=1, n=M); for(i=1, n, A= 1/(1 - x*A^4 - x^2*A^3*deriv(A) + x*O(x^n)) ); Vec(A)

