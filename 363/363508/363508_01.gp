seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, (4+subst(A, x, x^k))*x^k/k)+x*O(x^n))); Vec(A);
seq(22)    


