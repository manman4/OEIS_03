M=18;

\\ a(n) = -Sum_{k=1..n} k^(n-k) * (n-1)^(k-1) * binomial(n,k) for n > 0.
a(n) = if(n==0, 1, -sum(k=1, n, k^(n-k) * (n-1)^(k-1) * binomial(n,k)));
for(n=0, M, print1(a(n),", "))                 

