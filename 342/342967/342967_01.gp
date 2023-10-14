M=20;

a(n) = 1+sum(j=1, n, prod(k=0, j-1, binomial(2*n-1, n+k)/binomial(2*n-1, k)));
for(n=0, M, print1(a(n), ", "));