M=19;

a(n) = sum(k=0, n\2, 2^(k*(n-k))*binomial(n-k, k));
for(n=0, M, print1(a(n), ", "));