seq(n) = my(A=1); for(i=1, n, A=exp(2*sum(k=1, i, subst(A, x, -x^k)*x^k/k)+x*O(x^n))); Vec(A);
seq(27)    


