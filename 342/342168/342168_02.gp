M=20;

a(n) = sum(k=0, n, (n+1)^(n-k)*binomial(2*n+1-k, k));
for(n=0, M, print1(a(n), ", "));