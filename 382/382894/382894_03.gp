\\ G.f. A(x) satisfies A(x) = ( 1 + x * (1+x)^3 * A(x)^(3/2) )^2.
my(A=1, n=22); for(i=1, n, A= ( 1 + x * (1+x)^3 * A^(3/2) + x*O(x^n) )^2 ); Vec(A)







