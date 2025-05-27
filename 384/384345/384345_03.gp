\\ G.f. A(x) satisfies A(x) = (1+x)^(1/5) * A(x/(1+x))^(4/5).
my(A=1+x*O(x^18), n=600); for(i=1, n, A = (1+x)^(1/5) *subst(A, x, x/(1+x))^(4/5) ); apply(round, Vec(A))

