seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, subst(A, x, 3*x^k)*x^k/k)+x*O(x^n))); Vec(A);

seq(20)

