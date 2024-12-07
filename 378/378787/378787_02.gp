\\ G.f. A(x) satisfies A(x) = ( 1 + x * (1 + x*A(x)^2)^3 )^2.
my(A=1, n=22); for(i=1, n, A= (1 + x * (1 + x*A^2)^3 )^2 + x*O(x^n) ); Vec(A)

\\ G.f.: A(x) = B(x)^2 where B(x) is the g.f. of A137957.
my(A=1, n=22); for(i=1, n, A= (1 + x * (1 + x*A^2)^3 )^2 + x*O(x^n) ); Vec(A^(1/2))