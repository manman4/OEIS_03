a(n) = sum(k=0, n, 2^k*binomial(3*n+1, k)*binomial(4*n-k, n-k))/(3*n+1);
for(n=0, 19, print1(a(n),", "))          

