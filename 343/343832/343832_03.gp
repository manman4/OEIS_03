M=30;

a(n) = (n-1)!*sum(k=0, n-1, binomial(2*n-1, k)/(n-1-k)!);
for(n=1, M, print1(a(n), ", "));