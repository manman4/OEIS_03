seq(n) = my(A=1); for(i=1, n, A=exp(sum(k=1, i, (3+subst(A, x, x^k))*x^k/k)+x*O(x^n))); Vec((1-x)^3 * A);
seq(24)

b(n) = if(n==0, 1, (1/n) * sum(k=1, n, (3 + sumdiv(k, d, d*b(d-1))) * b(n-k)));
a(n) = sum(k=0, 3, (-1)^k * binomial(3,k) * b(n-k));
for(n=0, 13, print1(a(n),", "))
