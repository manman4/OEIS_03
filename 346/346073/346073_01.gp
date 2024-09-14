M=42;

a(n) = sum(k=0, n\4, binomial(n-3*k, k)*binomial(2*k, k)/(k+1)); 
for(n=0, M, print1(a(n), ", "));
