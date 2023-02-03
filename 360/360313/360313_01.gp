M=34;

a(n) = sum(k=0, n\2, (-1)^k*binomial(n-1-k, n-2*k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  
