M=30;

a(n) = (2*n+1)!*sum(k=0, n, (-1)^k*binomial(n, k)/(k+n+1)!);
for(n=0, M, print1(a(n), ", "));