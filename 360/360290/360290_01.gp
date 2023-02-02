M=25;

a(n) = sum(k=0, n\2, binomial(n-1-k, k)*binomial(2*n-4*k, n-2*k));
for(n=0, M, print1(a(n), ", "));  

