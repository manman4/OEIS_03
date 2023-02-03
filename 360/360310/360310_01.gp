M=39;

a(n) = sum(k=0, n\4, binomial(n-1-3*k, n-4*k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  
