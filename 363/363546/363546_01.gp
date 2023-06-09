seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, subst(A, x, x^k)*x^k/(k*(1-3*x^k)))+x*O(x^n))); Vec(A);
seq(23)


