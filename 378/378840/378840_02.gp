\\ G.f. A(x) satisfies A(x) = ( 1 + x * A(x)^(4/3) * (1 + A(x)^(1/3)) )^3.
my(A=1, n=22); for(i=1, n, A= (1 + x * A^(4/3) * (1 + A^(1/3)) )^3 + x*O(x^n) ); Vec(A)


\\ Conjecture: g.f.: B(x)^3, where B(x) is the g.f. of A260332.
my(A=1, n=22); for(i=1, n, A= (1 + x * A^(4/3) * (1 + A^(1/3)) )^3 + x*O(x^n) ); Vec(A^(1/3))
