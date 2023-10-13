M=20;

a(n) = sum(k=0, n, (2*n)^k*binomial(n+1+k, 2*k+1));
for(n=0, M, print1(a(n), ", "));