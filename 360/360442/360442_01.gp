M=19;

a(n) = sum(k=1, n, k^(n-k)*(-n)^(k-1)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));