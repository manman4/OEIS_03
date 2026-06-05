\\ For n > 0, a(n) = (1/n) * Sum_{k=0..floor((n-1)/2)} (-1)^k * binomial(n-1-k,k) * S(n-1-k,n), 
\\ where S(0,k) = 1 and S(n,k) = (k/n) * Sum_{j=1..n} 2^j * binomial(n,j) * binomial(n+k-1,j-1) for n > 0. 
S(n,k) = if(n==0, 1, (k/n) * sum(j=1, n, 2^j * binomial(n,j) * binomial(n+k-1,j-1)));
matrix(7, 7, n, k, S(n-1, k-1))
a(n) = (1/n) * sum(k=0, (n-1)\2, (-1)^k * binomial(n-1-k, k) * S(n-1-k,n));
for(n=1, 23, print1(a(n),", "));

