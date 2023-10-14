M=10;

a(n) = sum(k=0, 2*n, (-1)^(n+k)*binomial(2*n, k)^n);
for(n=0, M, print1(a(n), ", "));