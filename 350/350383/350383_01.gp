M=29;

a(n) = sum(k=0, n, (-1)^(n-k)*binomial(n-1+k, k)*binomial(n, 3*k));
for(n=0, M, print1(a(n), ", "));