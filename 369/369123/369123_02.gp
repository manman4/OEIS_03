M=24;

a(n) = sum(k=0, n\2, (-1)^k * binomial(n+k,k) * binomial(3*n+1,n-2*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
