seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A +x*O(x^n)))^2 ); Vec(A);         
seq(18)

\\ G.f.: A(x) = (1 + x*B(x))^2 where B(x) is the g.f. of A161634.
seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A +x*O(x^n)))^2 ); Vec(A^(1/2));
seq(18)
