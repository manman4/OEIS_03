M=21;

\\ a(n) = Sum_{k=0..n} 2^(n-k) * ((2*k+1) * (2*k+3)/3) * binomial(2*k,k) * binomial(n+1,n-k).
a(n) = sum(k=0, n, 2^(n-k) * ((2*k+1) * (2*k+3)/3) * binomial(2*k,k) * binomial(n+1,n-k));
for(n=0, M, print1(a(n), ", "));