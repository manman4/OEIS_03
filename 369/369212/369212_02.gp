M=26;

a(n) = sum(k=0, n\3, binomial(n+1,k) * binomial(2*n-2*k+2,n-3*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
