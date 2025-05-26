\\ G.f. A(x) satisfies A(x) = A(x/(1-x))^(3/4) / (1-x)^3.
my(A=1+x*O(x^18), n=600); for(i=1, n, A = subst(A, x, x/(1-x))^(3/4) / (1-x)^3 ); apply(round, Vec(A))

\\ G.f.: B(x)^12, where B(x) is the g.f. of A090353.
my(A=1+x*O(x^18), n=600); for(i=1, n, A = subst(A, x, x/(1-x))^(3/4) / (1-x)^3 ); apply(round, Vec(A^(1/12)))
