seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, (3*(-1)^k+subst(A, x, x^k))*x^k/k)+x*O(x^n))); Vec((1+x)^3*A);
seq(35)

b(n) = if(n==0, 1, (1/n) * sum(k=1, n, (3 * (-1)^k + sumdiv(k, d, d*b(d-1))) * b(n-k)));
a(n) = sum(k=0, 3, binomial(3,k) * b(n-k));
for(n=0, 15, print1(a(n),", "))
