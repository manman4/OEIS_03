\\ G.f. A(x) satisfies A(x) = 1/( 1 - x * (1+x)^2 * A(x) )^3.
my(A=1, n=22); for(i=1, n, A= 1/( 1 - x * (1+x)^2 * A )^3 + x*O(x^n) ); Vec(A)  

\\ G.f.: B(x)^3, where B(x) is the g.f. of A378786.
my(A=1, n=22); for(i=1, n, A= 1/( 1 - x * (1+x)^2 * A )^3 + x*O(x^n) ); Vec(A^(1/3))




