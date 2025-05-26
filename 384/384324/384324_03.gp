\\ G.f. A(x) satisfies A(x) = A(x/(1-x))^(2/3) / (1-x)^2.
my(A=1+x*O(x^20), n=500); for(i=1, n, A = subst(A, x, x/(1-x))^(2/3) / (1-x)^2 ); apply(round, Vec(A))

\\ G.f.: B(x)^6, where B(x) is the g.f. of A090351.
my(A=1+x*O(x^20), n=500); for(i=1, n, A = subst(A, x, x/(1-x))^(2/3) / (1-x)^2 ); apply(round, Vec(A^(1/6)))
