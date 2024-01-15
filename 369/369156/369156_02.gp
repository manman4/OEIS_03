M=21;

a(n) = sum(k=0, n\4, (-1)^k * binomial(n+1,k) * binomial(4*n-4*k+4,n-4*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
