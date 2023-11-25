M=20;

a(n) = sum(k=0, n, binomial(3*k+2,n-k) * binomial(4*k,k)/(3*k+1) );
for(n=0, M, print1(a(n),", "))