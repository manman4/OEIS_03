\\ a(n) = Sum_{k=0..n} (k/n)^2 * binomial(n,k)^5.
a(n) = sum(k=0, n, (k/n)^2 * binomial(n,k)^5);
for(n=1, 17, print1(a(n), ", "))