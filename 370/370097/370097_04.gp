\\ a(n) = Sum_{k=0..n} 2^k * binomial(2*n+k-1,k). 
a(n) = sum(k=0, n, 2^k * binomial(2*n+k-1,k));
for(n=0, 20, print1(a(n), ", "));



