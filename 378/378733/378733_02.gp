\\ G.f. A(x) satisfies A(x) = 1 + x / (1 - x*A(x)^2)^4.
my(A=1, n=22); for(i=1, n, A= 1 + x/(1 - x*A^2)^4 + x*O(x^n) ); Vec(A)

\\ G.f.: A(x) = sqrt(B(x)) where B(x) is the g.f. of A365123.
my(A=1, n=22); for(i=1, n, A= 1 + x/(1 - x*A^2)^4 + x*O(x^n) ); Vec(A^2)
 