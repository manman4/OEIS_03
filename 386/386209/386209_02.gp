M=19;

\\ G.f. A(x) satisfies A(x) = 1/(1-x)^2 + x^2 * (d/dx A(x)^2).
my(A=1, n=M); for(i=1, n, A= 1/( 1 - x + x*O(x^n) )^2 + x^2 * deriv(A^2) ); Vec(A)  