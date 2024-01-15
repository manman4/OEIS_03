M=23;

a(n) = sum(k=0, n\4, binomial(n+k,k) * binomial(3*n-2*k+1,n-4*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
