M=20;

a(n) = sum(k=0, n\2, binomial(n+1,k) * binomial(4*n-4*k+4,n-2*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
