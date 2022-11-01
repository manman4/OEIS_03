M=16;

a(n) = sum(k=0, n, (n-1)^(n-k)*binomial(n^2+1, k));
for(n=0, M, print1(a(n), ", "));