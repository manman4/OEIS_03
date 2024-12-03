\\ G.f. A(x) satisfies A(x) = 1/( 1 - x*A(x)^(3/2)/(1 - x*A(x)^(3/2)) )^2.
my(A=1, n=22); for(i=1, n, A= 1/(1-x*A^(3/2)/(1-x*A^(3/2)))^2 + x*O(x^n) ); Vec(A)

\\ G.f.: B(x)^2 where B(x) is the g.f. of A243659.
my(A=1, n=22); for(i=1, n, A= 1/(1-x*A^(3/2)/(1-x*A^(3/2)))^2 + x*O(x^n) ); Vec(A^(1/2))

