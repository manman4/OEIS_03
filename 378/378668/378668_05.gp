\\ G.f. A(x) satisfies A(x) = ( 1 + x*A(x)^(5/2)/(1 - x*A(x)^2) )^2.
my(A=1, n=22); for(i=1, n, A= ( 1 + x*A^(5/2)/(1 - x*A^2) )^2 + x*O(x^n) ); Vec(A)

 