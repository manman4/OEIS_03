M=22;

a(n) = sum(k=0, n\4, binomial(n+1,k) * binomial(3*n-3*k+3,n-4*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
