M=21;

\\ G.f. A(x) satisfies A(x) = 1/(1-x)^3 + x^2*A(x)*A'(x).
my(A=1, n=M); for(i=1, n, A= 1/( 1 - x + x*O(x^n) )^3 + x^2 * A * deriv(A) ); Vec(A)  