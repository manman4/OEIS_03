M=16;

a(n) = sum(k=0, n, k^(3*(n-k))*binomial(n, k));
for(n=0, M, print1(a(n), ", "));