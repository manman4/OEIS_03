M=13;

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} (3*k)!/6^k * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, (1/n) * sum(k=1, n, (3*k)!/6^k * binomial(n,k) * a(n-k)));
for(n=0, M, print1(a(n), ", "))

