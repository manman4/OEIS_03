seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, subst(A, x, 2*x^k)^2*x^k/k)+x*O(x^n))); Vec(A);
seq(14)    


