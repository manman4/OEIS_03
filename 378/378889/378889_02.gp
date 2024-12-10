\\ G.f. A(x) satisfies A(x) = ( 1 + x*A(x)^(4/3)/(1 + x*A(x)^(1/3)) )^3.
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^(4/3)/(1 + x*A^(1/3)) +x*O(x^n) )^3 ); Vec(A);
seq(25)

\\ G.f.: A(x) = B(x)^3 where B(x) is the g.f. of A364758.
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^(4/3)/(1 + x*A^(1/3)) +x*O(x^n) )^3 ); Vec(A^(1/3));
seq(25)