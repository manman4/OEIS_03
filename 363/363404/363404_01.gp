seq(n) = my(w=exp(2*Pi*I/3), A=1); for(i=1, n, A=exp(sum(k=1, i, sum(m=0, 2, subst(A, x, w^m*x^k))/3*x^k/k)+x*O(x^n))); apply(round, Vec(A));
seq(40)

