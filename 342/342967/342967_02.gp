M=20;

a(n) = sum(j=0, n, prod(k=0, n-1, binomial(n+k, j)/binomial(j+k, j)));
for(n=0, M, print1(a(n), ", "));