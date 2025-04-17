M=24;

\\ a(n) = Sum_{k=0..n} k^3 * 2^(n-k) * binomial(n,k).
a(n) = sum(k=0, n, k^3 * 2^(n-k) * binomial(n,k));
for(n=0, M, print1(a(n),", "))  
