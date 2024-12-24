M=19;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * binomial(2*(n+1),k) * binomial(2*(n+1),n-k).
a(n) = sum(k=0, n, 2^k * binomial(2*(n+1),k) * binomial(2*(n+1),n-k)/(n+1));
for(n=0, M, print1(a(n)", "))

\\ A219538(n) = (1/n) * Sum_{k=0..floor(n-1)/2} 2^(n-2*k) * binomial(n,k) * binomial(3*n-k,n-1-2*k) for n > 0. 
a219538(n) = if(n==0, 1, sum(k=0, (n-1)\2, 2^(n-2*k)*binomial(n,k)*binomial(3*n-k,n-1-2*k))/n);   
b(n) = a219538(n+1)/2; 
for(n=0, 100, print1(a(n)-b(n),", ")) 