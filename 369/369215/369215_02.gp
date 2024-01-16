M=20;

a(n) = sum(k=0, n, binomial(n+k,k) * binomial(4*n+2*k+2,n-k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
