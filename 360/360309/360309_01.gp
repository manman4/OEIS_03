M=37;

a(n) = sum(k=0, n\3, binomial(n-1-2*k, n-3*k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  
