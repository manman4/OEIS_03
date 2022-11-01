M=16;

a(n) = sum(k=0, n, binomial(n*k, k)*binomial(n*(n-k), n-k));
for(n=0, M, print1(a(n), ", "));