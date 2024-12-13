M=26;

\\ a(n) = (1/n) * Sum_{k=0..n} (-1)^k * binomial(n,k) * binomial(n-4*k,n-k-1) for n > 0.
a(n) = if(n==0, 1, 1/n*sum(k=0, n, (-1)^k*binomial(n, k)*binomial(n-4*k, n-k-1)));
for(n=0, M, print1(a(n), ", "))
