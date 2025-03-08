a(n) = sum(k=0, n\2, (-1)^k*binomial(3*n+k, k)*binomial(4*n-k, n-2*k))/(3*n+1);
for(n=0, 24, print1(a(n), ", "))

