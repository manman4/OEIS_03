\\ a(n) = Sum_{k=0..n} n^k * binomial(2*n,n-k).		
a(n) = sum(k=0, n, n^k * binomial(2*n, n-k));
for(n=0, 18, print1(a(n), ", "))

