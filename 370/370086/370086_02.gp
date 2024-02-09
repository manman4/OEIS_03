M=16;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} (2*k)!/2^k * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, (1/n) * sum(k=1, n, (2*k)!/2^k * binomial(n,k) * a(n-k)));
for(n=0, M, print1(a(n), ", "))

