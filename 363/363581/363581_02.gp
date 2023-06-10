seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, ((-3)^k+subst(A, x, x^k))*x^k/k)+x*O(x^n))); Vec((1+3*x)*A);
seq(24)

b(n) = if(n==0, 1, (1/n) * sum(k=1, n, ((-3)^k + sumdiv(k, d, d*b(d-1))) * b(n-k)));
a(n) = if(n==0, 1, b(n) + 3*b(n-1));
for(n=0, 15, print1(a(n),", "))


