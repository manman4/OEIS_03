M=20;

a(n) = if(n==0, 1, n*sum(k=0, n, (2*n+2)^k*binomial(n+k, 2*k)/(n+k)));
for(n=0, M, print1(a(n), ", "));