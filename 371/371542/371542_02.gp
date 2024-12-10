seq(n) = my(A=1); for(i=1, n, A=(1 + x*A/(1+x) +x*O(x^n) )^3 ); Vec(A);
seq(25)

\\ G.f.: A(x) = (1 + B(x))^3 where B(x) is the g.f. of A127897.
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A/(1+x) +x*O(x^n) )^3 ); Vec(A^(1/3));
seq(25)