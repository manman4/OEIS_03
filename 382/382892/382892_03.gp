\\ G.f. A(x) satisfies A(x) = ( 1 + x * (1+x)^3 * A(x)^(4/3) )^3.
my(A=1, n=22); for(i=1, n, A= ( 1 + x * (1+x)^3 * A^(4/3) + x*O(x^n) )^3 ); Vec(A)







