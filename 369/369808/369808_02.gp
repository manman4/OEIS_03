M=34;

\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n-1+2*k,n-5*k).
a(n) = sum(k=0, n\5, binomial(n-1+2*k, n-5*k));
for(n=0, M, print1(a(n), ", "));