M=26;

a(n) = sum(k=0, n\4, (-1)^k*binomial(n-1-3*k, k)*binomial(2*n-8*k, n-4*k));
for(n=0, M, print1(a(n), ", "));  

