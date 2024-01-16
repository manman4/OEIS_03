M=18;

a(n) = sum(k=0, n, binomial(n+k,k) * binomial(5*n+3*k+3,n-k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
