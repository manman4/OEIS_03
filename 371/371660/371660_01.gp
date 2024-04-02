\\ a(n) = if(n==0, 1, (1/n)*sum(k=0, (n-1)\2, 3^(n-k)*binomial(n, k)*binomial(4*n-k, n-1-2*k)));
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, 3^(n-k) * binomial(n,k) * binomial(4*n-k,n-1-2*k))/n);
for(n=0, 17, print1(a(n),", ")) 


