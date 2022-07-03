M=13;

a(n) = sum(k=0, n, k^(k*(n-k))*binomial(n, k));
for(n=0, M, print1(a(n), ", "));