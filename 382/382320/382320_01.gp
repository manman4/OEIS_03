M=20;

seq(n) = my(A=x); for(i=1, n, A=x+sum(k=1, i, subst(A, x, x^k))^2+x*O(x^n)); Vec(A);
seq(M)  