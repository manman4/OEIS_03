\\ G.f. A(x) satisfies A(x) = ( 1 + x/(1 - x*A(x)^(3/4)) )^4.
seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A^(3/4) +x*O(x^n)))^4 ); Vec(A);
seq(18)

\\ G.f.: A(x) = (1 + x*B(x))^4 where B(x) is the g.f. of A364742.
seq(n) = my(A=1); for(i=1, n, A=(1 + x/(1 - x*A^(3/4) +x*O(x^n)))^4 ); Vec(A^(1/4));
seq(18)
