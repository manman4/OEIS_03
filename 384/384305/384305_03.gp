\\ G.f. A(x) satisfies A(x) = A(x/(1-x))^(5/6) / (1-x)^5.
my(A=1+x*O(x^18), n=600); for(i=1, n, A = subst(A, x, x/(1-x))^(5/6) / (1-x)^5 ); apply(round, Vec(A))

\\ G.f.: B(x)^30, where B(x) is the g.f. of A090358.
my(A=1+x*O(x^18), n=600); for(i=1, n, A = subst(A, x, x/(1-x))^(5/6) / (1-x)^5 ); apply(round, Vec(A^(1/30)))
