M=8;

a(n) = sum(k=0, n, k^(k*n)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));