\\ G.f. A(x) satisfies A(x) = 1 + x * (1+x)^2 * A(x)^4.
my(A=1, n=22); for(i=1, n, A= 1 + x * (1+x)^2 * A^4 + x*O(x^n) ); Vec(A)