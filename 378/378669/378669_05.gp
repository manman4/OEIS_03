\\ G.f. A(x) satisfies A(x) = ( 1 + x*A(x)^(5/3)/(1 - x*A(x)^(4/3)) )^3.
my(A=1, n=22); for(i=1, n, A= ( 1 + x*A^(5/3)/(1 - x*A^(4/3)) )^3 + x*O(x^n) ); Vec(A)


 