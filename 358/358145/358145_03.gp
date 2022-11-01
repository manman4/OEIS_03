M=16;

a(n) = sum(k=0, n, n^(n-k)*binomial((n-1)*n+k, k));
for(n=0, M, print1(a(n), ", "));