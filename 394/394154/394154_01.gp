\\ For n > 0, a(n) = (1/n) * Sum_{j=0..floor((n-1)/2)} (-1)^j * binomial(n-1-j,j) * S(n,n-1-j), 
\\ where S(n,0) = 1 and S(n,k) = (n/k) * Sum_{j=1..k} 2^j * binomial(k,j) * binomial(n+k-1,j-1) for k > 0.
S(n,k) = if(k==0, 1, (n/k) * sum(j=1, k, 2^j * binomial(k,j) * binomial(n+k-1,j-1)));
a(n) = (1/n) * sum(j=0, (n-1)\2, (-1)^j * binomial(n-1-j,j) * S(n,n-1-j));
for(n=1, 20, print1(a(n),", "));

