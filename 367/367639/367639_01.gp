M=25;

a(n) = sum(k=0, n, binomial(k+2,n-k) * binomial(2*k,k)/(k+1) );
for(n=0, M, print1(a(n),", "))