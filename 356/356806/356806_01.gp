M=17;

a(n) = sum(k=0, n, (k*n-1)^(n-k)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));