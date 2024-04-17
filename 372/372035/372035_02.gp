\\ a(n) = Sum_{k=0..n} 4^k * binomial(k/2+1/2,k) * binomial(n-1,n-k)/(k+1).
a(n) = sum(k=0, n, 4^k * binomial(k/2+1/2,k) * binomial(n-1,n-k)/(k+1));
for(n=0, 35, print1(a(n), ", "))

\\ a(n) = 2 * A371888(n) for n > 0.
a371888(n) = if(n==0, 1, sum(k=0, n, binomial(n, k)*binomial(n-2*k, n-k-1))/n);
for(n=1, 35, print1(a(n) - 2*a371888(n), ", "))
