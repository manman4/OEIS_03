\\ G.f. A(x) satisfies A(x) = (1+x)^(1/2) * A(x/(1+x))^(1/2).
my(A=1+x*O(x^18), n=600); for(i=1, n, A = (1+x)^(1/2) *subst(A, x, x/(1+x))^(1/2) ); apply(round, Vec(A))

