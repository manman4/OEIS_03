\\ a(n) = (1/n) * Sum_{k=0..floor((n-1)/2)} binomial(n,k) * binomial(5*n-k,n-1-2*k) for n > 0.
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, binomial(n, k)*binomial(5*n-k, n-1-2*k))/n);
for(n=0, 20, print1(a(n),", ")) 



