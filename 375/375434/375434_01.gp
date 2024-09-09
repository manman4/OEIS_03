\\ a(n) = Sum_{k=0..n} 3^k * binomial(n+k+1,k) * binomial(n+k+1,n-k) / (n+k+1). 
a(n) = sum(k=0, n, 3^k * binomial(n+k+1, k) * binomial(n+k+1, n-k) / (n+k+1));
for(n=0, 30, print1(a(n),", ")) 

