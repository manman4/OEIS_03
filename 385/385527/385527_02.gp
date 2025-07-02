M=12;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (k+1) * 4^k * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, sum(k=0, n-1, (k+1) * 4^k * binomial(n-1, k) * a(k) * a(n-1-k)));
for(n=0, M, print1(a(n),", "));