M=21;

a(n) = sum(k=0, n\4, binomial(n+k,k) * binomial(4*n-k+2,n-4*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
