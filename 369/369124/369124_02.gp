M=20;

a(n) = sum(k=0, n\4, (-1)^k * binomial(n+k,k) * binomial(5*n+3,n-4*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
