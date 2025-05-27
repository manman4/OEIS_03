\\ G.f. A(x) satisfies A(x) = (1+x)^2 * A(x/(1+x))^(2/3).
my(A=1+x*O(x^18), n=600); for(i=1, n, A = (1+x)^2 *subst(A, x, x/(1+x))^(2/3) ); apply(round, Vec(A))

\\ G.f.: B(x)^6, where B(x) is the g.f. of A384344.
my(A=1+x*O(x^18), n=600); for(i=1, n, A = (1+x)^2 *subst(A, x, x/(1+x))^(2/3) ); apply(round, Vec(A^(1/6)))