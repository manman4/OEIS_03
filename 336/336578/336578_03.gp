a(n) = sum(k=0, n, 2^k*binomial(n^2+1, k)*binomial((n+1)*n-k, n-k))/(n^2+1);
for(n=0, 19, print1(a(n),", "))          

