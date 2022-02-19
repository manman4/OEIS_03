M=19;

a(n) = if(n==0, 1, -3*n*sum(k=0, n-1, binomial(n-1, k)*a(k)));
for(n=0, M, print1(a(n), ", "));